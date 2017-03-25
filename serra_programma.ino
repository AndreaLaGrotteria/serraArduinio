#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Time.h>
#include <TimeLib.h>
#include <NewPing.h>

//igrometri
int igro1_1 = A0; 
int igro1_2 = A1;
int igro1_3 = A2;
int igro1_4 = A3;
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
const int trigger_pin = 13; 
const int echo_pin = 12;
const int max_distance = 40;
int distanza = 0;
NewPing sonar(trigger_pin, echo_pin, max_distance); 

// temperatura
const int sens_temp = 5 ; 
OneWire oneWire(sens_temp);  
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer_1; //agiungere indirizzo
DeviceAddress Thermometer_2;
float temp_1;
float temp_2;

//pompe
int pompa1 = 6;//aczsoooooo
int pompa2 = 3;
bool full = true;

//led
const int led = 50;

//ventole
const int vent_parte_1 = 6;
const int vent_parte_2 = 5;


void setup() {
  Serial.begin(9600);
  sensors.begin();


  // pompe
  pinMode(pompa1, OUTPUT);
  pinMode(pompa2, OUTPUT);


  //led
  pinMode(led, OUTPUT);

  
  // temperatura: set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(Thermometer_1, 9);
  sensors.setResolution(Thermometer_2, 9);


}

void loop() {
  //inizializza messaggio da mandare
  String messaggio;
  
  //delay per sensore ultrasuoni
  delay(500);

  //LETTURA DATI
  // Lettura porte analogiche per igrometri
  var1_1 = analogRead(igro1_1);
  var1_2 = analogRead(igro1_2);
  var_med_1 = (var1_1 + var1_2)/2;
  Serial.println("Var med 1: " + var_med_1);
  bagna(var_med_1, pompa1);
  
  var2_1 = analogRead(igro1_3);
  var2_2 = analogRead(igro1_4);
  var_med_2 = (var2_1 + var2_2)/2;
  Serial.println("Var med 2: " + var_med_2);
  bagna(var_med_2, pompa2);

  //temperatura
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp_1 = sensors.getTempC(insideThermometer_1);
  temp_2 = sensors.getTempC(insideThermometer_2);
  Serial.println("temperatura sensore 1: " + String(temp_1));
  Serial.println("temperatura sensore 2: " + String(temp_2));
  Serial.println();

  //ora
  time_t tempo = now();
  int ora = hour(tempo); //controllare
  
  
  //led
  if((ora > 7) && (ora < 19)){
    digitalWrite(led, HIGH);
  } else{
    digitalWrite(led, LOW);
  }
  

  // sensore ultrasuoni e WiFi
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  distanza = uS / US_ROUNDTRIP_CM; // sono cm
  Serial.println("Distanza: ");
  Serial.print(distanza);
  Serial.println("cm");
  
  if(distanza >= 25){
    // Messaggio all' ESP8266 che invia la notifica di acqua in esaurimento
    full = true;
  } else if(distanza >= 32) {
    //messaggio acqua finita
    full = false;
  } else{
    // Messaggio all' ESP8266 che invia la notifica
    full = true;
  }
  // aggiungere notificatore
}

void bagna(int media, int pompa){
  if((media <= secco) && (full == true)){
    Serial.println("Sto bagnando");
    digitalWrite(pompa, HIGH);
    delay(1500);
    digitalWrite(pompa, LOW);
    Serial.println("Ho bagnato"); 
  }
}
