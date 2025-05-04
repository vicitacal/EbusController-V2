#include <EbusController.h>
#include <Settings.h>
#include <Arduino.h>
#include <GyverHub.h>
#include <BoilerErrors.h>
#include <PairsFile.h>
#include <BuildInfo.h>
#include <BoilerMqtt.h>
#include <SensorController.h>

class ViewHandler {

private:

    void getInfo(gh::Info& info) {
        switch (info.type) {
            case gh::Info::Type::Version:
            info.add("Controller sw version", ProgramVersion);
            info.add("Adapter hw version", _controller.RawState->adapterHwVersion);
            info.add("Adapter sw version", _controller.RawState->adapterSwVersion);
            info.add("Adapter type", _controller.State.adapterType);
            info.add("Build number", BUILD_NUMBER);
            info.add("Build date", BUILD_DATE);
            break;
            default: break;
        }
    }

    void buildInfo(gh::Builder& b) {
        b.Title("Boiler info");
        bool isDisconnected = _controller.RawState->connectionStatus == 0;
        b.Icon_("A0", &_controller.State.connectionStatus).icon("f1e6").label("Connection");
        if (isDisconnected) {
            b.Text_("A10", su::Text(_controller.State.connectionErrorDescription)).label("Connection error");
        }
        //b.GaugeLinear().label("Heating temp").icon("f773").unit(" °C").value(_controller.State->actualHeatingTemp / 10.0).range(0, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear_("A1", &_controller.State.actualDHWTemp).label("DHW temp").icon("e005").unit(" °C").range(-10, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear_("A2", &_controller.RawState->outdoorTemp).label("Outside temp").icon("f76b").unit(" °C").range(-50, 50, 1).disabled(isDisconnected);
        b.GaugeLinear_("A3", &_controller.State.actualPressure).label("Pressure").icon("f3fd").unit(" bar").range(0, 3, 0.1).disabled(isDisconnected);
        {
            gh::Row r(b);
            b.Icon_("A4", &_controller.State.dhwStatus).icon("e005").label("DHW status").disabled(isDisconnected);
            b.Icon_("A5", &_controller.State.heatingStatus).icon("f773").label("Heating status").disabled(isDisconnected);
            b.Icon_("A6", &_controller.State.burnerStatus).icon("f46a").label("Burner status").disabled(isDisconnected);
        }
        b.Label_("A7", su::Text(_controller.State.uptimeString)).label("Uptime").disabled(isDisconnected);
        b.Display_("A8", su::Text(_controller.State.errorDescription)).label("Error").disabled(isDisconnected);
        b.Label_("A9", su::Value(_controller.RawState->rebootCode)).label("Reboot code").disabled(isDisconnected);
    }

    void buildControl(gh::Builder& b) {
        b.Title("Boiler control");
        b.Slider_(STR(HeatingSpIndex), &_setups).label("Heating setpoint").range(38, 75, 0.1).unit(" °C").attach(staticPropertyHandler);
        b.Slider_(STR(HeatingSpFsIndex), &_setups).label("Heating setpoint in FS").range(38, 75, 0.1).unit(" °C").attach(staticPropertyHandler);
        b.Slider_(STR(DhwSpIndex), &_setups).label("DHW setpoint").range(38, 60, 1).unit(" °C").attach(staticPropertyHandler);
        if (_writeDataStatus != ModbusMaster::ku8MBSuccess) {
            b.Text(_controller.State.connectionErrorDescription).label("Write data error");
        }
        {
            gh::Row r(b);
            b.Switch_(STR(HeatingEnIndex), &_setups).label("Heating enable").attach(staticPropertyHandler);
            b.Switch_(STR(DhwEnIndex), &_setups).label("DHW enable").attach(staticPropertyHandler);
        }
        b.LED_("SyncLed", &_controller.IsSynchronized).label("Settings synchronized");
        b.Button().label("Reset errors").icon("f2f9").attach([](){if (_instance) { _instance->_controller.invokeCommand(adapterCommand::resetErrors); }});
        b.Button().label("Reboor adapter").icon("f011").attach([](){if (_instance) { _instance->_controller.invokeCommand(adapterCommand::adapterReboot); }});
    }

    void buildSensors(gh::Builder& b) {
        b.Title("Sensors");
        b.Label_("B0", su::Text(_sensors.State.UptimeString)).label("Uptime");
        {
            gh::Row r(b);
            b.Gauge_("B1", &_sensors.State.InsideTemperatureQuinping).label("Temperature in Quin").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("B2", &_sensors.State.InsideTemperatureDs).label("Temperature in Ds").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("B3", &_sensors.State.OutsideTemperature).label("Temperature out").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("B4", &_sensors.State.Humidity).label("Humidity").unit(" %").range(0, 100, 1);
        }
        {
            gh::Row r(b);
            b.Gauge_("B5", &_sensors.State.CarbonDioxide).label("Carbon dioxide").range(0, 4000, 1);
            b.Gauge_("B6", &_sensors.State.Pm25).label("Pm25").range(0, 60, 1);
            b.Gauge_("B7", &_sensors.State.Pm10).label("Pm10").range(0, 60, 1);
            b.Gauge_("B8", &_sensors.State.Battery).label("Battery").unit(" %").range(0, 100, 1);
        }
    }

