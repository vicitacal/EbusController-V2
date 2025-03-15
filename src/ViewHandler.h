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
            info.add("Adapter hw version", _controller.State->adapterHwVersion);
            info.add("Adapter sw version", _controller.State->adapterSwVersion);
            info.add("Adapter type", _controller.State->getAdapterTypeName());
            info.add("Build number", BUILD_NUMBER);
            info.add("Build date", BUILD_DATE);
            break;
            default: break;
        }
    }

    void buildInfo(gh::Builder& b) {
        b.Title("Boiler info");
        bool isDisconnected = _controller.State->connectionStatus == 0;
        b.Icon().icon("f1e6").label("Connection").value(_controller.State->connectionStatus);
        if (isDisconnected) {
            b.Text(BoilerErrors::GetConnectionErrorDescription(_lastUpdateStatus)).label("Connection error");
        }
        b.GaugeLinear().label("Heating temp").icon("f773").unit(" °C").value(_controller.State->actualHeatingTemp / 10.0).range(0, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear().label("DHW temp").icon("e005").unit(" °C").value(_controller.State->actualDHWTemp / 10.0).range(-10, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear().label("Outside temp").icon("f76b").unit(" °C").value(_controller.State->outdoorTemp).range(-50, 50, 1).disabled(isDisconnected);
        b.GaugeLinear().label("Pressure").icon("f3fd").unit(" bar").value(_controller.State->actualPressure / 10.0).range(0, 3, 0.1).disabled(isDisconnected);
        {
            gh::Row r(b);
            b.Icon().icon("e005").label("DHW status").value(_controller.State->dhwStatus).disabled(isDisconnected);
            b.Icon().icon("f773").label("Heating status").value(_controller.State->heatingStatus).disabled(isDisconnected);
            b.Icon().icon("f46a").label("Burner status").value(_controller.State->burnerStatus).disabled(isDisconnected);
        }
        b.Label(_controller.State->getTimeString()).label("Uptime").disabled(isDisconnected);
        b.Label(_controller.State->rebootCode).label("Reboot code").disabled(isDisconnected);
        b.Display().label("Error").value(BoilerErrors::GetErrorDescription((errorCode)_controller.State->mainErrorCode)).disabled(isDisconnected);
    }

    void buildControl(gh::Builder& b) {
        b.Title("Boiler control");
        b.Slider_(STR(HeatingSpIndex), &_setups).label("Heating setpoint").range(38, 75, 0.1).unit(" °C").attach(staticPropertyHandler);
        b.Slider_(STR(HeatingSpFsIndex), &_setups).label("Heating setpoint in FS").range(38, 75, 0.1).unit(" °C").attach(staticPropertyHandler);
        b.Slider_(STR(DhwSpIndex), &_setups).label("DHW setpoint").range(38, 60, 1).unit(" °C").attach(staticPropertyHandler);
        {
            gh::Row r(b);
            b.Switch_(STR(HeatingEnIndex), &_setups).label("Heating enable").attach(staticPropertyHandler);
            b.Switch_(STR(DhwEnIndex), &_setups).label("DHW enable").attach(staticPropertyHandler);
        }
        if (_writeDataStatus != ModbusMaster::ku8MBSuccess) {
            b.Text(BoilerErrors::GetConnectionErrorDescription(_writeDataStatus)).label("Write data error");
        }
        b.LED_("SyncLed").value(_controller.IsSynchronized).label("Settings synchronized");
        if (b.changed()) { 
            b.refresh();
        }
    }

    void buildSensors(gh::Builder& b) {
        b.Title("Sensors");
        b.Label_("0", _sensors.State.UptimeString).label("Uptime");
        {
            gh::Row r(b);
            b.Gauge_("1", &_sensors.State.InsideTemperatureQuinping).label("Temperature in Quin").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("2", &_sensors.State.InsideTemperatureDs).label("Temperature in Ds").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("3", &_sensors.State.OutsideTemperature).label("Temperature out").unit(" °C").range(-10, 50, 0.1);
            b.Gauge_("4", &_sensors.State.Humidity).label("Humidity").unit(" %").range(0, 100, 1);
        }
        {
            gh::Row r(b);
            b.Gauge_("5", &_sensors.State.CarbonDioxide).label("Carbon dioxide").range(0, 4000, 1);
            b.Gauge_("6", &_sensors.State.Pm25).label("Pm25").range(0, 60, 1);
            b.Gauge_("7", &_sensors.State.Pm10).label("Pm10").range(0, 60, 1);
            b.Gauge_("8", &_sensors.State.Battery).label("Battery").unit(" %").range(0, 100, 1);
        }
        if (b.changed()) { 
            b.refresh();
        }
    }

    void build(gh::Builder& b) {
        Serial.println("On build");
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
            _hub.sendCLI(_controller.State->ToString());
        } else if (str == "stateRaw") {
            _hub.sendCLI(_controller.getRawDataString());
        } else if (str == "setups"){
            _hub.sendCLI(_controller.getRawSetupsString());
        } else {
            _hub.sendCLI("Unknown command. Available commands: state, stateRaw, setups");
        }
    }

    void UpdateSensors() {
        _hub.sendUpdate(_updateSensorString);
    }

    static void staticPropertyHandler(gh::Build& b) {
        if (_instance) {
            _instance->_controller.setSetting(b.name, b.value);
            _instance->_hub.update(b.name);
        }
    }

    static String GetUpdateSensorString() {
        String result("0");
        for (size_t i = 1; i < SensorsState::SensorsCount; i++) {
            result += ";";
            result += i;
        }
        return result;
    }

    BoilerEbusController& _controller;
    PairsFile _setups;
    SensorController _sensors;
    String _updateSensorString;
    GyverHub _hub;
    BoilerMqtt _mqtt;
    gh::Timer _updateTimer;
    static ViewHandler* _instance;
    uint8_t _lastUpdateStatus = 0;
    uint8_t _writeDataStatus = 0;
    
public:

    ViewHandler(BoilerEbusController& controller) : _controller(controller), _setups(PairsFile(&GH_FS, "/boilerSetups.dat", 3000)), 
                                                    _sensors(SensorController(_mqtt)), _updateSensorString(GetUpdateSensorString()) {
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
        _sensors.AttachStateUpdate([this]() { this->UpdateSensors(); });
        _sensors.Begin();
        _controller.attachPropertySync([this]() { this->_hub.update("SyncLed"); });
        _updateTimer = gh::Timer(AdapterRefreshTime);
    }

    void Tick() {
        _setups.tick();
        _sensors.Tick();
        _hub.tick();
        if (_updateTimer) {
            _lastUpdateStatus = _controller.updateState();
            _writeDataStatus = _controller.syncronizeSettings();
        }
    }
    
};
