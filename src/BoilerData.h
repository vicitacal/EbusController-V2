#pragma once
#include <Arduino.h>
#include <BoilerErrors.h>
#include <GyverHub.h>

#define SECONDS_IN_MINUTE 60;
#define SECONDS_IN_HOUR 3600;
#define SECONDS_IN_DAY 86400;

struct BoilerData {

    String adapterType = "Unknown";
    String uptimeString = "00:00:00";
    String errorDescription = "No error";
    String connectionErrorDescription = "No error";
    bool connectionStatus;
    float actualDHWTemp;
    float actualHeatingTemp;
    float actualPressure;
    bool burnerStatus;
    bool heatingStatus;
    bool dhwStatus;

    void setAdapterType(uint8_t value) {
        switch (value) {
            case 0: adapterType = "Opentherm";
            case 1: adapterType = "Ebus";
            case 2: adapterType = "Navien";
            default: adapterType = "Unknown";
        }
    }

    void setErrorCode(uint16_t value) {
        errorDescription = BoilerErrors::GetErrorDescription((errorCode)value);
    }

    void setConnectionErrorCode(uint8_t value) {
        connectionErrorDescription = BoilerErrors::GetConnectionErrorDescription((errorCode)value);
    }

    void setUptime(uint32_t seconds) {
        static char buffer[32];
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
        uptimeString = String(buffer);
    }

};
