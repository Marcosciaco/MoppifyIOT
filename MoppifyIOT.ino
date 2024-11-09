#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

#define SS_PIN 12
#define RST_PIN 13

#define PIN_RGB 15
#define NUM_PIXELS 23

MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_RGB, NEO_GRB + NEO_KHZ800);

MFRC522::MIFARE_Key key; 

byte nuidPICC[4];
bool logged = false;

const char* ssid = "Sciaco router";
const char* password = "12345678";
const char* apiUrl = "http://192.168.117.152:5000/authenticate";


void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  NeoPixel.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  SPI.begin();
  rfid.PCD_Init();

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

}

void loop() {

  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }


  if ((rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3]) || !logged) {

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    
    printHex(rfid.uid.uidByte, rfid.uid.size, "login");
    logged = true;
    Serial.println();

  } else {
    printHex(rfid.uid.uidByte, rfid.uid.size, "logout");
    logged = false;
  }
  rfid.PICC_HaltA();

  rfid.PCD_StopCrypto1();
}

void printHex(byte *buffer, byte bufferSize, String action) {
  String hexString = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] < 0x10) hexString += "0";
    hexString += String(buffer[i], HEX);
  }
  sendActionToServer(hexString, action);
}

void sendActionToServer(String uid, String action) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("{\"rfid\":\"" + uid + "\", \"action\":\""+ action + "\", \"cart\":\"4e105d3d-4734-4858-addc-085fae4e0a70\"}");
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);

      const size_t capacity = JSON_OBJECT_SIZE(2) + 100;
      DynamicJsonDocument doc(capacity);

      DeserializationError error = deserializeJson(doc, response);
      if (!error) {
        const char* position = doc["position"];
        if (action.equals("login")) {
          if (String(position).equals("janitor")) {
            showGenitorLoggedSuccessfully();
          } else {
            showAdminLoggedSuccessfully();
          }
        } else {
          if (String(position).equals("janitor")) {
            showGenitorUnLogSuccessfully();
          } else {
            showAdminUnLogSuccessfully();
          }
        }
      } else {
        Serial.println(error.c_str());
      }
    } else {
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
  }
}

void showGenitorLoggedSuccessfully() {
  NeoPixel.setBrightness(100);

  NeoPixel.clear();

  for (int pixel = 0; pixel <= NUM_PIXELS/2; pixel++) {
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(0, 255, 0));
    NeoPixel.setPixelColor(NUM_PIXELS - pixel, NeoPixel.Color(0, 255, 0));

    NeoPixel.show();

    delay(100);
  }

  NeoPixel.show();
}

void showAdminLoggedSuccessfully() {
  NeoPixel.setBrightness(100);

  NeoPixel.clear();

  for (int pixel = 0; pixel <= NUM_PIXELS/2; pixel++) {
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(255, 0, 0));
    NeoPixel.setPixelColor(NUM_PIXELS - pixel, NeoPixel.Color(255, 0, 0));
    NeoPixel.show();

    delay(100);
  }

  NeoPixel.show();
}

void showGenitorUnLogSuccessfully() {
  NeoPixel.setBrightness(100);

  for (int i = 0; i <= NUM_PIXELS; i++) {
    NeoPixel.setPixelColor(i, NeoPixel.Color(0, 255, 0));
  }
  NeoPixel.show();
  delay(100);

  for (int i = 0; i <= NUM_PIXELS / 2; i++) {
    NeoPixel.setPixelColor((NUM_PIXELS / 2) - i - 1, NeoPixel.Color(0, 0, 0));
    NeoPixel.setPixelColor((NUM_PIXELS / 2) + i, NeoPixel.Color(0, 0, 0));

    NeoPixel.show();
    delay(100);
  }
}

void showAdminUnLogSuccessfully() {
  NeoPixel.setBrightness(100);

  for (int i = 0; i <= NUM_PIXELS; i++) {
    NeoPixel.setPixelColor(i, NeoPixel.Color(255, 0, 0));
  }
  NeoPixel.show();
  delay(100);

  for (int i = 0; i <= NUM_PIXELS / 2; i++) {
    NeoPixel.setPixelColor((NUM_PIXELS / 2) - i - 1, NeoPixel.Color(0, 0, 0));
    NeoPixel.setPixelColor((NUM_PIXELS / 2) + i, NeoPixel.Color(0, 0, 0));

    NeoPixel.show();
    delay(100);
  }
}
