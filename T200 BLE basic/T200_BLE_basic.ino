/**
 *  Recieve the BLE signal from an iOS device, running GoBLE
 * 
 *  GoBLE - Bluetooth 4.0 Controller
 *  https://itunes.apple.com/us/app/goble-bluetooth-4-0-controller/id950937437
 * 
 *  App: https://github.com/CainZ/GoBle
 *  
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID    "0000dfb0-0000-1000-8000-00805f9b34fb" // service UUID for GoBLE App
#define SERIALPORT_UUID "0000dfb1-0000-1000-8000-00805f9b34fb" // is SerialPortID

#define ledPin 2   // indicate Bluetooth connection
#define motorA1 16 // RX2 unused
#define motorA2 17 // TX2 unused
#define motorB1 18
#define motorB2 19

int dark = 300;
boolean MotorMatrix[5][4] = {{LOW,  LOW, LOW,  LOW},
                             {HIGH, LOW, HIGH, LOW},
                             {HIGH, LOW, LOW, HIGH},
                             {LOW, HIGH, LOW, HIGH},
                             {LOW, HIGH, HIGH, LOW}};

BLEServer *pServer = NULL;
BLECharacteristic * SerialCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

int joystickX, joystickY;
int buttonState[7];        // skip zero and confusion, button 3 = buttonState[3]

void MotorDrive(int button) {
  digitalWrite(motorA1, MotorMatrix[button][0]); 
  digitalWrite(motorA2, MotorMatrix[button][1]); 
  digitalWrite(motorB1, MotorMatrix[button][2]); 
  digitalWrite(motorB2, MotorMatrix[button][3]); 
  Serial.print("Pin 16 - 19: ");
  Serial.print(MotorMatrix[button][0]);
  Serial.print("  ");
  Serial.print(MotorMatrix[button][1]);
  Serial.print("  ");
  Serial.print(MotorMatrix[button][2]);
  Serial.print("  ");
  Serial.print(MotorMatrix[button][3]);
  Serial.println("  ");
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {

        // store values in buttonState[]
        for (int i = 0; i < 7; i++) buttonState[i] = 0;
        for (int i = 0; i < rxValue[3]; i++) buttonState[rxValue[i + 5]] = 1;
        // store values in joystickX and joystickY from 0 to 255
        int i = rxValue[3];
        joystickX = rxValue[i + 5];
        joystickY = rxValue[i + 6];
        i = 0;
        if (buttonState[1] > 0) i = 1; // forward
        if (buttonState[2] > 0) i = 2; // right
        if (buttonState[3] > 0) i = 3; // backward
        if (buttonState[4] > 0) i = 4; // left
        MotorDrive(i);

 /**
        if (buttonState[3] > 0) { // backward
          digitalWrite(motorA1, HIGH); 
          digitalWrite(motorA2, LOW); 
          digitalWrite(motorB1, HIGH); 
          digitalWrite(motorB2, LOW); 
        } else {
          digitalWrite(motorA1, LOW);
          digitalWrite(motorB1, LOW);
          digitalWrite(motorA2, LOW);
          digitalWrite(motorB2, LOW);
        }
        if (buttonState[4] > 0) { // left
          digitalWrite(motorA1, HIGH);
          digitalWrite(motorB2, HIGH);
        } else {
          digitalWrite(motorA1, LOW);
          digitalWrite(motorB2, LOW);
        }
        if (buttonState[5] > 0) { // select
          digitalWrite(ledPin, HIGH);
        } else {
          digitalWrite(ledPin, LOW);
        }   **/     
      }
   }
};

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  // Create the BLE Device  
  BLEDevice::init("T200 ESP32 BLE simple");
  // Create the BLE Server - to advertise the service, that the iPhone App can connect to
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic * SerialCharacteristic = pService->createCharacteristic(
                    SERIALPORT_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_WRITE_NR  
                    );
  SerialCharacteristic->setCallbacks(new MyCallbacks());
  
  Serial.begin(115200);
  // Start the service
  pService->start();
  Serial.println("Service started, App should connect via BLE."); 

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  BLEDevice::startAdvertising();
}

void loop() {
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        dark = 300;
        oldDeviceConnected = deviceConnected;
    }
    digitalWrite(ledPin, HIGH);
    delay(50);
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        dark = 3000;
    }
    digitalWrite(ledPin, LOW);
    delay(dark);
}