    void build(gh::Builder& b) {
        b.Menu(F("Info;Control;Sensors"));
        switch (_hub.menu) {
        case 0:
            buildInfo(b);
            break;
        case 1:
            buildControl(b);
            break;
        case 2:
            buildSensors(b);
            break;
        default:
            break;
        }
    }

    void handleCLI(String str) {
        Serial.print("CLI: ");
        Serial.println(str);
        if (str == "state") {
            _hub.sendCLI(_controller.RawState->ToString());
        } else if (str == "stateRaw") {
            _hub.sendCLI(_controller.getRawDataString());
        } else if (str == "setups") {
            _hub.sendCLI(_controller.getRawSetupsString());
        } else if (str == "update") {
            UpdateWidgets();
        } else {
            _hub.sendCLI("Unknown command. Available commands: state, stateRaw, setups, update");
        }
    }

    void UpdateWidgets() {
        Serial.println("Update widgets");
        switch (_hub.menu) {
        case 0:
            if (_controller.RawState->connectionStatus) {
                _hub.sendUpdate(_updateBoilerInfoString);
            }
            break;
        case 1:
            this->_hub.sendUpdate("SyncLed");
            break;
        case 2:
            _hub.sendUpdate(_updateSensorString);
            break;
        default:
            break;
        }
    }

    static void staticPropertyHandler(gh::Build& b) {
        if (_instance) {
            _instance->_controller.setSetting(b.name, b.value);
            _instance->UpdateWidgets();
            //_instance->_hub.sendUpdate(b.name);
        }
    }

    static String GetUpdateString(String prefix, uint8_t count) {
        if (count == 0) { return String(); }
        String result(prefix);
        result += "0";
        for (size_t i = 1; i < count; i++) {
            result += ";";
            result += prefix;
            result += i;
        }
        return result;
    }

    BoilerEbusController& _controller;
    PairsFile _setups;
    SensorController _sensors;
    String _updateSensorString;
    String _updateBoilerInfoString;
    GyverHub _hub;
    BoilerMqtt _mqtt;
    gh::Timer _updateTimer = gh::Timer(AdapterRefreshTime);
    static ViewHandler* _instance;
    uint8_t _writeDataStatus = 0;
    
public:

    ViewHandler(BoilerEbusController& controller) : _controller(controller), _setups(PairsFile(&GH_FS, "/boilerSetups.dat", 3000)), 
                                                    _sensors(SensorController(_mqtt)), _updateSensorString(GetUpdateString("B", SensorsState::SensorsCount)), 
                                                    _updateBoilerInfoString(GetUpdateString("A", BoilerInfoWidgetsCount)) {
        _instance = this;
    }

    void Setup() {
        _mqtt.config(MqttHost, MqttPort, MqttLogin, MqttPass, MqttId, MqttPrefix);
        _hub.mqtt.config(MqttHost, MqttPort, MqttLogin, MqttPass);
        _hub.config(MqttPrefix, HubName, HubIcon, MqttId);
        _hub.onBuild([this](gh::Builder& b) { this->build(b); });
        _hub.onInfo([this](gh::Info& b) { this->getInfo(b); });
        _hub.onCLI([this](String str) { this->handleCLI(str); });
        _hub.onUpload([this](String path) { if (path == "/boilerSetups.dat") { _setups.begin(); }});
        _hub.begin();
        _setups.begin();
        for (uint16_t i = 0; i < _setups.amount(); i++) {
            auto pair = _setups.get(i);
            _controller.setSetting(pair.key, pair);
        }
        _hub.setBufferSize(2000);
        _sensors.AttachStateUpdate([this]() { this->UpdateWidgets(); });
        _sensors.Begin();
        _controller.attachPropertySync([this]() { this->UpdateWidgets(); });
        _controller.attachOnCommandDone([this](commandResponse r) { 
            this->_hub.sendCLI(String("Command is done: ") + BoilerEbusController::ResponceToString(r)); 
        });
    }

    void Tick() {
        _setups.tick();
        _sensors.Tick();
        _hub.tick();
        _controller.tick();
        if (_updateTimer) {
            _controller.updateState();
            _writeDataStatus = _controller.syncronizeSettings();
            if (_hub.menu == 0) { UpdateWidgets(); }
        }
    }
    
};
