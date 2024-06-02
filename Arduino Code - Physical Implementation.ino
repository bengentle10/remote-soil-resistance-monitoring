// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial mySerial(7, 8);
OneWire oneWire(2);

DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

const int moistTemp = A0;
const int buzzpin = 10;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(moistTemp, INPUT);
  pinMode(buzzpin, OUTPUT);

  // locate devices on the bus
  sensors.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("INITIALIZING...");
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(5000);
  lcd.clear();
  lcd.print("TEMP MOIST RES ");
  

}


void loop(void)
{ 

  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);

  float moistV1 = analogRead(moistTemp);
  int moistV2 = map(moistV1, 0, 1023, 100, 0);

  if (moistV2 == 0) {
    moistV2 = 1;
  }
  
  float soilRes = (1.0 / tempC)*(133.5 / (float)moistV2);

  if (tempC != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(0, 1);
    lcd.print(int(tempC));
    lcd.write(223);
    lcd.print("C  ");
    lcd.print(moistV2);
    lcd.print(" %  ");
    lcd.print(soilRes, 2);

    if (soilRes >= 5.0 && isfinite(soilRes)) {
      sendSoilResistSMS(soilRes);
      buzz();
    }
  }

}


void sendSoilResistSMS(float soilRes) {
  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+233504147999\""); // change ZZ with country code and +233504147999 with phone number to sms
  updateSerial();
  mySerial.print("Soil Resistance: ");
  mySerial.print(soilRes); //text content
  updateSerial();
  mySerial.write(26);
}


void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void buzz() {
  digitalWrite(buzzpin, 1);
  delay(2000);
  digitalWrite(buzzpin, 0);
  delay(10);
}
