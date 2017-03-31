#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Time.h>
#include <TimeLib.h>
#include <NewPing.h>

// Blynk
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>

#define BLYNK_PRINT Serial // Comment this out to disable prints and save space
#define ESP8266_BAUD 9600

SoftwareSerial EspSerial(3, 2); // RX, TX
char auth[] = "2dfb30515e074218b30ed47244183c3a";
char ssid[] = "La Grotteria - Maina Network";
char pass[] = "Giapi@0123456789";

ESP8266 wifi(&EspSerial);


//igrometri
int igro1_1 = A0; 
int igro1_2 = A3;
int igro1_3 = A2;
int igro1_4 = A1;
int var1_1;
int var1_2;
int var2_1;
int var2_2;
int var_med_1;
int var_med_2;
const int secco = 200;
const int normale = 400;
const int umido = 600;

// ultrasonic module
const int trigger_pin = 10; 
const int echo_pin = 11;
const int max_distance = 40;
int distanza = 0;
NewPing sonar(trigger_pin, echo_pin, max_distance); 

// temperatura
const int sens_temp = 6 ; 
OneWire oneWire(sens_temp);  
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer_1 = {0x28, 0xFF, 0x98, 0xB0, 0xA1, 0x15, 0x04, 0x7F}; //aggiungere indirizzo
DeviceAddress Thermometer_2 = {0x28, 0xFF, 0x45, 0x5B, 0x91, 0x15, 0x01, 0xF1};
float temp_1;
float temp_2;

//pompe
int pompa1 = 3;
int pompa2 = 5;
bool full = true;

//led
const int led = 4;

//ventole
const int vent_parte_1 = 8;
const int vent_parte_2 = 9;


void setup() {
  Serial.begin(9600);
  sensors.begin();

  // pompe
  pinMode(pompa1, OUTPUT);
  pinMode(pompa2, OUTPUT);

  //ventole
  pinMode(vent_parte_1, OUTPUT);
  pinMode(vent_parte_2, OUTPUT);

  //led
  pinMode(led, OUTPUT);

  // temperatura: set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(Thermometer_1, 9);
  sensors.setResolution(Thermometer_2, 9);

  delay(10);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);


}

void loop() {

  Blynk.run();
  
  //delay per sensore ultrasuoni
  delay(500);

  //LETTURA DATI
  // Lettura porte analogiche per igrometri
  var1_1 = analogRead(igro1_1);
  var1_2 = analogRead(igro1_2);
  var_med_1 = (var1_1 + var1_2)/2;
  Serial.print("Var med 1: ");
  Serial.println(var_med_1);
  bagna(var_med_1, pompa1);
  
  var2_1 = analogRead(igro1_3);
  var2_2 = analogRead(igro1_4);
  var_med_2 = (var2_1 + var2_2)/2;
  Serial.print("Var med 2: ");
  Serial.println(var_med_2);
  bagna(var_med_2, pompa2);

  //temperatura
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp_1 = sensors.getTempC(Thermometer_1);
  temp_2 = sensors.getTempC(Thermometer_2);
  Serial.println("temperatura sensore 1: " + String(temp_1));
  Serial.println("temperatura sensore 2: " + String(temp_2));
  Serial.println();
  Blynk.vitualWrite(V0, temp_1);
  Blynk.vitualWrite(V1, temp_2);
 

  if(temp_1 > 30){
    digitalWrite(vent_parte_1, HIGH);
  } else if(temp_1 < 30){
    digitalWrite(vent_parte_1, LOW);
  }

  if(temp_2 > 30){
    digitalWrite(vent_parte_2, HIGH);
  } else if(temp_2 < 30){
    digitalWrite(vent_parte_2, LOW);
  }


  //ora
  time_t tempo = now();
  int ora = hour(tempo); //controllare
  
  
  //led
  /*if((ora > 7) && (ora < 19)){
    digitalWrite(led, HIGH);
  } else{
    digitalWrite(led, LOW);
  } */

  // digitalWrite(led, HIGH); //Accensione led aggiunta su Blynk
  

  // sensore ultrasuoni e WiFi
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  distanza = uS / US_ROUNDTRIP_CM; // sono cm
  Serial.println("Distanza: ");
  Serial.print(distanza);
  Serial.println("cm");
  Blynk.virtualWrite(V2, distanza);
  
  if(distanza >= 25){
    Blynk.notify("Acqua in esaurimento");
    full = true;
  } else if(distanza >= 32) {
    Blynk.notify("Acqua finita!!");
    full = false;
  } else{
    full = true;
  }
}

void bagna(int media, int pompa){
  if((media <= secco) && (full == true)){
    Serial.println("Sto bagnando");
    digitalWrite(pompa, HIGH);
    delay(2500);
    digitalWrite(pompa, LOW);
    Serial.println("Ho bagnato"); 
  }
}
