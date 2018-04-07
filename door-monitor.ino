//  notbookies 8/5/17

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>  //used to connect to multiple networks
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>  // used for cleaner JSON code/logic
#include "index.h"  // the HTML/CSS/JavaScript is in this separate file, as a string

#define DOOR_PIN 5
#define LED_PIN 2
#define NETWORK "********"
#define PASSWD "********"

ESP8266WiFiMulti WiFiMulti;  //create an instance of the ESP8266WiFiMulti class
ESP8266WebServer server(80);    //listen on port 80

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

char dataBuffer[256]; // string that will hold JSON object
bool  lastState;
int   visitors = 0;

unsigned long start, finished = millis();
unsigned long elapsed,
              longestOpen, longestClosed,
              avgOpenCount, avgClosedCount,
              avgOpenTot, avgClosedTot,
              avgOpen, avgClosed = 0;

void handleWifi() {
  
  WiFiMulti.addAP(NETWORK, PASSWD);
  Serial.println();
  Serial.print("Wait for WiFi ...");
  
  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
  
}

void handleMDNS() {
  
  // mDNS service - to be able to resolve the name "doorsensor" in a browser
  if (!MDNS.begin("doorsensor")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http", "tcp", 80); // add service to MDNS

}

//  blink LED x number of times when system is ready
void systemReady(int blinks) {
  int dT = 300;
  for (int x = 0; x < blinks; x++) {
    digitalWrite(LED_PIN, HIGH);
    delay(dT);
    digitalWrite(LED_PIN, LOW);
    delay(dT);
  }
}

void setup() {

  Serial.begin(115200);

  pinMode     (DOOR_PIN,  INPUT);
  pinMode     (LED_PIN,   OUTPUT);
  digitalWrite(LED_PIN,   LOW);

  lastState = digitalRead(DOOR_PIN);

  handleWifi();
  handleMDNS();
  systemReady(3);
  
  // handle index and GET request
  server.on ( "/",      []() {server.send ( 200, "text/html",         SITE_index ); });
  server.on ( "/data",  []() {server.send ( 200, "application/json",  dataBuffer ); });
  server.begin();

}
  
void loop() {

  if(WiFiMulti.run() != WL_CONNECTED){      // auto-reconnect (hopefully)
    Serial.println("Wifi not connected!");
    delay(1000);
    handleWifi();
  }
  
  bool doorState = digitalRead(DOOR_PIN);
  
  if (doorState != lastState){  // if state has changed
    finished = millis();
    elapsed = finished - start; // time since last change
    root.prettyPrintTo(Serial);
    start = millis();           // this needs to be after elapsed is calculated

    if (doorState == 0){            // if door is open
      digitalWrite(LED_PIN, LOW);
      avgClosedCount ++;
      avgClosedTot += elapsed;
      avgClosed = avgClosedTot / avgClosedCount;
      if (elapsed > longestClosed){
        longestClosed = elapsed;    // set longestClosed based on time since last open
      }
    } else if (doorState == 1){     // if door is closed
      digitalWrite(LED_PIN, HIGH);
      avgOpenCount ++;
      avgOpenTot += elapsed;
      avgOpen = avgOpenTot / avgOpenCount;
      if (elapsed > longestOpen){
        longestOpen = elapsed;      // set longestOpen based on time since last open
      }
    }
  }
  
  delay(100);   // poor man's debounce
  // counting a visitor if the door goes from closed to open (full cycle)
  if (doorState == 0 && lastState == 1){
    visitors ++;
  }

  // add variables to JSON object
  root["doorMsg"]       = doorState;
  root["timer"]         = millis() - start;
  root["visitors"]      = visitors;
  root["longestOpen"]   = longestOpen;
  root["longestClosed"] = longestClosed;
  root["avgOpen"]       = avgOpen;
  root["avgClosed"]     = avgClosed;
  root.printTo(dataBuffer, sizeof(dataBuffer));   // send JSON object to string
  
  server.handleClient();  // dont know what this does
  lastState = doorState;  // for determining if the state changes
}

