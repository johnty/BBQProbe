#ifndef LED_BUILTIN
#define LED_BUILTIN 22
#endif

#include "max6675.h" //todo: move to global install?
unsigned long t_prev;

//TTGO
int thermoDO = 19;
int thermoCS = 23;
int thermoCLK = 18; //NOTE: pin 13 is built in LED, may be problematic with teensy!

//teensy
//int thermoDO = 21;
//int thermoCS = 19;
//int thermoCLK = 18; //NOTE: pin 13 is built in LED, may be problematic with teensy!

double tempA; //for TC4
double tempB; //for TC4
bool unit_F = false; //for TC4

float degC, degF;
int toggle = 0;



MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

//#################################################################################
//code from https://github.com/FilePhil/TC4-Emulator/blob/master/TC4-Emulator.ino
void handleSerialCommand(){   

    if (Serial.available()>0){
        String msg = Serial.readStringUntil('\n');

        if (msg.indexOf("CHAN;")== 0){  //Ignore this Setting
            Serial.print("#OK");
        }
        else if (msg.indexOf("UNITS;")== 0){

            if (msg.substring(6,7)=="F"){   //Change to Farenheit
                unit_F = true;
                Serial.println("#OK Farenheit");
            }
            else if (msg.substring(6,7)=="C"){  //Change to Celsius
                unit_F = false;
                Serial.println("#OK Celsius");
            }

        }
        else if (msg.indexOf("READ")==0){   //Send Temps
           Command_READ();

       }
   }

}
//Send Data
void Command_READ(){    
    Serial.print("0.00,");
    Serial.print(tempA);
    Serial.print(",");
    Serial.print(tempB);
    Serial.println(",0.00,0.00");
}
//end code snippet from https://github.com/FilePhil/TC4-Emulator/blob/master/TC4-Emulator.ino
//#################################################################################

void setup() {

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT); //LED

  //fast blink: connection OK!
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }

  //Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(1000);
  t_prev = millis();
}

void loop() {
  if (millis() - t_prev > 1000) { //read temp every second
    degC = thermocouple.readCelsius();
    degF = degC * 9.0 / 5.0 + 32;
    if (unit_F) {
       tempA = degF;
       tempB = degF;
    }
    else {
       tempA = degC;
       tempB = degC;
    }
    toggle = !toggle;
    digitalWrite(LED_BUILTIN, toggle);
    t_prev = millis();
  }
  handleSerialCommand();
}
