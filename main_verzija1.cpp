#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <WiFi.h>

const char* ssid     = "Danijel"; //vlastiti ssid
const char* password = "12345678";//lozinka
WiFiServer server(80);


BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
BLEServer *pServer;
BLEScan* pBLEScan;
int scanTime = 15;
BLEScanResults foundDevices;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "8dd905df-e461-47a4-b69c-44e8f3f1ce1f"
#define CHARACTERISTIC_UUID "0e6b976b-7ae6-49fd-9d36-f12f5cf9520f"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("Nova vrijednost: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void WiFiLocalWebPageCtrl(void);
void connectWiFi(void);

void BLEScaning(void){
  
  pBLEScan->setActiveScan(true); 
  foundDevices = pBLEScan->start(scanTime);
  Serial.print("Broj uređaja: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Skeniranje završeno!");
}

void BLEServerSetup(void){

    Serial.println("1- Download and install an BLE scanner app in your phone");
    Serial.println("2- Scan for BLE devices in the app");
    Serial.println("3- Connect to MyESP32");
    Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
    Serial.println("5- See the magic =)");


    // Stvori BLE posluzitelj
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Stvori BLE uslugu
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Kreiraj BLE karakteristike
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  
                        );

   pCharacteristic->setCallbacks(new MyCallbacks());
  // Zapocni uslugu
  pService->start();

  // Zapocni odasiljanje
  pServer->getAdvertising()->start();
  Serial.println("Spreman za spajanje :)");
}

void setup() {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(5,OUTPUT);


  // Stvori BLE uredaj
  BLEDevice::init("MyESP32");
  //pBLEScan = BLEDevice::getScan(); 
  BLEServerSetup();
  //connectWiFi();
}

void loop() {

 // kad je bluetooth uredaj spojen upali LED na pinu 2 i ugasi LED na vratima 5
  if(deviceConnected){ 
    digitalWrite(5, LOW); 
    digitalWrite(2,HIGH);
  }
  else {digitalWrite(2,LOW); digitalWrite(5,HIGH);}
  delay(2000);
  //WiFiLocalWebPageCtrl();
}



void WiFiLocalWebPageCtrl(void)
{
  WiFiClient client = server.available();  
  
  if (client) {                            
    Serial.println("Novi klijent.");           
    String currentLine = "";                
    while (client.connected()) {           
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    

          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            //Skeniranje broja bluetooth uređaja
            BLEScaning();
            client.print("Broj bluetooth uredaja: ");
            client.println(foundDevices.getCount());
            client.println();            
            
            break;
          } else {    
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    client.stop();
    Serial.println("Kljent odspojen.");
  }
}

void connectWiFi(void)
{
  Serial.println();
  Serial.println();
  Serial.print("Spajanje na: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi spojen.");
  Serial.println("IP adresa: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
}