/*------------------------------------------------------------------------------
  Copyright  @ DVXLAB , All rights reserved 
  Distributed under MIT License
------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
Oakplug  pins 
1-3.3 -outer side
2-gpio0
3-RX
4-TX
5-gnd

relay r4 ->gpio4
led white l2 ->gpio13
led orange l3-> gpio12


------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <WiFiUdp.h>

#define AP_MODE

//any one 
//#define ESP01
#define OAKPLUG
//#define NODEMCU


#ifdef ESP01
   #define RELAY 0 // ESP 01 relay
   #define LED  1 //ESP 01 TX->LED
#endif
#ifdef OAKPLUG
   #define RELAY 4
   #define LED  13 
#endif 

#ifdef NODEMCU
    #define LED  2  // Node MCU
 #endif   

#ifdef AP_MODE
IPAddress local_IP(192, 168, 4, 101);
IPAddress gateway(192, 168, 4, 101);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4); 
char* ssid = "ePySwitch";
char* password = "e12345678";
#else 
IPAddress local_IP(192, 168, 0, 101);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);
char* ssid = "Hawkxxxx";
char* password = "e12345678";
#endif
//const int Port = 80;
//WiFiUDP Udp;

WiFiServer server(80);




void setup()
{  
  bool res = SPIFFS.begin();
  #ifndef ESP01
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting ESP-01 Access Point...");
  #endif
  //Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  
  //Wifi Init
#ifdef AP_MODE
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)){
    #ifndef ESP01
    Serial.println("Soft AP config Failed");
    #endif
    digitalWrite(RELAY, HIGH);
  }
  if (!WiFi.softAP(ssid, password)){
    #ifndef ESP01
    Serial.println("Soft AP Failed");
    #endif
    digitalWrite(RELAY, HIGH);
  }
  #ifndef ESP01
   Serial.println(WiFi.softAPIP());
   #endif
#else 
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);

  } 
 #endif 

  server.begin();
  #ifndef ESP01
  Serial.println("ESP-01 Web Server ");
  Serial.println();
  #endif
}

void loop()
{
  static bool LED_Status=true;
  static int value = LOW;
  static unsigned long l = 0;
  static unsigned long task1_Led = 0;
  unsigned long t = millis();
  //1.LED blinking Task
  if((t - task1_Led ) > 500){
    task1_Led = t;
    if ( LED_Status== false)
       digitalWrite(LED, HIGH);   
    else                    
       digitalWrite(LED, LOW);    
    LED_Status=!LED_Status;    
  }
  
  //2.Packet Processing
  WiFiClient client = server.available();
  if (!client) {
    return;
  }   
 
  // Read the first line of GET containing request command
  
  String request = client.readStringUntil('\r');
  #ifndef ESP01
  Serial.println(request);
  #endif
  client.flush();
  // Check command string and perform request
 
  
  
  if (request.indexOf("/relay/0") != -1){
    digitalWrite(RELAY, HIGH);
    value = LOW;
  }
  
  if (request.indexOf("/relay/1") != -1) {
    digitalWrite(RELAY, LOW);
    value = HIGH;
  } 
  // Server Response

  // Header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // header end marker
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
  client.println("<body><h1> Web Switch</h1>");
  if(value == HIGH)
    client.println("<p>RELAY STATUS: ON </p>"); 
  else 
    client.println("<p>RELAY STATUS: OFF </p>");

  if (value != HIGH) 
    client.println("<p><a href=\"/relay/1\"><button class=\"button\">ON</button></a></p>");
  else 
    client.println("<p><a href=\"/relay/0\"><button class=\"button button2\">OFF</button></a></p>");
  client.println("</body></html>");
  client.println();
  delay(0.5);
  #ifndef ESP01
  Serial.println("Client disconnected");
  Serial.println("");
  #endif
}

