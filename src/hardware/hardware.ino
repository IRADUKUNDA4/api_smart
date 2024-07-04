#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

Servo myservo;

// rfid ppinout
#define SS_PIN D8   // The ESP8266 pin D8
#define RST_PIN D3  // The ESP8266 pin D2
#define RED D0
// #define GREEN D4
bool taped = false;

// connecting to wifi
ESP8266WiFiMulti WiFiMulti;


MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows

// Define a variable to store the card UID
byte cardUID[7];
char previousCard[32];

int wallet = 200;

// Define the specific UUID for comparison
byte specificUUID[4] = { 0x1D, 0x0C, 0x09, 0x6C };
byte cleaner[4] = { 0xC1, 0x56, 0xF3, 0x24 };

void setup() {
  Serial.begin(115200);
  myservo.attach(D4);
  myservo.write(0);
  delay(2000);
  // connecting to wifi
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  // WiFiMulti.addAP("UR-CST");
  //WiFiMulti.addAP("Joseph", "Jeph12345.");
  WiFiMulti.addAP("Dene", "Denyse88");
  // WiFiMulti.addAP("Bertin", "01234567");
  // WiFiMulti.addAP("Irembo WIFI", "WIFI@Irembo!");

  // nfc card reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scan a MIFARE Classic card");
  //  myservo.attach(servoPin);

  //i2c screen
  lcd.begin();  // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();
  pinMode(RED, OUTPUT);
  digitalWrite(RED, 0);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(" public toilets");
  lcd.setCursor(0, 1);
  lcd.print("managementsystem");

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Store the card UID in the variable
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID[i] = mfrc522.uid.uidByte[i];
  }

  // Clear the LCD
  lcd.clear();

  // Display the card UID on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Card:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    lcd.print(cardUID[i] < 0x10 ? "0" : "");
    lcd.print(cardUID[i], HEX);
  }
  delay(300);
  // Compare the card UID with the specific UUID
  if (memcmp(cardUID, specificUUID, sizeof(specificUUID)) == 0 && wallet > 0) {
    lcd.setCursor(0, 1);
    lcd.print("balance : ");
    lcd.print(wallet);

  } else if (memcmp(cardUID, cleaner, sizeof(cleaner)) == 0) {
    lcd.setCursor(0, 1);
    lcd.print("processing......");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("processing......");

    // Determine the length of the UID
    int uidLength = mfrc522.uid.size;

    // Ensure the buffer is large enough to hold the entire UID
    char cardUIDString[uidLength * 3 + 1];  // Each byte is represented by two characters plus null terminator

    // Convert the UID to a string
    sprintf(cardUIDString, "%02X", cardUID[0]);  // Start with the first byte
    for (int i = 1; i < uidLength; i++) {
      sprintf(&cardUIDString[strlen(cardUIDString)], "%02X", cardUID[i]);  // Append subsequent bytes
    }
    Serial.print("original Card: ");
    Serial.println(cardUIDString);
    getData(cardUIDString);
  }
}

void postData(char* prefix) {
  Serial.print(prefix);

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (prefix[strlen(prefix) - 1] != '\0') {
      prefix[strlen(prefix) - 1] = '\0';
    }
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Registering Card");
    lcd.setCursor(0, 1);
    lcd.print("Processing.....");

    // Concatenate the prefix with the rest of the URL
    char url[256];  // Adjust the size based on your expected URL length
    snprintf(url, sizeof(url), "http://192.168.43.134:4000/signup/?username=%s", prefix);

    if (http.begin(client, url)) {
      
      Serial.print("[HTTP] POST...\n");

      // Send the POST request
      int httpCode = http.POST("");

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          lcd.setCursor(0, 0);
          lcd.print("Registered");
          lcd.setCursor(0, 1);
          lcd.print("..................");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        // Serial.printf("[HTTP] POST... failed, error: %s\d", httpCode);
        lcd.print(http.errorToString(httpCode).c_str());
        return;
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }
}

void turnServo() {
  int pos;
  Serial.print("openinng");
  for (pos = 0; pos <= 180; pos += 1) {  // goes from 0 degrees to 180 degrees
    myservo.write(pos);                  // tell servo to go to position in variable 'pos'
    delay(15);                           // waits 15ms for the servo to reach the position
  }
  delay(3000);
  Serial.print("closing");
  for (pos = 180; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
    myservo.write(pos);                  // tell servo to go to position in variable 'pos'
    delay(15);                           // waits 15ms for the servo to reach the position
  }
  Serial.print("done");
}

