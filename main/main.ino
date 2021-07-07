#include <DS3231.h>
#include <Wire.h>

#include <DHT_U.h>
#include <DHT.h>

#include <MHZ19.h>
#include <Arduino.h>

#include <TM1637.h>

#define SCREEN_DIO 12
#define SCREEN_CLK 11
#define BUTTON 2
#include <SoftwareSerial.h>
#define RX_PIN 7
#define TX_PIN 8
SoftwareSerial mySerial(RX_PIN, TX_PIN);   

#define BAUDRATE 9600 

#define sensorDHC 3
#define DARKEST 1
#define BRIGHTER 7


#define DHTTYPE DHT22 
DHT_Unified dht(sensorDHC, DHTTYPE);

DS3231 timer;
bool h12Flag;
bool pmFlag;

byte visible[] = {1,0,0,0};
int periodo = 1000;
unsigned long tiempoAhora = 0;

TM1637 screen(SCREEN_CLK, SCREEN_DIO);
MHZ19 sensorC02; 

void setup() {


  delay(1000);
  mySerial.begin(9600); 
  pinMode(BUTTON, INPUT);
  screen.begin(); 
  screen.display("HOLA");
  sensorC02.begin(mySerial);
  //sensorC02.setFilter(true, false);  
  sensorC02.autoCalibration();
    
  dht.begin();
  Wire.begin();
  Serial.begin(BAUDRATE);

  screen.setBrightness(DARKEST);
  //adjustTime();
}

void loop() {
  
  tiempoAhora = millis();
  while(millis() < tiempoAhora+periodo) {
    int buttonState = digitalRead(BUTTON);
  
    if (buttonState == HIGH) {
      Serial.println("pulsado");
      printNext();
      break;
    }
  }
  printActual();
  delay(200);
   
}

void printActual(){
  if (visible[0] == 1){
    printTime();
  }else if (visible[1] == 1){
     printCO2();
  }else if (visible[2] == 1){
     printTemperature();
  }else if (visible[3] == 1){
     printHumidity();
  }
  
}

void printNext() {
  if (visible[0] == 1){
    visible[0] = 0;
    visible[1] = 1;
  }else if (visible[1] == 1){
    visible[1] = 0;
    visible[2] = 1;
  }else if (visible[2] == 1){
    visible[2] = 0;
    visible[3] = 1;
  }else if (visible[3] == 1){
    visible[3] = 0;
    visible[0] = 1;
  }
  screen.colonOff();
  screen.clearScreen();
  //printActual();
}

void printTime() {


  int hour = timer.getHour(h12Flag, pmFlag);
  int minute = timer.getMinute();
  screen.colonOn();
  String hora;
  String minuto;
  if (minute<10){
    if(minute<1){
        minuto = "00";
    }else{
        minuto = "0"+(String)minute;
    }
  }else{
    minuto = (String)minute;
  }
   
  if (hour<10){
    if(hour<1){
        hora = "00";
    }else{
        hora = "0"+(String)hour;
    }
  }else{
    hora = (String)hour;
  }
  
  screen.display((String)(hora+minuto));
 
}

void adjustTime() {

  timer.setClockMode(false);  // set to 24h

  timer.setYear(2021);
  timer.setMonth(7);
  timer.setHour(13);
  timer.setMinute(4);
  timer.setSecond(1);
}


void printCO2() {
  
  int co2 = sensorC02.getCO2();
  Serial.println("CO2 LECTURA ");
  Serial.println(co2);
  if (co2 < 1000){
    screen.display(co2, true, false, 1);  
  }else{
    screen.display(co2, true, false, 0);
  }
  
}

void printTemperature() {


 
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temp = 0.0;
  if (isnan(event.temperature)) {
    dht.temperature().getEvent(&event);
  }
  if (!isnan(event.temperature)) {
    temp = event.temperature;
    String tempS = (String) temp;
    tempS.replace(".","_");
    screen.display(tempS);
  }else{
    screen.display(temp);
  }
  
 
}

void printHumidity() {

  sensors_event_t event; 
  float hum = 0.0;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    dht.humidity().getEvent(&event);
  }
  if (!isnan(event.relative_humidity)) {
    hum = event.relative_humidity;
    String humS = (String) hum;
    humS.replace(".","_");
    screen.display(humS);
  }else{
    screen.display(hum);
  }

 
}
