#pragma once
#include <cstdint>

enum errorCode : uint16_t {
    NoError = 0,
    NTC_ReturnLineOpen = 1,
    NTC_HWSTempOpen = 2,
    NTC_BoilerTempOpen = 3,
    NTC_CollectorTempOpen = 4,
    ExhaustGasSensorOpen = 5,
    DraftSensorOpen = 6,
    NTC_SolarCollectorFault = 7,
    NTC_BoilerGroundOpen = 8,
    NTC_ExhaustTempOpen = 9,
    NTC_SupplyLineShortCircuit = 10, 
    NTC_ReturnLineShortCircuit = 11, 
    NTC_HWSTempShortCircuit = 12, 
    NTC_BoilerTempShortCircuit = 13, 
    NTC_CollectorTempShortCircuit = 14, 
    ExhaustGasSensorShortCircuit = 15, 
    DraftSensorShortCircuit = 16, 
    NTC_SolarCollectorShortCircuit = 17, 
    NTC_BoilerGroundShortCircuit = 18, 
    NTC_ExhaustTempShortCircuit = 19, 
    TemperatureSafetyLimitTriggered = 20, 
    BoilerLockedDueToOverTemperature = 21, 
    LowWaterLevelPumpBlocked = 22, 
    LowWaterLevelTemperatureDifference = 23, 
    LowWaterLevelRapidTemperatureRise = 24, 
    CombustionGasLeakageProtection = 25, 
    GasValveStepperMotorCurrentFault = 26, 
    IonizationFlameDetectionFault = 27, 
    IgnitionFailure = 28, 
    FlameLossDuringOperation = 29, 
    BlockingSensorOpenCircuit = 30, 
    BlockingSensorShortCircuit = 31, 
    FanProtectionRPM = 32, 
    PressureSensorOpen = 33, 
    PressureSensorClosed = 34, 
    FlueGasRemovalSystemError = 35, 
    DraftDiagnosticsError = 36, 
    FanRPMOutOfRange = 37, 
    FrequencyMeasurementOutOfRange = 38, 
    BoilerSelfDiagnosticsError = 39, 
    GasRegulationFault = 41, 
    EncodingResistorIssue = 42, 
    BoilerModelIdentificationError = 43, 
    EBusLowVoltage = 49, 
    CO_SensorError = 55, 
    PreheatingFeedbackMissing = 58, 
    GasValveControlPlusError = 60, 
    GasValveControlMinusError = 61, 
    GasValveErrorDisabled = 62, 
    EEPROM_Error = 63, 
    AOC_Error = 64, 
    ElectronicsOverheating = 65, 
    IIC_DataBusError = 66, 
    FlameSignalUnreliable = 67, 
    IncorrectOSN = 70, 
    WaterFlowSensorOpen = 71, 
    FlowSensorTemperatureMismatch = 72, 
    PressureSensorOpenOrShortCircuit = 73, 
    PressureSensorElectricalFault = 74, 
    PressureSensorRepeatedError = 75, 
    ThermalFuseFault = 76, 
    SHE_SensorInputError = 80, 
    CirculationPumpError = 81, 
    AnodeTesterFault = 82, 
    APC_ModuleNotConnected = 90, 
    APC_ModuleSensorFault = 91, 
};

class BoilerErrors {

public:

    static const char* GetConnectionErrorDescription(uint8_t error) {
        switch (error) {
            case ModbusMaster::ku8MBSuccess: return "Success";
            case ModbusMaster::ku8MBIllegalFunction: return "Modbus protocol illegal function exception";
            case ModbusMaster::ku8MBIllegalDataAddress: return "Modbus protocol illegal data address exception";
            case ModbusMaster::ku8MBIllegalDataValue: return "Modbus protocol illegal data value exception";
            case ModbusMaster::ku8MBSlaveDeviceFailure: return "Modbus protocol slave device failure exception";
            case ModbusMaster::ku8MBInvalidSlaveID: return "ModbusMaster invalid response slave ID exception";
            case ModbusMaster::ku8MBInvalidFunction: return "ModbusMaster invalid response function exception";
            case ModbusMaster::ku8MBResponseTimedOut: return "ModbusMaster response timed out exception";
            case ModbusMaster::ku8MBInvalidCRC: return "ModbusMaster invalid response CRC exception";
            default: return "Unknown error";
        }
    }

