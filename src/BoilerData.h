
#include "Arduino.h"
#define SECONDS_IN_MINUTE 60;
#define SECONDS_IN_HOUR 3600;
#define SECONDS_IN_DAY 86400;

#pragma pack(push, 1)

struct BoilerData {
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
    int16_t actualHeatingTemp;      //X0.1 Celsius
    int16_t actualDHWTemp;          //X0.1 Celsius
    uint16_t actualPressure;        //X0.1 bar
    uint16_t actualDHWFlow;
    uint16_t burnerModulation;
    uint8_t dhwStatus : 1;
    uint8_t heatingStatus : 1;
    uint8_t burnerStatus : 1;
    uint8_t reserved3 : 5;
    uint8_t reserved2;
    uint16_t mainErrorCode;
    uint16_t additionalErrorCode;
    int16_t outdoorTemp;
    uint16_t manufacturerCode;
    uint16_t modelCode;
    uint16_t errorFlags;

    String GetBoilerData() {
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

    char* getTimeString() {
        static char buffer[32];
        auto seconds = uptime;
        uint64_t days = seconds / SECONDS_IN_DAY;
        seconds %= SECONDS_IN_DAY;
        uint8_t hours = seconds / SECONDS_IN_HOUR;
        seconds %= SECONDS_IN_HOUR;
        uint8_t minutes = seconds / SECONDS_IN_MINUTE;
        uint8_t remaining_seconds = seconds % SECONDS_IN_MINUTE;
        
        if (days > 0) {
            snprintf(buffer, sizeof(buffer), "%llu day%s, %u:%02u:%02u",
                    (unsigned long long)days, (days > 1) ? "s" : "", hours, minutes, remaining_seconds);
        } else {
            snprintf(buffer, sizeof(buffer), "%u:%02u:%02u", hours, minutes, remaining_seconds);
        }
        
        return buffer;
    }

    const char* getAdapterTypeName() {
        switch (adapterType) {
            case 0: return "Opentherm";
            case 1: return "Ebus";
            case 2: return "Navien";
            default: return "Unknown";
        }
    }

};

#pragma pack(pop)
