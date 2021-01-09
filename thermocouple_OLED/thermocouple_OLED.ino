// this example is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "max6675.h" //todo: move to global install?

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

int thermoDO = 12;
int thermoCS = 10;
int thermoCLK = 14; //NOTE: pin 13 is built in LED, may be problematic with teensy!

float degC, degF;
int toggle = 0;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

void setup() {
  pinMode(13, OUTPUT); //LED
  for (int i = 0; i < 10; i++) {
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    delay(50);
  }


  //Serial.begin(9600);
  // use Arduino pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(1000);

  //Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(1000);
}

void loop() {
  // basic readout test, just print the current temp


  degC = thermocouple.readCelsius();
  degF = degC * 9.0 / 5.0 + 32;
  //  Serial.print("C = ");
  //  Serial.println(degC);
  //  Serial.print("F = ");
  //  Serial.println();
  delay(200);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  toggle = !toggle;
  display.print("Curr Temp ");
  display.println(toggle);
  display.setTextSize(2);
  display.print(degC);
  display.println(" C");
  display.print(degF);
  display.println(" F");
  display.display();
  digitalWrite(13, toggle);

  delay(1000);
}
