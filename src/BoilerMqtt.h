#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <GyverHub.h>


class BoilerMqtt {

public:

    BoilerMqtt() {
        mqtt.setClient(mclient);
        mqtt.setBufferSize(GH_UPL_CHUNK_SIZE);
    }

    void config(const char* host, uint16_t port, const String& login = "", const String& pass = "", uint32_t mqId = 0, const String& prefix = "", uint8_t qos = 0, bool ret = 0) {
        if (!host) return;
        mq_host = host;
        mqtt.setServer(mq_host.c_str(), port);
        _config(login, pass, qos, ret);
        if (mqId <= 0xfffff) { mqId += 0xfffff; }
        ultoa(mqId, this->id, HEX);
        _prefix = prefix;
    }

    bool online() {
        return mqtt.connected();
    }

    void tick() {
        if (mq_configured) {
            if (!mqtt.connected() && (!mqtt_tmr || millis() - mqtt_tmr > GH_MQTT_RECONNECT)) {
                mqtt_tmr = millis();
                _connect();
            }
            mqtt.loop();
        }
    }

private:

    PubSubClient mqtt;
    WiFiClient mclient;
    bool mq_configured = false;
    uint32_t mqtt_tmr = 0;
    uint8_t qos = 0;
    bool ret = 0;
    String mq_host;
    String mq_login;
    String mq_pass;

    char* id;
    String _prefix;

    void begin() {
        mqtt.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
            uint16_t tlen = strlen(topic);
            char topic_buf[tlen + 1];
            memcpy(topic_buf, topic, tlen);
            topic_buf[tlen] = 0;

            char data_buf[len + 1];
            memcpy(data_buf, data, len);
            data_buf[len] = 0;

            parse(GHTXT(topic_buf, tlen), GHTXT(data_buf, len));
        });
    }

    void end() {
        mqtt.disconnect();
    }

    void send(gh::BridgeData& data) {
        if (!mqtt.connected()) return;
        mqtt.beginPublish(data.topic.c_str(), data.text.length(), ret);
        mqtt.print(data.text);
        mqtt.endPublish();
    }

    void _connect() {
        String m_id("hub_");
        m_id += String(random(0xffffff), HEX);
        bool ok = 0;

        String status(_prefix);
        status += F("/hub/");
        status += id;
        status += F("/status");

        String offline(F("offline"));

        if (mq_login.length()) {
            ok = mqtt.connect(m_id.c_str(), mq_login.c_str(), mq_pass.c_str(), status.c_str(), qos, ret, offline.c_str());
        } else {
            ok = mqtt.connect(m_id.c_str(), status.c_str(), qos, ret, offline.c_str());
        } 
            

        if (ok) {
            // String sub_topic(_prefix);
            // mqtt.subscribe(sub_topic.c_str(), qos);

            // sub_topic += '/';
            // sub_topic += id;
            // sub_topic += "/#";
            // mqtt.subscribe(sub_topic.c_str(), qos);
        }
    }

    void parse(GHTXT url, GHTXT data = GHTXT()) {
        if (!url.valid()) { return; }
        if (!data.valid()) {
            char* eq = (char*)url.find('=');
            if (eq) {
                *eq = 0;
                data = GHTXT(eq + 1, url.end() - eq - 1);
                url = GHTXT(url.str(), eq - url.str());
            }
        }
//        _parse(_hub, *this, url, data);
    }

    void _config(const String& login, const String& pass, uint8_t nqos, bool nret) {
        qos = nqos;
        ret = nret;
        mq_login = login;
        mq_pass = pass;
        mq_configured = true;
    }
    
};
