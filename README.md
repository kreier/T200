# T200 - ESP32 based robots

This robot car uses the ESP32 to control its actions. The hardware setup is similar, but the upgrades come in software. This is an upgrade from the [T100](https://github.com/kreier/T100) that was powered by an Arduino Uno. Now we have WiFi and Bluetooth BLE build in. And way more storage for our programs.

![Robot](T200.jpg)

## Hardware and materials

All materials were ordered at [CỬA HÀNG IC ĐÂY RỒI](https://icdayroi.com/). This is the list:

1. [Arduino UNO R3 DIP](https://icdayroi.com/arduino-uno-r3-dip) 110.000₫
2. [Khung Xe Robot](https://icdayroi.com/khung-xe-robot) 68.000₫
3. [Shield L293D motor arduino](https://icdayroi.com/shield-l293d-motor-arduino) 34.000₫
4. [Module thu phát bluetooth HC-05](https://icdayroi.com/module-thu-phat-bluetooth-hc-05) 80.000₫
5. Four 10 cm cable 0.25 mm² to connect the motors to the shield
6. Three female-male jumper wire to connect the bluetooth module to the Arduino (+3.3V, GND, RX)
7. So in general: some [jumper wires](https://icdayroi.com/bo-day-cam-test-board-65-soi) 19.000₫
8. Maybe [a breadboard](https://icdayroi.com/testboard-mini-syb-170) to connect 5.000₫

Here is the image from Fritzing missing ...

## Building steps

* Assemble the robot
* Connect the motors to M1 and M4 on the L293D shield
* Add the AFMotor.h motor library (library/AFMotor.zip) in the Arduino IDE
* Upload the program [T100.ino](T100.ino) to your Arduino Uno
* Install the software [Arduino Bluetooth Controller](https://play.google.com/store/apps/details?id=com.satech.arduinocontroller) to your Android phone
* Connect to the bluetooth module of the robot
* Configure the keys of the remote the following:
  - "U" for up
  - "D" for down
  - "L" for left
  - "R" for right

## Software
### T200 basic

Here the robot just blinks with the LED, attached to pin 2, and drives the motors forward and backward. The motor controller is connected to pins 16, 17, 18 and 19.

### T200 BT

The robot can be controlled, using Bluetooth 2.0 with the software ... steps follow, straigt forward.

### T200 BLE basic

Using the software ... the arrow keys forward and backward.

### T200 BLE PWM

Now we make use of the analog controller stick and its 255 values to drive the motors with PWM.

Details, instructions and pictures can be found in the [Wiki](https://github.com/kreier/T200/wiki). 