void getData(char* prefix) {

  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    // if (prefix[strlen(prefix) - 1] != '\0') {
    //   prefix[strlen(prefix) - 1] = '\0';
    // }

    char url[256];  // Adjust the size based on your expected URL length
    snprintf(url, sizeof(url), "http://192.168.43.134:4000/user/?id=%s", prefix);
    if (http.begin(client, url)) {
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      Serial.printf("http code : ", httpCode);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        lcd.clear();
        JsonDocument doc;

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          deserializeJson(doc, payload);
          const char* username = doc["data"]["username"];
          const char* role = doc["data"]["role"];
          double wallet = doc["data"]["wallet"];
          if (strcmp(role, "admin") == 0) {
            Serial.print("he is an admin");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Admin Card");
            lcd.setCursor(0, 1);
            lcd.print(".Access allowed.");
            digitalWrite(RED, 0);
            digitalWrite(RED, 0);
            turnServo();
            digitalWrite(RED, 0);
            return;
          } else if (strcmp(role, "cleaner") == 0) {
            if (taped == true && strcmp(previousCard, prefix) != 0) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("The restroom is");
              lcd.setCursor(0, 1);
              lcd.print("    occupied");
              delay(1500);
            } else {
              Serial.print("he is a cleaner");
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Cleaner Card");
              lcd.setCursor(0, 1);
              lcd.print(".Access allowed.");
              digitalWrite(RED, 0);
              digitalWrite(RED, 0);
              turnServo();
              digitalWrite(RED, 0);
            }
            return;
          } else if (strcmp(role, "user") == 0) {
            if (wallet < 100 && taped == false) {
              lcd.setCursor(0, 0);
              lcd.print("low funds.....");
              lcd.setCursor(0, 1);
              std::string walletStr = std::to_string(wallet);
              lcd.print((std::string("balance : ") + walletStr).c_str());
              digitalWrite(RED, 1);
              delay(3000);
              digitalWrite(RED, 0);
              taped == false;
            } else if (taped == true && strcmp(previousCard, prefix) == 0) {
              taped = false;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print((std::string("card : ") + username).c_str());
              lcd.setCursor(0, 1);
              wallet = wallet - 50;
              std::string walletStr = std::to_string(wallet);
              lcd.print((std::string("balance : ") + walletStr).c_str());
              digitalWrite(RED, 0);
              digitalWrite(RED, 0);
              turnServo();
              transact(prefix);
              digitalWrite(RED, 0);
            } else if (taped == true && strcmp(previousCard, prefix) != 0) {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("The restroom is");
              lcd.setCursor(0, 1);
              lcd.print("    occupied");
              digitalWrite(RED, 1);
              delay(1500);
              digitalWrite(RED, 0);
            } else {
              taped = true;
              strcpy(previousCard, prefix);
              Serial.print("assigned previous Card: ");
              Serial.println(previousCard);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print((std::string("card : ") + username).c_str());
              lcd.setCursor(0, 1);
              wallet = wallet - 50;
              std::string walletStr = std::to_string(wallet);
              lcd.print((std::string("balance : ") + walletStr).c_str());
              digitalWrite(RED, 0);
              digitalWrite(RED, 0);
              turnServo();
              transact(prefix);
              digitalWrite(RED, 0);
            }
          }
          delay(4000);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }
}

void transact(char* prefix) {
  Serial.print(prefix);

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;

    HTTPClient http;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Transacting..!");
    lcd.setCursor(0, 1);
    lcd.print("Processing......");

    // Concatenate the prefix with the rest of the URL
    char url[256];  // Adjust the size based on your expected URL length
    snprintf(url, sizeof(url), "http://192.168.43.134:4000/transact/?id=%s&op=sub&amount=50", prefix);

    if (http.begin(client, url)) {
      Serial.print("[HTTP] PATCH...\n");

      // Send the PATCH request
      int httpCode = http.PATCH("");

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] PATCH... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          lcd.print("posted successfull");
        }
      } else {
        Serial.printf("[HTTP] PATCH... failed, error: %s\n", http.errorToString(httpCode).c_str());
        lcd.print(http.errorToString(httpCode).c_str());
        return;
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }
}