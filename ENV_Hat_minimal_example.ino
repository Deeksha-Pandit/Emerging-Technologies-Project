#include <M5StickCPlus.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// The remote service we wish to connect to.
#define CHARACTERISTIC_UUID "a370d780-3c47-495a-9ff2-248a822c933b"
// The characteristic of the remote service we are interested in.
#define SERVICE_UUID "56aadc63-0354-4b7c-bb8b-ee2ba0f6b3d1"

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLECharacteristic *pCharacteristic;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

float cTemp    = 0;
float fTemp    = 0;
float humidity = 0;

void setup() {
  M5.begin();
  Wire.begin(0,26);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 4);
  //M5.Lcd.print("Starting BLE work!");

  BLEDevice::init("Proximity_M5");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );

  pCharacteristic->setValue("Hello World says Temp");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
 // M5.Lcd.print("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  unsigned int data[6];

  // Start I2C Transmission
  Wire.beginTransmission(0x44);
  // Send measurement command
  Wire.write(0x2C);
  Wire.write(0x06);
  // Stop I2C transmission
  if (Wire.endTransmission() != 0){
    Serial.println("ERROR -- endTransmission Error!");
  }
  else{
    delay(500);
    // Request 6 bytes of data
    Wire.requestFrom(0x44, 6);

    // Read 6 bytes of data
    // cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
    for (int i = 0; i < 6; i++) {
      data[i] = Wire.read();
    };
    delay(50);

    if (Wire.available() != 0){
      Serial.println("ERROR -- Wire.available still has data!");
    }
    else{
      // Convert the data
      cTemp    = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
      fTemp    = (cTemp * 1.8) + 32;
      humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
      Serial.print("celsius: ");
      Serial.println(cTemp);
      M5.Lcd.setCursor(0, 30, 2);
      M5.Lcd.printf("Temp: %2.1f Humi: %2.0f", cTemp, humidity);
      if(cTemp>20){
        pCharacteristic->setValue("HOT");        
      }
      else{
        pCharacteristic->setValue("COOL");
      }
    }
  delay(2000);
  }
}