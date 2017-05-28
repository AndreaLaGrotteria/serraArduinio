#define BLYNK_PRINT Serial
#define BLYNK_DEBUG

#include <SimpleTimer.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <NewPing.h>

// Blynk
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#include <TimeLib.h>
#include <WidgetRTC.h>

#define ESP8266_BAUD 9600

SoftwareSerial EspSerial(11, 10); // RX dell'esp8266, TX dell'esp8266
char auth[] = "2dfb30515e074218b30ed47244183c3a";
char ssid[] = "toolbox";
char pass[] = "Toolbox.Torino";

ESP8266 wifi(&EspSerial);

//igrometri
const char igro1_1 = A1; 
const char igro1_2 = A2; 
const char igro2_1 = A5; 
const char igro2_2 = A4; 
int var1_1;
int var1_2;
int var2_1;
int var2_2;
int var_med_1;
int var_med_2;
int climaTerreno_1 = 5;
int climaTerreno_2 = 5;

// ultrasonic module
const char trigger_pin = 3; //aggiungere pin
const char echo_pin = 4;    //aggiungere pin
const int max_distance = 40;
int distanza = 0;
int perc_acqua = 0;
NewPing sonar(trigger_pin, echo_pin, max_distance);

// temperatura
const char sens_temp;  //aggiungere pin
OneWire oneWire(sens_temp);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer_1 = {0x28, 0xFF, 0x98, 0xB0, 0xA1, 0x15, 0x04, 0x7F};
DeviceAddress Thermometer_2 = {0x28, 0xFF, 0x45, 0x5B, 0x91, 0x15, 0x01, 0xF1};
float temp_1;
float temp_2;

//pompe
const char pompa_1 = 8; 
const char pompa_2 = 9; 
bool full = true;

//led
const char led = 6;

//ventole
const char ventola_1 = 1;
const char ventola_2 = 0;
int climaAria_1 = 26;
int climaAria_2 = 26;

//timer
SimpleTimer timer;

//RTC
WidgetRTC rtc;

//menù blynk
//Ambiente 1
/*BLYNK_WRITE(V0) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Clima caldo selezionato - ambiente 1");
      climaAria_1 = 35;
      climaTerreno_1 = 100;
      break;
    case 2: // Item 2
      Serial.println("Clima temperato selezionato - ambiente 1");
      climaAria_1 = 26;
      climaTerreno_1 = 200;
      break;
    case 3: // Item 3
      Serial.println("Clima freddo selezionato - ambeiente 1");
      climaAria_1 = 17;
      climaTerreno_1 = 300;
      break;
    default:
      Serial.println("Errore con la selezione del clima - ambiente 1");
  }
}

BLYNK_WRITE(V1) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Clima caldo selezionato - ambiente 2");
      climaAria_2 = 35;
      climaTerreno_2 = 100;
      break;
    case 2: // Item 2
      Serial.println("Clima temperato selezionato - ambiente 2");
      climaAria_2 = 26;
      climaTerreno_2 = 200;
      break;
    case 3: // Item 3
      Serial.println("Clima freddo selezionato - ambeiente 2");
      climaAria_2 = 17;
      climaTerreno_2 = 300;
      break;
    default:
      Serial.println("Errore con la selezione del clima - ambiente 2");
  }
} */

void setup() {
  Serial.begin(9600);

  delay(100);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(100);
  Blynk.begin(auth, wifi, ssid, pass);

  rtc.begin();

  timer.setInterval(10000, core);

  sensors.begin();

  // pompe
  pinMode(pompa_1, OUTPUT);
  pinMode(pompa_2, OUTPUT);

  //ventole
  pinMode(ventola_1, OUTPUT);
  pinMode(ventola_2, OUTPUT);

  //led
  pinMode(led, OUTPUT);

  Serial.println("Boot OK");

}

void loop() {
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}

void core(){

  // Lettura porte analogiche per igrometri
  var1_1 = analogRead(igro1_1);
  var1_2 = analogRead(igro1_2);
  if((var1_1 > 0) && (var1_2 > 0)){
    var_med_1 = (var1_1 + var1_2)/2;
    Serial.print("Var med 1: ");
    Serial.println(var_med_1);
    bagna(var_med_1, pompa_1, climaTerreno_1);
  } else{
    Serial.println("Errore igrometri area 1");
  }

  var2_1 = analogRead(igro2_1);
  var2_2 = analogRead(igro2_2);
  if((var2_1 > 0) && (var2_2 > 0)){
    var_med_2 = (var2_1 + var2_2)/2;
    Serial.print("Var med 2: ");
    Serial.println(var_med_2);
    bagna(var_med_2, pompa_2, climaTerreno_2);
  } else{
    Serial.println("Errore igrometri area 2");
  }

  //temperatura
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp_1 = 27;
  temp_2 = 26;

  /*Serial.println("temperatura sensore 1: " + String(temp_1));
  Serial.println("temperatura sensore 2: " + String(temp_2));
  Serial.println(); */
  Blynk.virtualWrite(V4, temp_1);
  Blynk.virtualWrite(V5, temp_2);

  //ventole
  if(temp_1 > climaAria_1){
    digitalWrite(ventola_1, HIGH);
  } else if(temp_1 < climaAria_1){
    digitalWrite(ventola_1, LOW);
  }

  if(temp_2 > climaAria_2){
    digitalWrite(ventola_2, HIGH);
  } else if(temp_2 < climaAria_2){
    digitalWrite(ventola_2, LOW);
  }

  // sensore ultrasuoni e WiFi
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  distanza = uS / US_ROUNDTRIP_CM; // sono cm
  Serial.print("Distanza: ");
  Serial.print(distanza);
  Serial.println("cm");
  perc_acqua = ((40 - distanza)*1023)/40;
  Blynk.virtualWrite(V3, perc_acqua);

  if(distanza >= 25){
    Blynk.notify("Acqua in esaurimento");
    full = true;
  } else if(distanza >= 32) {
    Blynk.notify("Acqua finita!!");
    full = false;
  } else{
    full = true;
  }


  //led
  if((hour() > 8) && (hour() < 20)){
    digitalWrite(led, LOW);
  } else{
    digitalWrite(led, HIGH);
  }

}

void bagna(int media, int pompa, int secco){
  if((media <= secco) && (full == true)){
    Serial.println("Sto bagnando");
    digitalWrite(pompa, LOW);
    delay(1500);
    digitalWrite(pompa, HIGH);
    Serial.println("Ho bagnato");
    delay(1500);
  }
}
