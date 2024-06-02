#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(9, 10);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int moistPin = A0;
float soilRes;
float tempC;
int buzz = 13; // Specify the buzzer pin here
int moistV1,moistV2;
int l = 1;
int A = 1;
bool thresholdExceeded = false;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  mySerial.begin(9600);
  lcd.begin(16, 2);
  pinMode(moistPin, INPUT);
  pinMode(buzz, OUTPUT);
  lcd.setCursor(0,0);
  // lcd.print("SOIL RESISTANCE");
  // lcd.setCursor(0,1);
  // lcd.print(" INSTRUMENT");
  // delay(100);
  // lcd.clear();
  lcd.print("TEMP ");
  lcd.print("MOIST ");
  lcd.print(" RES ");
}


void loop() {
    sensors.requestTemperatures();
    moistV1 = analogRead(moistPin);
    moistV2 = map(moistV1, 0, 1023, 100, 0);
    tempC = sensors.getTempCByIndex(0);
    soilRes = (l * 133.5) / (A * tempC * moistV2);
    String tempS = String(tempC);
    if (tempS == "-127.00") {
    //do nothing
    } else {
      lcd.setCursor(0, 1);
      lcd.print(int(tempC));
      lcd.print((char)223);
      lcd.print("C ");
      lcd.print(moistV2);
      lcd.print(" % ");
      lcd.print(soilRes);
    }
    
    if (soilRes > 5 && !thresholdExceeded) {
      SendMessage();
      thresholdExceeded = true;
      digitalWrite(buzz, HIGH);
      delay(100);
    } else if (soilRes <= 5 && thresholdExceeded) {
      thresholdExceeded = false;
      digitalWrite(buzz, LOW);
    }
}


void SendMessage() {
    if (soilRes > 0) {
    mySerial.println("AT+CMGF=1");
    mySerial.println("AT+CMGS=\"+1251234567\"\r");
    mySerial.println("Soil Resistance: " + String(soilRes, 2));
    mySerial.println(char(26));
  } 
}