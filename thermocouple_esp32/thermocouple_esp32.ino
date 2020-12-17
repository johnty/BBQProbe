#include <WiFi.h>
#include "max6675.h" //todo: move to global install?

const char* ssid     = "ssid_here";
const char* password = "pass_here";

unsigned long t_prev;

WiFiServer server(80);

int thermoDO = 21;
int thermoCS = 19;
int thermoCLK = 18; //NOTE: pin 13 is built in LED, may be problematic with teensy!

float degC, degF;
int toggle = 0;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;


void setup() {

  Serial.begin(115200);

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
    Serial.print("C = ");
    Serial.print(degC);
    Serial.print("    F = ");
    Serial.println(degF);
    toggle = !toggle;
    digitalWrite(LED_BUILTIN, toggle);
    t_prev = millis();
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
