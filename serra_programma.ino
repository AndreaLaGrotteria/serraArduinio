#include <Time.h>
#include <TimeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>
#include <ESP8266WiFi.h>

//igrometri
int igro1_1 = A0; 
int igro1_2 = A1;
int igro1_3 = A2;
int igro1_4 = A3;
int igro2_1 = A4;
int igro2_2 = A5;
int igro2_3 = A6;
int igro2_4 = A7;
int var1_1;
int var1_2;
int var1_3;
int var1_4;
int var2_1;
int var2_2;
int var2_3;
int var2_4;
int var_med_1;
int var_med_2;
const int secco = 200;
const int normale = 400;
const int umido = 600;

// ultrasonic module
const int trigger_pin = 4; // giusto?
const int echo_pin = 3;
const int max_distance = 40;
int distanza = 0;
NewPing sonar(trigger_pin, echo_pin, max_distance); 

// temperatura
const int sens_temp= 9; // controllare input, entrambi i termometri dovrebbero avere stesso pin (collegamento ?) e dovrebbe cambiare solo DeviceAdress
OneWire oneWire(sens_temp);  
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer_1;
DeviceAddress insideThermometer_2;
float temp_1;
float temp_2;

//pompe
int pompa1 = 1;
int pompa2 = 2;
bool full = true;

//led
const int led_parte_1 = 5;
const int led_parte_2 = 6;

//ventole
const int vent_parte_1 = 7;
const int vent_parte_2 = 8;

//WiFi ESP8266
const char* ssid = "YOUR_SSID";//ssid
const char* password = "YOUR_PASSWORD";//password

void setup() {
  Serial.begin(9600);
  sensors.begin();


  // pompe
  pinMode(pompa1, OUTPUT);
  pinMode(pompa2, OUTPUT);


  //led
  pinMode(led_parte_1, OUTPUT);
  pinMode(led_parte_2, OUTPUT);

  
  // temperatura: set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer_1, 9);
  sensors.setResolution(insideThermometer_2, 9);


  //WiFi
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.println("Boot OK");
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
  var1_3 = analogRead(igro1_3);
  var1_4 = analogRead(igro1_4);
  var_med_1 = (var1_1 + var1_2 + var1_3 + var1_4)/4;
  bagna(var_med_1, pompa1);
  
  var2_1 = analogRead(igro2_1);
  var2_2 = analogRead(igro2_2);
  var2_3 = analogRead(igro2_3);
  var2_4 = analogRead(igro2_4);
  var_med_2 = (var2_1 + var2_2 + var2_3 + var2_4)/4;
  bagna(var_med_2, pompa2);

  //temperatura
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp_1 = sensors.getTempC(insideThermometer_1);
  temp_2 = sensors.getTempC(insideThermometer_2);
  Serial.println("temperatura: " + temp_1);
  Serial.println(temp_2);
  Serial.println();

  //ora
  time_t tempo = now();
  int ora = hour(tempo); //controllare
  
  
  //led
  if((ora > 7) && (ora < 19)){
    digitalWrite(led_parte_1, HIGH);
    digitalWrite(led_parte_2, HIGH);
  } else{
    digitalWrite(led_parte_1, LOW);
    digitalWrite(led_parte_2, LOW);
  }
  

  // sensore ultrasuoni e WiFi
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  distanza = uS / US_ROUNDTRIP_CM; // sono cm
  if(distanza >= 30){
    messaggio = "Ricordati di riempire il serbatoio dell'acqua, è quasi vuoto!" + "\nValore umidità terreno area 1: " + var_med_1 + "    Valore umidità terreno area 2: " + var_med_2 + "\nTemperatura area 1: " + temp_1 + "    Temperatura area 2: " + temp_2;
    full = false;
  } else{
    messaggio = "Serbatoio dell'acqua OK" + "\nValore umidità terreno area 1: " + var_med_1 + "    Valore umidità terreno area 2: " + var_med_2 + "\nTemperatura area 1: " + temp_1 + "    Temperatura area 2: " + temp_2;
    full = true;
  }
  notificatore(messaggio);
  

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

/*
void notificatore(){
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
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

            // the content of the HTTP response follows the header:
            client.print(messaggio);

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
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
}
*/
