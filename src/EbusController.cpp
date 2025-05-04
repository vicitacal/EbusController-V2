#include <EbusController.h>

BoilerEbusController::BoilerEbusController(ModbusMaster &master) : _master(master) {
    
    RawState = reinterpret_cast<BoilerRawData*>(_rawData);

}

uint8_t BoilerEbusController::updateState() {

    auto returnCode = _master.readHoldingRegisters(0x10, 0x14);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    if (_master.available() < sizeof(BoilerRawData) / 2) {
        return ModbusMaster::ku8MBIllegalDataValue;
    }
    for (uint8_t i = 0; i < sizeof(BoilerRawData) / 2; i++) {
        _rawData[i + (i == 2) - (i == 3)] = _master.receive();    
    }
    State.actualDHWTemp = RawState->actualDHWTemp * 0.1f;
    State.actualHeatingTemp = RawState->actualHeatingTemp * 0.1f;
    State.actualPressure = RawState->actualPressure * 0.1f;
    State.burnerStatus = RawState->burnerStatus;
    State.connectionStatus = RawState->connectionStatus;
    State.dhwStatus = RawState->dhwStatus;
    State.heatingStatus = RawState->heatingStatus;
    State.setUptime(RawState->uptime);
    State.setAdapterType(RawState->adapterType);
    State.setErrorCode(RawState->mainErrorCode);
    State.setConnectionErrorCode(returnCode);
    return returnCode;

}

uint8_t BoilerEbusController::writeValue(adapterRegister option, uint16_t value) {

    _master.clearTransmitBuffer();
    _master.setTransmitBuffer(0, value);
    return _master.writeMultipleRegisters(option, 1);

}

void BoilerEbusController::invokeCommand(adapterCommand command) {
    if (_commandInProgress) { return; }
    auto returnCode = writeValue(adapterRegister::commandAdr, command);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        finishCommand(commandResponse::communicationError);
        return;
    }
    _commandInProgress = true;
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
    if (_onSettingsSync) {
        _onSettingsSync();
    }
}

void BoilerEbusController::finishCommand(commandResponse data) {
    _commandInProgress = false;
    if (_onCommandDone) {
        _onCommandDone(data);
    }
}

void BoilerEbusController::attachPropertySync(std::function<void ()> func) {
    _onSettingsSync = func;
}

void BoilerEbusController::attachOnCommandDone(std::function<void (commandResponse)> func) {
    _onCommandDone = func;
}

uint8_t BoilerEbusController::syncronizeSettings() {
    if (IsSynchronized) { return ModbusMaster::ku8MBSuccess; }
    _master.clearTransmitBuffer();
    _master.setTransmitBuffer(0, _currentSettings.heatingSetpoint * 10);
    _master.setTransmitBuffer(1, _currentSettings.heatingSetpointFS * 10);
    auto returnCode = _master.writeMultipleRegisters(adapterRegister::heatingSetpointAdr, 2);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    _master.clearTransmitBuffer();
    _master.setTransmitBuffer(0, _currentSettings.dhwSetpoint);
    returnCode = _master.writeMultipleRegisters(adapterRegister::dhwSetpointAdr, 1);
    if (returnCode != ModbusMaster::ku8MBSuccess) {
        return returnCode;
    }
    _master.clearTransmitBuffer();
    _master.setTransmitBuffer(0, _currentSettings.modeFlags.flags);
    returnCode = _master.writeMultipleRegisters(adapterRegister::modesAdr, 1);
    IsSynchronized = returnCode == ModbusMaster::ku8MBSuccess;
    if (_onSettingsSync) {
        _onSettingsSync();
    }
    return returnCode;
}

void BoilerEbusController::printState() {
    Serial.println(RawState->ToString());
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

String BoilerEbusController::ResponceToString(commandResponse data) {
    switch (data) {
        case commandResponse::communicationError: return "Communication error";
        case commandResponse::error: return "Error";
        case commandResponse::boilerNotSupport: return "Boiler not support";
        case commandResponse::notSupportId: return "Not support id";
        case commandResponse::adapterNotSupport: return "Adapter not support";
        case commandResponse::timeout: return "Timeout";
        case commandResponse::success: return "Success";
        case commandResponse::iddle: return "Iddle";
        case commandResponse::processing: return "Processing";
        default: return "Unknown";
    }
}

void BoilerEbusController::tick() {
    if (_commandInProgress) {
        auto returnCode = _master.readHoldingRegisters(adapterRegister::commandResponce, 0x1);
        if (returnCode == ModbusMaster::ku8MBSuccess) {
            auto responce = (commandResponse)_master.receive();
            if (responce != commandResponse::processing) {
                finishCommand(responce);
            }
        } else {
            finishCommand(commandResponse::communicationError);
        }
    }
}
