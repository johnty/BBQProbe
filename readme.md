#some MAX6675 examples with Arduino / Teensy

uses Adafruit MAX6675 library (included)

change the pins to ones on your board.

**thermocouple_serial**: sends reading via serial port

**thermocouple_esp32**: esp32 version that runs a simple server that allows probing of temperature via http requests

**thermocouple_OLED**: displays temperature on a ssd1306 based OLED screen

**thermocouple_BLE_UART**: esp32 version that implements a BLE UART service (using BLE Arduino example) and sends a single temperature string via ASCII. Can be used with the Adafruit Bluefruit LE Connect plotter/logger

TODO: delete multiple copies of MAX6675 library and reference root folder
