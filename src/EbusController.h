#pragma once
#include "Arduino.h"
#include "BoilerData.h"
#include "ModbusMaster.h"
#include "GyverHub.h"

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

    heatingSetpoint         = 1,
    fsHeatingSetpoint       = 2,
    dhwSetpoint             = 3,
    heatingEn               = 4,
    dhwEn                   = 5
    
};

struct boilerSettings {

    uint16_t heatingSetpoint;
    uint16_t heatingSetpointFS;
    uint8_t dhwSetpoint;
    gh::Flags modeFlags;

};

class BoilerEbusController {

public:

    BoilerEbusController(ModbusMaster &master);
    uint8_t updateState();
    uint8_t syncronizeSettings();
    uint8_t setValue(boilerRegister option, uint16_t value);
    void setSetting(boilerSetting setting, uint16_t value);
    void printState();

    BoilerData* State;
    bool IsSynchronized;

private:

    ModbusMaster* _master;
    uint16_t _rawData[sizeof(BoilerData) / 2];
    boilerSettings _currentSettings = boilerSettings();
    
};
