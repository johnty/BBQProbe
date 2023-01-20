//ESP32 based thermocouple application
// Computes Rate of Rise of temperature, for coffee roasting and simliar applications
// Plots to SSD1306 OLED, and also serial port, and serves it as html text via webserver as well.

// uncomment #define below to run unittest-like functions for checking RoR computation algorithm,
// which are executed from setup():
//#define TESTROR

#ifndef LED_BUILTIN
#define LED_BUILTIN 22
#endif

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "max6675.h" //todo: move to global install?

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const char* ssid     = "ssid";
const char* password = "password";

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

unsigned long t_prev;

WiFiServer server(80);



int thermoCLK = 18; //NOTE: pin 13 is built in LED, may be problematic with teensy!
int thermoDO = 19;
int thermoCS = 23;

float degC, degF;
float degFperMin;
int toggle = 0;

#define VEC_SIZE 20
float degFhist[VEC_SIZE];
int writeIdx = 0;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;


void setup() {

  Serial.begin(115200);
#ifdef TESTROR
  testRoR();
#endif

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT); //LED
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  //slow blink while attempting to connect wifi
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    Serial.print(".");
  }

  //fast blink: connection OK!
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();



  // use Arduino pins
  //pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  //pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  //  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //
  //  display.clearDisplay();
  //  display.drawPixel(10, 10, WHITE);
  //  display.display();
  //  delay(1000);

  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(1000);
  t_prev = millis();
}

void loop() {

  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {
    processClient(client);
  }

  if (millis() - t_prev > 1000) { //read temp every second
    degC = thermocouple.readCelsius();
    degF = degC * 9.0 / 5.0 + 32;
    degFperMin = getRoR(degF);
    Serial.print("C = ");
    Serial.print(degC);
    Serial.print("F = ");
    Serial.println(degF);
    
    toggle = !toggle;
    digitalWrite(LED_BUILTIN, toggle);
    t_prev = millis();
    updateDisplay();
  }
  //  display.clearDisplay();
  //  display.setTextSize(2);
  //  display.setTextColor(WHITE);
  //  display.setCursor(0, 0);
  //  toggle = !toggle;
  //  display.print("Curr Temp ");
  //  display.println(toggle);
  //  display.setTextSize(2);
  //  display.print(degC);
  //  display.println(" C");
  //  display.print(degF);
  //  display.println(" F");
  //  display.display();
  //  digitalWrite(13, toggle);
}

void processClient(WiFiClient& client) {
  Serial.println("New Client.");           // print a message out the serial port
  String currentLine = "";                // make a String to hold incoming data from the client
  while (client.connected()) {            // loop while the client's connected
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();             // read a byte, then
      Serial.write(c);                    // print it out the serial monitor
      if (c == '\n') {                    // if the byte is a newline character

        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.print("<head><meta http-equiv=\"refresh\" content=\"5\"></head>");
          // the content of the HTTP response follows the header:
          //client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
          //client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
          client.print("Probe 1 temp = ");
          client.print(degC);
          client.print(" C / ");
          client.print(degF);
          client.print(" F. <br>");

          // The HTTP response ends with another blank line:
          client.println();
          // break out of the while loop:
          break;
        } else {    // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }

      // Check to see if the client request was "GET /H" or "GET /L":
      if (currentLine.endsWith("GET /H")) {
        digitalWrite(5, HIGH);               // GET /H turns the LED on
      }
      if (currentLine.endsWith("GET /L")) {
        digitalWrite(5, LOW);                // GET /L turns the LED off
      }
    }
  }
  // close the connection:
  client.stop();
  Serial.println("Client Disconnected.");
}

void updateDisplay()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  //display.print("Curr Temp ");
  //display.println(toggle);
  display.print(degF);
  display.println(" F");
  display.print(degFperMin);
  display.println(" F/m");
  display.display();
}

float getRoR(float currTemp)
{
  //we have VEC_SIZE seconds of readings
  // taken at 1 second intervals
  // starting at writeIdx, wrapping around the array
  degFhist[writeIdx] = currTemp;
  writeIdx++;
  if (writeIdx >= VEC_SIZE) {
    writeIdx = 0;
  }
  float v1 = 0;
  float v2 = 0;
  for (int i = 0; i < VEC_SIZE; i++) {
    int readIdx = (i + writeIdx) % VEC_SIZE;
    if (i < VEC_SIZE / 2) {
      v1 += degFhist[readIdx];
    }
    else {
      v2 += degFhist[readIdx];
    }
  }
  v1 = v1 * 2 / VEC_SIZE;
  v2 = v2 * 2 / VEC_SIZE;
  return 60 * (v2 - v1) / (VEC_SIZE / 2);
}



//###################### ROR TEST CODE
//###################### do not insert other code in this section
#ifdef TESTROR
void testRoR()
{
  Serial.println("****BEGIN test RoR****");

  for (int i = 0; i < VEC_SIZE; i++) {
    testOneRoR(0);
  }
  for (int i = 0; i < VEC_SIZE; i++) {
    testOneRoR(1);
  }

  Serial.println("begin 1 deg/s rise, RoR should be 60");
  for (int i = 0; i < 100; i++) {
    testOneRoR(i + 0.5);
  }
  Serial.println("end 1 deg/s rise. was ror 60?\n\n");


  Serial.println("!!!begin -2 deg/s drop from 100; RoR should be -120");
  for (int i = 100; i >= 0; i -= 2) {
    testOneRoR(i - 0.2);
  }
  Serial.println("!!!!end -2 deg/s drop. was ror - 120?\n\n");

  Serial.println("!!!begin random test; RoR should be ???");
  for (int i = 0; i < 100; i++) {
    testOneRoR(20 + random(1, 500) / 100.0);
  }

  Serial.println("****END test RoR****\n\n");
}

void testOneRoR(float in) {
  float ror = getRoR(in);
  Serial.print("push ");
  Serial.print(in);
  Serial.print(".    ror=");
  Serial.println(ror);
}
#endif
###################### //END ROR TEST CODE
