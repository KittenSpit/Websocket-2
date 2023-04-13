/* 
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-web-server-websocket-sliders/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <Arduino_JSON.h>
#include "Button2.h" //  https://github.com/LennartHennigs/Button2
#include "ESPRotary.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"

#include "Wait2.h"


#include <SPI.h>
#include <Wire.h>

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


/////////////////////////////////////////////////////////////////

#define ROTARY_PIN1	D5
#define ROTARY_PIN2	D6
#define BUTTON_PIN	D7

#define CLICKS_PER_STEP   4   // this number depends on your rotary encoder 
#define MIN_POS         -1
#define MAX_POS         11
#define START_POS       0
#define INCREMENT       1  // this number is the counter increment on each step

#define SERIAL_SPEED    115200

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ESPRotary r;
Button2 b;

// Replace with your network credentials

const char* ssid = "PHD1 2.4";
const char* password = "Andrew1Laura2";

Wait2 outputNow (5000);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set LED GPIO
const int ledPin1 = 14;
const int ledPin2 = 12;
const int ledPin3 = 13;

String message = "";
String sliderValue1 = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";
String sliderValue4 = "0";

int dutyCycle1;
int dutyCycle2;
int dutyCycle3;
int dutyCycle4;

String string_1 = "";
String old_string_1 = "";
String string_2 = "";
String old_string_2 = "";
String string_3 = "";
String old_string_3 = "";

//Json Variable to Hold Slider Values
JSONVar sliderValues;

//Get Slider Values
String getSliderValues(){
  sliderValues["sliderValue1"] = String(sliderValue1);
  sliderValues["sliderValue2"] = String(sliderValue2);
  sliderValues["sliderValue3"] = String(sliderValue3);
  sliderValues["sliderValue4"] = String(sliderValue4);
  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

void Display_String(String _the_string,int _x,int _y,int _size,int _show)
{
   display.setTextSize(_size);       
   if ((_show == 1)) {display.setTextColor(SSD1306_WHITE);}
   else display.setTextColor(SSD1306_BLACK);
   display.setCursor(_x, _y);
   display.print(_the_string);
   display.display();
}



// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("");
    Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
   display.setTextColor(1);
   display.setCursor(0, 40);
   display.print(WiFi.localIP());
   display.display();
 
  
  delay(1000);
}

void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0) {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 1023);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 1023);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }    
    if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 1023);
      Serial.println(dutyCycle3);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("4s") >= 0) {
      sliderValue4 = message.substring(2);
      dutyCycle4 = map(sliderValue4.toInt(), 0, 100, 0, 1023);
      Serial.println(dutyCycle4);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (strcmp((char*)data, "getValues") == 0) {
      notifyClients(getSliderValues());
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

/////////////////////////////////////////////////////////////////

// on change
void rotate(ESPRotary& r) {
  sliderValue2=r.getPosition()*10;
  Serial.println(sliderValue2);
  dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 1023);
  notifyClients(getSliderValues());
}

// on left or right rotation
void showDirection(ESPRotary& r) {
  Serial.println(r.directionToString(r.getDirection()));
}

  // out of bounds event
void upper(ESPRotary& r) {
   Serial.println("upper bound hit");
}

// out of bounds event
void lower(ESPRotary& r) {
   Serial.println("lower bound hit");
}

 
// single click
void click(Button2& btn) {
  Serial.println("Click!");
}

// long click
void resetPosition(Button2& btn) {
   r.resetPosition();
      sliderValue2=0;
  dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 1023);
  notifyClients(getSliderValues());
  Serial.println("Reset!");
}

/////////////////////////////////////////////////////////////////

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {

 // pinMode(ledPin1, OUTPUT);
 // pinMode(ledPin2, OUTPUT);
 // pinMode(ledPin3, OUTPUT);

  Serial.begin(SERIAL_SPEED);
  delay(50);
  Serial.println("\n\nSimple Counter");
  
  //r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP);
  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);
  r.setLowerOverflowHandler(lower);
  r.setUpperOverflowHandler(upper);

  b.begin(BUTTON_PIN);
  b.setDebounceTime(100);
  b.setLongClickTime(300);
  b.setTapHandler(click);
  b.setLongClickHandler(resetPosition);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); // Clear display buffer
  display.display();
  display.setRotation(2);



// Added **************
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
      Serial.println("Try to open for Read text.txt");
  File file = LittleFS.open("/text.txt", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
//*************


  initFS();
  initWiFi();
  initWebSocket();
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "index.html", "text/html");
  });
  
  server.serveStatic("/", LittleFS, "/");

  // Start server
  server.begin();
}

void loop() {

  r.loop();
  b.loop();

  //analogWrite(ledPin1, dutyCycle1);
  //analogWrite(ledPin2, dutyCycle2);
  //analogWrite(ledPin3, dutyCycle3);

 
  string_1 = String(dutyCycle1); 
  string_2 = String(dutyCycle2); 
  string_3 = String(dutyCycle3); 
  
//---------------------------------------------------------------------
  if (string_1 != old_string_1)
  {
    Display_String(old_string_1,0,0,1,0); // pass string, x, y, size, 0=clear
    Display_String(string_1,0,0,1,1); // pass string, x, y, size, 1=white
    old_string_1=string_1;
  }
//---------------------------------------------------------------------
//---------------------------------------------------------------------
  if (string_2 != old_string_2)
  {
    Display_String(old_string_2,44,0,1,0); // pass string, x, y, size, 0=clear
    Display_String(string_2,44,0,1,1); // pass string, x, y, size, 1=white
    old_string_2=string_2;
  }
//---------------------------------------------------------------------
//---------------------------------------------------------------------
  if (string_3 != old_string_3)
  {
    Display_String(old_string_3,88,0,1,0); // pass string, x, y, size, 0=clear
    Display_String(string_3,88,0,1,1); // pass string, x, y, size, 1=white
    old_string_3=string_3;
  }
//---------------------------------------------------------------------

if (outputNow.ok_togo()) {
  Serial.print("Output Now "); 
  Serial.println(millis()); 
  sliderValue1=75;
  dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 1023);
  notifyClients(getSliderValues());
}
  ws.cleanupClients();
}