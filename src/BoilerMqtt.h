#pragma once
#include <Arduino.h>
#include <WiFiClient.h>
#include <GyverHub.h>


class BoilerMqtt {

public:

    BoilerMqtt() {

    }

    void config(const char* host, uint16_t port, const String& login = "", const String& pass = "", uint32_t mqId = 0, const String& prefix = "", uint8_t qos = 0, bool ret = 0) {
        if (!host || _configured) { return; }
        _host = host;
        _prefix = prefix;
        _mqtt.setClient(_mclient);
        _mqtt.setBufferSize(GH_UPL_CHUNK_SIZE);
        _mqtt.setServer(_host.c_str(), port);
        _config(login, pass, qos, ret);
        if (mqId <= 0xfffff) { mqId += 0xfffff; }
        ultoa(mqId, this->_id, HEX);
    }

    bool online() {
        return _mqtt.connected();
    }

    void tick() {
        if (_configured) {
            if (!_mqtt.connected() && (!mqtt_tmr || millis() - mqtt_tmr > GH_MQTT_RECONNECT)) {
                mqtt_tmr = millis();
                _connect();
            }
            _mqtt.loop();
        }
    }

    void send(gh::BridgeData& data) {
        if (!_mqtt.connected()) return;
        _mqtt.beginPublish(data.topic.c_str(), data.text.length(), false);
        _mqtt.print(data.text);
        _mqtt.endPublish();
    }

    // void onMessage(std::function<void (GHTXT, GHTXT)> func) {
    //     _parseCallback = func;
    // }

private:

    PubSubClient _mqtt;
    WiFiClient _mclient = WiFiClient();
    bool _configured = false;
    uint32_t mqtt_tmr = 0;
    uint8_t _qos = 0;
    bool _ret = 0;
    String _host;
    String _login;
    String _pass;
    //std::function<void (GHTXT, GHTXT)> _parseCallback;

    char _id[9] = {'\0'};
    String _prefix;

    void begin() {
        _mqtt.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
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
        _mqtt.disconnect();
    }

    void _connect() {
        String m_id("hub_");
        m_id += String(random(0xffffff), HEX);
        bool ok = 0;

        String status(_prefix);
        status += F("/hub/");
        status += _id;
        status += F("/adapterStatus");

        String offline(F("offline"));

        if (_login.length()) {
            ok = _mqtt.connect(m_id.c_str(), _login.c_str(), _pass.c_str(), status.c_str(), _qos, _ret, offline.c_str());
        } else {
            ok = _mqtt.connect(m_id.c_str(), status.c_str(), _qos, _ret, offline.c_str());
        } 
        
        if (ok) {
            _mqtt.subscribe(MqttSensorUp, _qos);
        }
    }

    void parse(GHTXT url, GHTXT data = GHTXT()) {
        // if (!_parseCallback || !url.valid()) { return; }
        if (!data.valid()) {
            char* eq = (char*)url.find('=');
            if (eq) {
                *eq = 0;
                data = GHTXT(eq + 1, url.end() - eq - 1);
                url = GHTXT(url.str(), eq - url.str());
            }
        }
        //_parseCallback(url, data);
    }

    void _config(const String& login, const String& pass, uint8_t nqos, bool nret) {
        _qos = nqos;
        _ret = nret;
        _login = login;
        _pass = pass;
        _configured = true;
    }
    
};
