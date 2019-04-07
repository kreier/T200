/**
 *  Recieve the BLE signal from an iOS device, running GoBLE
 * 
 *  GoBLE - Bluetooth 4.0 Controller
 *  https://itunes.apple.com/us/app/goble-bluetooth-4-0-controller/id950937437
 * 
 *  App: https://github.com/CainZ/GoBle
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

const int freq = 22000;
const int A1Channel = 0;  // PWM channels 0 to 7
const int A2Channel = 1;
const int B1Channel = 2;
const int B2Channel = 3;
const int resolution = 8; // Resolution 8, 10, 12, 15 bit

BLEServer *pServer = NULL;
BLECharacteristic * SerialCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

int joystickX, joystickY;
int buttonState[7];        // skip zero and confusion, button 3 = buttonState[3]

void MotorDrive(int A, int B) { // A left, B right, positive forward, negative backward
  ledcWrite(A1Channel, constrain(-2 * A, 0, 255));
  ledcWrite(A2Channel, constrain( 2 * A, 0, 255));
  ledcWrite(B1Channel, constrain(-2 * B, 0, 255));
  ledcWrite(B2Channel, constrain( 2 * B, 0, 255));
  Serial.print("Pin 16 - 19: ");
  Serial.print(constrain(-2 * A, 0, 255));
  Serial.print("  ");
  Serial.print(constrain( 2 * A, 0, 255));
  Serial.print("  ");
  Serial.print(constrain(-2 * B, 0, 255));
  Serial.print("  ");
  Serial.print(constrain( 2 * B, 0, 255));
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
        MotorDrive((joystickX - 128) - (joystickY - 128), (joystickX - 128) + (joystickY - 128));
      }
   }
};

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  // Configure PWM with LED PWM functionalities
  ledcSetup(A1Channel, freq, resolution);
  ledcSetup(A2Channel, freq, resolution);
  ledcSetup(B1Channel, freq, resolution);
  ledcSetup(B2Channel, freq, resolution);
  
  // attach the channel to the pins
  ledcAttachPin(motorA1, A1Channel);
  ledcAttachPin(motorA2, A2Channel);
  ledcAttachPin(motorB1, B1Channel);
  ledcAttachPin(motorB2, B2Channel);
    
  // Create the BLE Device  
  BLEDevice::init("T200 BLE PWM");
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
        dark = 300; // not connected
        oldDeviceConnected = deviceConnected;
    }
    digitalWrite(ledPin, HIGH);
    delay(50);
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        dark = 3000; // connected
    }
    digitalWrite(ledPin, LOW);
    delay(dark);
}
