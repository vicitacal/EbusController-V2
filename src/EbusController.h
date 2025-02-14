#pragma once
#include "Arduino.h"
#include "BoilerData.h"
#include "ModbusMaster.h"
#include "GyverHub.h"
#include "Settings.h"
#include "Utils.h"

#define RawDataSize sizeof(BoilerData)/2

enum adapterCommand : uint8_t {

    noCommand = 0,
    adapterReboot = 2,
    resetErrors = 3

};

enum commandResponse : int8_t {
    
    error = -5,
    boilerNotSupport = -4,
    notSupportId = -3,
    adapterNotSupport = -2,
    timeOut = -1,
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

enum boilerRegister : uint8_t {

    heatingSetpointAdr         = 0x0031,
    fsHeatingSetpointAdr       = 0x0032,
    dhwSetpointAdr             = 0x0037,
    maxBurnerModulationAdr     = 0x0038,
    modesAdr                   = 0x0039,
    commandAdr                 = 0x0080

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
    uint8_t writeValue(boilerRegister option, uint16_t value);
    void setSetting(su::AnyText option, uint16_t value);
    void setSetting(boilerSetting setting, uint16_t value);
    void printState();
    String getRawDataString();
    String getRawSetupsString();

    BoilerData* State;
    bool IsSynchronized;

private:

    ModbusMaster* _master;
    uint16_t _rawData[RawDataSize];
    boilerSettings _currentSettings = boilerSettings();
    
};
