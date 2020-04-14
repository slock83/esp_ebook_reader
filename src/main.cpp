#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266FtpServer.h>


Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

std::string test_str = "This is a test string to confirm that fast-reading quite works.";
const char* ssid = "SFR_3710";
const char* password = "bvs59zqkt2ztb27stbg7";


FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

File currentFile;

File loadFile() {
  File file = SPIFFS.open("/test.txt", "r");
  return file;
}

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("Connecting to SSID ");
    display.print(ssid);
    display.display();
    delay(500);
    Serial.print(".");
    
  }
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("WIFI Ok !");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  if (SPIFFS.begin()) {
    display.clearDisplay();
    ftpSrv.begin("esp8266","esp8266");
    display.print("SPIFFS OK !");
    display.println(WiFi.localIP());
    display.setCursor(0,0);
    display.display(); // actually display all of the above
    currentFile = loadFile();
  }
}


uint lastDisplay = 0;
uint msDelay = 250;
String currentWord;
void read_mode() {
  if ((millis() - lastDisplay) > msDelay) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    
    if(currentFile.available()) {
      currentWord = currentFile.readStringUntil(' ');
      currentWord.remove('\n');
      currentWord.remove('\r');
      currentWord.remove(' ');
      display.print(currentWord);
    } else {
      display.print("Current file over. Looping ...");      
      currentFile.close();
      currentFile = loadFile();
    }
    display.display();
    lastDisplay = millis();
  }
}


enum STATE {
  STATE_READING,
  STATE_MENU
};

STATE currentState = STATE_READING;
void loop() {
  yield();
  switch (currentState)
  {
  case STATE_READING:
    read_mode();
    break;
  
  default:
    break;
  }
  delay(250);
}