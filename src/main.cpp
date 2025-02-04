#include <Settings.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <GyverHub.h>
#include <ViewHandler.h>

SoftwareSerial adapterSerial = SoftwareSerial(RX_ADAPTER, TX_ADAPTER);
ModbusMaster node = ModbusMaster();
BoilerEbusController controller = BoilerEbusController(node);
GyverHub hub("MyDevices", "ESP8266", "");
ViewHandler view(hub, controller);

void preTransmission() {
  digitalWrite(RE_DE_PIN, HIGH);
}
void postTransmission() {
  digitalWrite(RE_DE_PIN, LOW);
}

void setup() {
  adapterSerial.begin(19200, SWSERIAL_8N1);
  adapterSerial.listen();
  Serial.begin(115200);
  pinMode(RE_DE_PIN, OUTPUT);
  digitalWrite(RE_DE_PIN, LOW);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node.begin(AdapterAdress, adapterSerial);

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  view.Setup();
}

void loop() {

  view.Tick();

  // if (Serial.available()) {
  //   auto value = Serial.parseInt();
  //   auto returnCode = controller.setValue(boilerOption::heatingSetpoint, value);
  //   if (returnCode != ModbusMaster::ku8MBSuccess) {
  //     Serial.print("Error: ");
  //     Serial.println(returnCode);
  //   } else {
  //     Serial.println("Success");
  //   }
  // }

  //prinntPenis();
  // delay(1000);
  // node.setTransmitBuffer(0, 410);
  //auto returnCode = controller.setValue(boilerOption::heatingSetpoint, 420);
  // auto returnCode = node.readHoldingRegisters(0x40, 0x2F);
  // if (returnCode != ModbusMaster::ku8MBSuccess) {
  //   Serial.print("Error: ");
  //   Serial.println(returnCode);
  // } else {
  //   Serial.println("Success");
  //   for (size_t i = 0x40; i < 0x6F; i++) {
  //     Serial.print(i, HEX);
  //     Serial.print(": ");
  //     Serial.println(node.receive());
  //   }
    
  //   delay(10000);
  // }
  // delay(1000);
  // while (node.available()) {
  //   Serial.println(node.receive());
  // }
  // auto available = adapterSerial.available();
  // if (available > 0){
  //   char boofer[available];
  //   adapterSerial.readBytes(boofer, available);
  //   for (byte i = 0; i < available; i++) {
  //     Serial.println(boofer[i], 16);
  //   }
  // }

}
