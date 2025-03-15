#include <SensorsState.h>
#include <Arduino.h>
#include <BoilerMqtt.h>
#include <ArduinoJson.h>
#include <Settings.h>

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
        State.Battery = (int)sensorData_0["battery"]["value"];
        State.InsideTemperatureQuinping = (float)sensorData_0["temperature"]["value"];
        State.Humidity = (int)sensorData_0["humidity"]["value"];
        State.CarbonDioxide = (int)sensorData_0["co2"]["value"];
        State.Pm25 = (int)sensorData_0["pm25"]["value"];
        State.Pm10 = (int)sensorData_0["pm10"]["value"];
        if (_onStateUpdate){
            _onStateUpdate();
        }
    }

    BoilerMqtt& _mqtt;
    std::function<void ()> _onStateUpdate;

public:

    SensorController(BoilerMqtt& mqtt) : _mqtt(mqtt) {

    }

    void Begin() {
        _mqtt.onMessage([this](GHTXT url, GHTXT data){ parseMessage(url, data); });
    }

    void AttachStateUpdate(std::function<void ()> func){
        _onStateUpdate = func;
    }

    void Tick() {
        _mqtt.tick();
    }

    SensorsState State;

};
