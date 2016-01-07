##Arduino core for ESP31B WiFi chip##
===========================================
###This is a beta project for a beta chip. Do not take it as final or stable. Intention is to have a working environment to test the new hardware and prepare the core and peripheral libs for the upcoming final ESP32 release.###

###This project brings support for ESP31B chip to the Arduino environment. It lets you write sketches using familiar Arduino functions and libraries, and run them directly on ESP31B, no external microcontroller required.###

- Compile the toolchain for your platform (you should have that already if you have done any programming for it)
- Install Arduino 1.6.5+
- Go to Arduino directory
- Clone this repository into hardware/espressif/ESP31B directory (or clone it elsewhere and create a symlink)
```bash
cd hardware
mkdir espressif
cd espressif
git clone https://github.com/me-no-dev/ESP31B.git
```
- Install Python 2.7 and have it included in the PATH
- Copy your compiled toolchain into hardware/espressif/ESP31B/tools directory
- Restart Arduino

#### Also Compatible with the Arduino Eclipse plugin ####

### License and credits ###

Most of the original source comes from the [ESP8266 Arduino Project](https://github.com/esp8266/Arduino) so hats off to @igrr, @Links2004, me and many others

Arduino IDE is developed and maintained by the Arduino team. The IDE is licensed under GPL.

Espressif SDK included in this build is under Espressif MIT License.

ESP31B core files are licensed under LGPL.

[SPI Flash File System (SPIFFS)](https://github.com/pellepl/spiffs) written by Peter Andersson is used in this project. It is distributed under MIT license.
