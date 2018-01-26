#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>



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
      digitalWrite(5, LOW); 
      digitalWrite(2,HIGH);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(2,LOW); 
      digitalWrite(5,HIGH);
    }
};

//void BLEScaning(void);

void BLEServerSetup(void){

    Serial.println("\n1- Downloadaj i instaliraj BLE scanner  aplikaciju na mobitel");
    Serial.println("2- Traži BLE uređaje u aplikaciji");
    Serial.println("3- Spoji se na MyESP32");
    Serial.println("4- Idi na CUSTOM CHARACTERISTIC u CUSTOM SERVICE , odaberi 'w' i napisi nesto");
  

    BLEDevice::init("MyESP32");

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
   pCharacteristic->setValue("Hello World");
  // Zapocni uslugu
  pService->start();

  // Zapocni odasiljanje
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void setup() {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);

  BLEServerSetup();
}

void loop() {
 
}
