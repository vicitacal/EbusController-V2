#include <EbusController.h>

BoilerEbusController::BoilerEbusController(ModbusMaster &master) {
    
    _master = &master;
    State = reinterpret_cast<BoilerData*>(_rawData);

}

uint8_t BoilerEbusController::updateState() {

    auto returnCode = _master->readHoldingRegisters(0x10, 0x14);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    if (_master->available() < sizeof(BoilerData) / 2){
        return ModbusMaster::ku8MBIllegalDataValue;
    }
    for (uint8_t i = 0; i < sizeof(BoilerData) / 2; i++) {
        _rawData[i + (i == 2) - (i == 3)] = _master->receive();    
    }
    return returnCode;

}

uint8_t BoilerEbusController::writeValue(boilerRegister option, uint16_t value) {

    _master->clearTransmitBuffer();
    _master->setTransmitBuffer(0, value);
    return _master->writeMultipleRegisters(option, 1);

}

void BoilerEbusController::setSetting(su::AnyText option, uint16_t value) {

    setSetting((boilerSetting)option.toInt16(), value);
    
}

void BoilerEbusController::setSetting(boilerSetting setting, uint16_t value) {
    switch (setting) {
        case heatingSetpoint:
            _currentSettings.heatingSetpoint = value;
            break;
        case fsHeatingSetpoint:
            _currentSettings.heatingSetpointFS = value;
            break;
        case dhwSetpoint:
            _currentSettings.dhwSetpoint = value;
            break;
        case heatingEn:
            _currentSettings.modeFlags.write(0, value);
            break;
        case dhwEn:
            _currentSettings.modeFlags.write(1, value);
            break;
        default:
            break;
    }
    IsSynchronized = false;
}

void BoilerEbusController::attachPropertyChanged(void (*handler)()) {
    _handler = handler;
}

uint8_t BoilerEbusController::syncronizeSettings() {
    if (IsSynchronized) { return ModbusMaster::ku8MBSuccess; }
    _master->clearTransmitBuffer();
    _master->setTransmitBuffer(0, _currentSettings.heatingSetpoint * 10);
    _master->setTransmitBuffer(1, _currentSettings.heatingSetpointFS * 10);
    auto returnCode = _master->writeMultipleRegisters(boilerRegister::heatingSetpointAdr, 2);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    _master->clearTransmitBuffer();
    _master->setTransmitBuffer(0, _currentSettings.dhwSetpoint);
    returnCode = _master->writeMultipleRegisters(boilerRegister::dhwSetpointAdr, 1);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    _master->clearTransmitBuffer();
    _master->setTransmitBuffer(0, _currentSettings.modeFlags.flags);
    returnCode = _master->writeMultipleRegisters(boilerRegister::modesAdr, 1);
    IsSynchronized = returnCode == ModbusMaster::ku8MBSuccess;
    if (IsSynchronized && _handler) {
        _handler();
    }
    return returnCode;
}

void BoilerEbusController::printState() {
    Serial.println(State->ToString());
}

String BoilerEbusController::getRawDataString() {
    String result;
    int dataCount = 0;
    for (uint8_t i = 0; i < RawDataSize; i++) {
        result += to_hex(_rawData[i]);
        dataCount++;
        if (dataCount >= 4) {
            dataCount = 0;
            result += '\n';
        }
    }
    return result;
}

String BoilerEbusController::getRawSetupsString() {
    return _currentSettings.ToString();
}