    static const char* GetErrorDescription(errorCode code) {
        switch (code) {
            case errorCode::NoError: return "No error";
            case errorCode::NTC_ReturnLineOpen: return "NTC return line temperature sensor open circuit";
            case errorCode::NTC_HWSTempOpen: return "NTC hot water supply temperature sensor open circuit";
            case errorCode::NTC_BoilerTempOpen: return "NTC boiler temperature sensor open circuit";
            case errorCode::NTC_CollectorTempOpen: return "NTC collector temperature sensor open circuit";
            case errorCode::ExhaustGasSensorOpen: return "Exhaust gas sensor open circuit";
            case errorCode::DraftSensorOpen: return "Draft sensor open circuit";
            case errorCode::NTC_SolarCollectorFault: return "NTC return line solar collector sensor fault";
            case errorCode::NTC_BoilerGroundOpen: return "NTC boiler ground sensor open circuit";
            case errorCode::NTC_ExhaustTempOpen: return "NTC exhaust temperature sensor open circuit";
            case errorCode::NTC_SupplyLineShortCircuit: return "NTC supply line temperature sensor short circuit";
            case errorCode::NTC_ReturnLineShortCircuit: return "NTC return line temperature sensor short circuit";
            case errorCode::NTC_HWSTempShortCircuit: return "NTC hot water supply temperature sensor short circuit";
            case errorCode::NTC_BoilerTempShortCircuit: return "NTC boiler temperature sensor short circuit";
            case errorCode::NTC_CollectorTempShortCircuit: return "NTC collector temperature sensor short circuit";
            case errorCode::ExhaustGasSensorShortCircuit: return "Exhaust gas sensor short circuit";
            case errorCode::DraftSensorShortCircuit: return "Draft sensor short circuit";
            case errorCode::NTC_SolarCollectorShortCircuit: return "NTC return line solar collector sensor short circuit";
            case errorCode::NTC_BoilerGroundShortCircuit: return "NTC boiler ground sensor short circuit";
            case errorCode::NTC_ExhaustTempShortCircuit: return "NTC exhaust temperature sensor short circuit";
            case errorCode::TemperatureSafetyLimitTriggered: return "Temperature safety limit triggered";
            case errorCode::BoilerLockedDueToOverTemperature: return "Boiler locked due to over temperature";
            case errorCode::LowWaterLevelPumpBlocked: return "Low water level: pump blocked or faulty";
            case errorCode::LowWaterLevelTemperatureDifference: return "Low water level: large temperature difference between supply and return lines";
            case errorCode::LowWaterLevelRapidTemperatureRise: return "Low water level: rapid temperature rise";
            case errorCode::CombustionGasLeakageProtection: return "Combustion gas leakage protection triggered";
            case errorCode::GasValveStepperMotorCurrentFault: return "Gas valve stepper motor current fault";
            case errorCode::IonizationFlameDetectionFault: return "Ionization flame detection fault";
            case errorCode::IgnitionFailure: return "Ignition failure";
            case errorCode::FlameLossDuringOperation: return "Flame loss during operation";
            case errorCode::BlockingSensorOpenCircuit: return "Blocking sensor open circuit";
            case errorCode::BlockingSensorShortCircuit: return "Blocking sensor short circuit";
            case errorCode::FanProtectionRPM: return "Fan anti-freeze protection: RPM out of range";
            case errorCode::PressureSensorOpen: return "Fan anti-freeze protection: pressure sensor open";
            case errorCode::PressureSensorClosed: return "Fan anti-freeze protection: pressure sensor closed";
            case errorCode::FlueGasRemovalSystemError: return "Flue gas removal system error";
            case errorCode::DraftDiagnosticsError: return "Draft diagnostics error";
            case errorCode::FanRPMOutOfRange: return "Fan RPM out of range";
            case errorCode::FrequencyMeasurementOutOfRange: return "Frequency measurement out of range";
            case errorCode::BoilerSelfDiagnosticsError: return "Boiler self-diagnostics error";
            case errorCode::GasRegulationFault: return "Gas regulation fault";
            case errorCode::EncodingResistorIssue: return "Encoding resistor issue";
            case errorCode::BoilerModelIdentificationError: return "Boiler model identification error";
            case errorCode::EBusLowVoltage: return "eBus low voltage";
            case errorCode::CO_SensorError: return "CO sensor error";
            case errorCode::PreheatingFeedbackMissing: return "Preheating feedback missing";
            case errorCode::GasValveControlPlusError: return "Gas valve control (+) error";
            case errorCode::GasValveControlMinusError: return "Gas valve control (-) error";
            case errorCode::GasValveErrorDisabled: return "Gas valve error disabled";
            case errorCode::EEPROM_Error: return "EEPROM error";
            case errorCode::AOC_Error: return "AOC error";
            case errorCode::ElectronicsOverheating: return "Electronics overheating";
            case errorCode::IIC_DataBusError: return "IIC data bus error";
            case errorCode::FlameSignalUnreliable: return "Flame signal unreliable";
            case errorCode::IncorrectOSN: return "Incorrect OSN";
            case errorCode::WaterFlowSensorOpen: return "Water flow sensor open circuit";
            case errorCode::FlowSensorTemperatureMismatch: return "Flow sensor temperature mismatch";
            case errorCode::PressureSensorOpenOrShortCircuit: return "Pressure sensor open or short circuit";
            case errorCode::PressureSensorElectricalFault: return "Pressure sensor electrical fault";
            case errorCode::PressureSensorRepeatedError: return "Pressure sensor repeated error";
            case errorCode::ThermalFuseFault: return "Primary heat exchanger thermal fuse fault";
            case errorCode::SHE_SensorInputError: return "SHE sensor input error";
            case errorCode::CirculationPumpError: return "Circulation pump error";
            case errorCode::AnodeTesterFault: return "Anode tester fault";
            case errorCode::APC_ModuleNotConnected: return "APC module not connected";
            case errorCode::APC_ModuleSensorFault: return "APC module sensor fault";
            default: return "Unknown error";
        }
    }

private:

};