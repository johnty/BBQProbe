quick and dirty arduino based thermocouple bbq probe with ssd1306 based OLED display using the MAX6675 driver chip (now discontinued as of 2016, but still widely available on the internet)

requires Adafruit libraries for OLED
MAX6675 library included here

used on a teensy LC but should work fine on most arduino-compatible board with enough pins.

see main sketch for SPI pins to drive the thermocouple. does not require hardware SPI port and can use almost any pin for this if you want to hook up extra probes!
