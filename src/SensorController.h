#include <SensorsState.h>
#include <Arduino.h>
#include <BoilerMqtt.h>
#include <ArduinoJson.h>
#include <Settings.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class SensorController {

private:

    void parseMessage(GHTXT url, GHTXT data) {
        JsonDocument doc;
        auto error = deserializeJson(doc, (String)data);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        if (doc["type"] != "12") { return; }
        JsonObject sensorData_0 = doc["sensorData"][0];
        State.SetUptime((int)sensorData_0["timestamp"]["value"]);
        State.SetBattery((int)sensorData_0["battery"]["value"]);
        State.SetInsideTemperatureQuinping((float)sensorData_0["temperature"]["value"]);
        State.SetHumidity((int)sensorData_0["humidity"]["value"]);
        State.SetCarbonDioxide((int)sensorData_0["co2"]["value"]);
        State.SetPm25((int)sensorData_0["pm25"]["value"]);
        State.SetPm10((int)sensorData_0["pm10"]["value"]);
    }

    void readDS1822Sensor() {
        auto responce = _sensors.requestTemperatures();
        if (!responce) { 
            Serial.println("DS1822 sensor read error");
            return; 
        }
        static uint8_t adr1[] = {0x28, 0xFF, 0x7C, 0x76, 0x1, 0x17, 0x4, 0xED}; 
        static uint8_t adr2[] = {0x28, 0xFF, 0x57, 0xA, 0x1, 0x17, 0x5, 0x81};
        State.SetOutsideTemperature(_sensors.getTempC(adr1, 10));
        State.SetInsideTemperatureDs(_sensors.getTempC(adr2, 10));
    }

    void sendData() {
        JsonDocument doc;
        doc["uptime"] = State.Uptime;
        doc["insideTemperatureQuinping"] = State.InsideTemperatureQuinping;
        doc["insideTemperatureDs"] = State.InsideTemperatureDs;
        doc["outsideTemperature"] = State.OutsideTemperature;
        doc["humidity"] = State.Humidity;
        doc["battery"] = State.Battery;
        doc["pm25"] = State.Pm25;
        doc["pm10"] = State.Pm10;
        doc["carbonDioxide"] = State.CarbonDioxide;
        String jsonData;
        serializeJson(doc, jsonData);
        gh::BridgeData data = gh::BridgeData(jsonData);
        data.topic = "SensorData";
        _mqtt.send(data);
    }

    BoilerMqtt& _mqtt;
    OneWire _oneWire;
    DallasTemperature _sensors;
    gh::Timer _updateTimer = gh::Timer(OneWireRefreshTime);
    std::function<void ()> _onStateUpdate;

public:

    SensorController(BoilerMqtt& mqtt) : _mqtt(mqtt), _oneWire(OneWireBusPin), _sensors(&_oneWire) {
        _sensors.begin();
    }

    void Begin() {
        _mqtt.onMessage([this](GHTXT url, GHTXT data) { parseMessage(url, data); });
        Serial.println("Finding temperature sensos...");
        uint8_t deviceAddress[8];
        for (uint8_t i = 0; i < _sensors.getDeviceCount(); i++) {
            if (_sensors.getAddress(deviceAddress, i)) {
                Serial.print("Found sensor: ");
                for (size_t i = 0; i < 8; i++) {
                    Serial.print("0x");
                    Serial.print(deviceAddress[i], HEX);
                    Serial.print(" ");
                }
                Serial.println("");
                if (!_sensors.setResolution(deviceAddress, 12)){
                    Serial.println("Sensor resolution set error");
                }
            }
        }
    }

    void AttachStateUpdate(std::function<void ()> func){
        _onStateUpdate = func;
    }

    void Tick() {
        _mqtt.tick();
        if (_updateTimer) {
            readDS1822Sensor();
        }
        if (State.IsStateChanged() && _onStateUpdate){
            _onStateUpdate();
        }
    }

    SensorsState State;

};
