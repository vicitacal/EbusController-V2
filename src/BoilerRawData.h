#pragma once
#include "Arduino.h"

#pragma pack(push, 1)

struct BoilerRawData {

    uint8_t rebootCode;
    uint8_t adapterType : 3;
    uint8_t connectionStatus : 1;
    uint8_t reserved1 : 4;
    uint8_t adapterSwVersion;
    uint8_t adapterHwVersion;
    uint32_t uptime;                //X1 sec
    uint16_t lowerLimitHeating;
    uint16_t upperLimitHeating;
    uint16_t lowerLimitDHW;
    uint16_t upperLimitDHW;
    int16_t actualDHWTemp;          //X0.1 Celsius
    int16_t actualHeatingTemp;      //X0.1 Celsius
    uint16_t actualPressure;        //X0.1 bar
    uint16_t actualDHWFlow;
    uint16_t burnerModulation;
    uint8_t burnerStatus : 1;
    uint8_t heatingStatus : 1;
    uint8_t dhwStatus : 1;
    uint8_t reserved3 : 5;
    uint8_t reserved2;
    uint16_t mainErrorCode;
    uint16_t additionalErrorCode;
    int16_t outdoorTemp;
    uint16_t manufacturerCode;
    uint16_t modelCode;
    uint16_t errorFlags;

    String ToString() {
        String result;
        result += "Boiler Data:\n";
        result += "Reboot Code: " + String(rebootCode) + "\n";
        result += "Adapter Type: " + String(adapterType) + "\n";
        result += "Connection Status: " + String(connectionStatus ? "Connected" : "Disconnected") + "\n";
        result += "Adapter Software Version: " + String(adapterSwVersion) + "\n";
        result += "Adapter Hardware Version: " + String(adapterHwVersion) + "\n";
        result += "Uptime: " + String(uptime) + " seconds\n";
        result += "Lower Limit Heating: " + String(lowerLimitHeating, 1) + " °C\n";
        result += "Upper Limit Heating: " + String(upperLimitHeating, 1) + " °C\n";
        result += "Lower Limit DHW: " + String(lowerLimitDHW, 1) + " °C\n";
        result += "Upper Limit DHW: " + String(upperLimitDHW, 1) + " °C\n";
        result += "Actual Heating Temperature: " + String(actualHeatingTemp / 10.0) + " °C\n";
        result += "Actual DHW Temperature: " + String(actualDHWTemp / 10.0) + " °C\n";
        result += "Actual Pressure: " + String(actualPressure / 10.0) + " bar\n";
        result += "Actual DHW Flow: " + String(actualDHWFlow) + " l/min\n";
        result += "Burner Modulation: " + String(burnerModulation) + " %\n";
        result += "DHW Status: " + String(dhwStatus ? "On" : "Off") + "\n";
        result += "Heating Status: " + String(heatingStatus ? "On" : "Off") + "\n";
        result += "Burner Status: " + String(burnerStatus ? "On" : "Off") + "\n";
        result += "Main Error Code: " + String(mainErrorCode) + "\n";
        result += "Additional Error Code: " + String(additionalErrorCode) + "\n";
        result += "Outdoor Temperature: " + String(outdoorTemp) + " °C\n";
        result += "Manufacturer Code: " + String(manufacturerCode) + "\n";
        result += "Model Code: " + String(modelCode) + "\n";
        result += "Error Flags: " + String(errorFlags, BIN) + "\n";
        result += "-------------------------\n";
        
        return result;
    }

};

#pragma pack(pop)