#include <EbusController.h>
#include <Settings.h>
#include <Arduino.h>
#include <GyverHub.h>
#include <BoilerErrors.h>
#include <PairsFile.h>

class ViewHandler {

private:

    void getInfo(gh::Info& info) {
        switch (info.type) {
            case gh::Info::Type::Version:
            info.add("Controller sw version", ProgramVersion);
            info.add("Adapter hw version", _controller->State->adapterHwVersion);
            info.add("Adapter sw version", _controller->State->adapterSwVersion);
            info.add("Adapter type", _controller->State->getAdapterTypeName());
            break;
            default: break;
        }
    }

    void buildInfo(gh::Builder& b) {
        b.Title("Boiler info");
        bool isDisconnected = _controller->State->connectionStatus == 0;
        b.Icon().icon("f1e6").label("Connection").value(_controller->State->connectionStatus);
        if (isDisconnected) {
            b.Text(BoilerErrors::GetConnectionErrorDescription(_lastUpdateStatus)).label("Connection error");
        }
        b.GaugeLinear().label("Heating temp").icon("f773").unit(" °C").value(_controller->State->actualHeatingTemp / 10.0).range(0, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear().label("DHW temp").icon("e005").unit(" °C").value(_controller->State->actualDHWTemp / 10.0).range(-10, 100, 0.1).disabled(isDisconnected);
        b.GaugeLinear().label("Outside temp").icon("f76b").unit(" °C").value(_controller->State->outdoorTemp).range(-50, 50, 1).disabled(isDisconnected);
        b.GaugeLinear().label("Pressure").icon("f3fd").unit(" bar").value(_controller->State->actualPressure / 10.0).range(0, 3, 0.1).disabled(isDisconnected);
        {
            gh::Row r(b);
            b.Icon().icon("e005").label("DHW status").value(_controller->State->dhwStatus).disabled(isDisconnected);
            b.Icon().icon("f773").label("Heating status").value(_controller->State->heatingStatus).disabled(isDisconnected);
            b.Icon().icon("f46a").label("Burner status").value(_controller->State->burnerStatus).disabled(isDisconnected);
        }
        b.Label(_controller->State->getTimeString()).label("Uptime").disabled(isDisconnected);
        b.Label(_controller->State->rebootCode).label("Reboot code").disabled(isDisconnected);
        b.Display().label("Error").value(BoilerErrors::GetErrorDescription((errorCode)_controller->State->mainErrorCode)).disabled(isDisconnected);
    }

    void handlePropertyChange(gh::Build& b) {
        _hub->sendCLI(b.name); 
        _hub->sendCLI(b.value);
    }

    void buildControl(gh::Builder& b) {
        b.Title("Boiler control");
        b.Slider_(HeatingSPName, &_setups).label("Heating setpoint").range(38, 75, 0.1).unit(" °C").attach([this](gh::Build& build) { this->handlePropertyChange(build); });
        b.Slider_(HeatingSPFSName, &_setups).label("Heating setpoint in FS").range(38, 75, 0.1).unit(" °C");
        b.Slider_(DhwSPName, &_setups).label("Heating setpoint").range(38, 60, 1).unit(" °C");
        {
            gh::Row r(b);
            b.Switch_(HeatingENName, &_setups).label("Heating enable");
            b.Switch_(DhwENName, &_setups).label("DHW enable");
        }
        if (_writeDataStatus != ModbusMaster::ku8MBSuccess) {
            b.Text(BoilerErrors::GetConnectionErrorDescription(_writeDataStatus)).label("Write data error");
        }
        if (b.changed()) { 
            b.refresh();
        }
    }

    void build(gh::Builder& b) {
        Serial.println("On update");
        b.Menu(F("Info;Control"));
        switch (_hub->menu) {
        case 0:
            buildInfo(b);
            break;
        case 1:
            buildControl(b);
            break;
        default:
            break;
        }
    }

    void handleCLI(String str) {
        Serial.print("CLI: ");
        Serial.println(str);
        if (str == "state") {
            _hub->sendCLI(_controller->State->GetBoilerData());
        }
    }

    PairsFile _setups;
    GyverHub* _hub;
    BoilerEbusController* _controller;
    gh::Timer _updateTimer;
    uint8_t _lastUpdateStatus = 0;
    uint8_t _writeDataStatus = 0;
    
public:

    ViewHandler(GyverHub& hub, BoilerEbusController& controller) {
        _hub = &hub;
        _controller = &controller;
    }

    void Setup() {
        _setups = PairsFile(&GH_FS, "/boilerSetups.dat", 3000);
        _hub->mqtt.config("test.mosquitto.org", 1883);
        _hub->config(F(Prefix), F("Ebus adapter"), F("f2cc"));
        _hub->onBuild([this](gh::Builder& b) { this->build(b); });
        _hub->onInfo([this](gh::Info& b) { this->getInfo(b); });
        _hub->onCLI([this](String str) { this->handleCLI(str); });
        _hub->onUpload([this](String path) { if (path == "/boilerSetups.dat") { _setups.begin(); }});
        _hub->begin();
        _setups.begin();
        _hub->setBufferSize(2000);
        _updateTimer = gh::Timer(AdapterRefreshTime);
    }

    void Tick() {
        _setups.tick();
        _hub->tick();
        if (_updateTimer) {
            _lastUpdateStatus = _controller->updateState();
            _writeDataStatus = _controller->syncronizeSettings();
        }
    }
    
};
