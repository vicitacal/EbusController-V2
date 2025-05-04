#pragma once
#include "Arduino.h"
#include "BoilerData.h"
#include "BoilerRawData.h"
#include "ModbusMaster.h"
#include "GyverHub.h"
#include "Settings.h"
#include "Utils.h"

#define RawDataSize sizeof(BoilerRawData)/2

enum adapterCommand : uint8_t {

    noCommand = 0,
    adapterReboot = 2,
    resetErrors = 3

};

enum commandResponse : int16_t {
    
    communicationError = -32768,
    error = -5,
    boilerNotSupport = -4,
    notSupportId = -3,
    adapterNotSupport = -2,
    timeout = -1,
    success = 0,
    iddle = 1,
    processing = 2

};

enum registerState : int8_t {

    writeError = -2,
    notSupported = -1,
    valid = 0,
    uninitialized = 1

};

enum adapterRegister : uint8_t {

    heatingSetpointAdr         = 0x0031,
    fsHeatingSetpointAdr       = 0x0032,
    dhwSetpointAdr             = 0x0037,
    maxBurnerModulationAdr     = 0x0038,
    modesAdr                   = 0x0039,
    commandAdr                 = 0x0080,
    commandResponce            = 0x0081

};

enum boilerSetting : uint8_t {

    heatingSetpoint         = HeatingSpIndex,
    fsHeatingSetpoint       = HeatingSpFsIndex,
    dhwSetpoint             = DhwSpIndex,
    heatingEn               = HeatingEnIndex,
    dhwEn                   = DhwEnIndex
    
};

struct boilerSettings {

    uint16_t heatingSetpoint;
    uint16_t heatingSetpointFS;
    uint8_t dhwSetpoint;
    gh::Flags modeFlags;

    String ToString() {
        String result;
        result += "Heating setpoint: " + String(heatingSetpoint) + "\n";
        result += "Heating setpoint fs: " + String(heatingSetpointFS) + "\n";
        result += "DHW setpoint: " + String(dhwSetpoint) + "\n";
        result += "Heating enable: " + String(modeFlags.get(0)) + "\n";
        result += "DHW enable: " + String(modeFlags.get(1)) + "\n";
        result += "Mode flags: " + to_binary(modeFlags.flags) + "\n";
        return result;
    }

};

class BoilerEbusController {

public:

    BoilerEbusController(ModbusMaster &master);
    uint8_t updateState();
    uint8_t syncronizeSettings();
    uint8_t writeValue(adapterRegister option, uint16_t value);
    void invokeCommand(adapterCommand command);
    void setSetting(su::AnyText option, uint16_t value);
    void setSetting(boilerSetting setting, uint16_t value);
    void attachPropertySync(std::function<void ()> func);
    void attachOnCommandDone(std::function<void (commandResponse)> func);
    void printState();
    void tick();
    String getRawDataString();
    String getRawSetupsString();
    BoilerRawData* RawState;
    BoilerData State;
    static String ResponceToString(commandResponse);

    bool IsSynchronized;
    
private:
    
    void finishCommand(commandResponse);

    std::function<void ()> _onSettingsSync;
    std::function<void (commandResponse)> _onCommandDone;
    ModbusMaster& _master;
    uint16_t _rawData[RawDataSize];
    boilerSettings _currentSettings = boilerSettings();
    bool _commandInProgress;
    
};
