#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <UniversalTelegramBot.h>


// Initialize Wifi connection to the router
char ssid[] = "G3 di Ninotz";     // your network SSID (name)
char password[] = "chitarra"; // your network key

#define BOTtoken "330278087:AAGI9xAt1E5sehRy0lT70ul8Mky3VEqQU-w"

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(9600);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  

}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      for (int i=0; i<numNewMessages; i++) {
        if (bot.messages[i].text == "\sensori"){
          bot.sendMessage(bot.messages[i].chat_id, "Livello acqua: 60% \n Umidità media: 20% \n Temperatura media: 25C", "");
        }
        else if (bot.messages[i].text == "\acqua"){
          bot.sendMessage(bot.messages[i].chat_id, "Livello acqua: 60%", "");
        }
        else{
          bot.sendMessage(bot.messages[i].chat_id, "Spiacente, non posso risponderti", "");
        }
        
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  } 

}
