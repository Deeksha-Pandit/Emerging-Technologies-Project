#include <M5StickCPlus.h>
#include "BLEDevice.h"
#include "esp32-hal-ledc.h"
#include <BLEUtils.h>
#include <BLEServer.h>
#include <SPIFFS.h>
String file_name =
    "/M5Stack/notes.txt";  // Sets the location and name of the file to be
                           // operated on.  
bool SPIFFS_FORMAT =
    false;  // Whether to initialize the SPIFFS.
#define COUNT_LOW   1500
#define COUNT_HIGH  9000
#define TIMER_WIDTH 16

// The remote service we wish to create
#define CHARACTERISTIC_UUID "a370d780-3c47-495a-9ff2-248a822c933b"//add here
// The characteristic of the remote service we are creating.
#define SERVICE_UUID "56aadc63-0354-4b7c-bb8b-ee2ba0f6b3d1"//add here

// The remote service we wish to connect to.
static BLEUUID serviceUUID("735f7fee-897c-4ba5-94d6-193ac1acf60a");//add here
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("2a43fe13-b7c2-4e79-ba44-019a210b0da5");//add here

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLECharacteristic *pCharacteristic;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static boolean doConnect_1 = false;
static boolean connected_1 = false;
static boolean doScan_1 = false;
static BLECharacteristic *pCharacteristic_1;
static BLERemoteCharacteristic* pRemoteCharacteristic_1;
static BLEAdvertisedDevice* myDevice_1;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    M5.Lcd.print("Notify callback for characteristic ");
    M5.Lcd.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    M5.Lcd.print(" of data length ");
    M5.Lcd.print(length);
    M5.Lcd.print("data: ");
    M5.Lcd.print((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    M5.Lcd.print("onDisconnect");
  }
};

bool connectToServer() {
    M5.Lcd.print("Forming a connection to ");
    M5.Lcd.print(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    M5.Lcd.print(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    M5.Lcd.print(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      M5.Lcd.print("Failed to find our service UUID: ");
      M5.Lcd.print(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    M5.Lcd.print(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      M5.Lcd.print("Failed to find our characteristic UUID: ");
      M5.Lcd.print(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    M5.Lcd.print(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      M5.Lcd.print("The characteristic value was: ");
      M5.Lcd.print(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      M5.Lcd.print("BLE Advertised Device found: ");
      M5.Lcd.print(advertisedDevice.toString().c_str());

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }
  }
}; // MyAdvertisedDeviceCallbacks

void setup() {
  M5.begin();
  BLEDevice::init("Servo_M5");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic_1 = pService->createCharacteristic(
                                          CHARACTERISTIC_UUID,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE
                                        );

  pCharacteristic_1->setValue("Hello World says Servo");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  M5.Lcd.print("Characteristic defined! Now you can read it in your phone!");

  ledcSetup(1, 50, TIMER_WIDTH);
  ledcAttachPin(26, 1);
}

// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    M5.Lcd.print(doConnect);
    if (connectToServer()) {
      M5.Lcd.print("We are now connected to the BLE Server.");
    } else {
      M5.Lcd.print("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setCursor(25, 80, 4);
      if (SPIFFS_FORMAT) {
        M5.Lcd.println(
            "SPIFFS format start...");  // output format String in LCD.
        SPIFFS.format();                // Formatting SPIFFS.  
        M5.Lcd.println("SPIFFS format finish");
    }

    if (SPIFFS.begin()) {  // Start SPIFFS, return 1 on success.
                           
        M5.Lcd.println("SPIFFS Begin.");
        // Write operation
        File dataFile = SPIFFS.open(
            file_name,
            "w");  // Create a File object dafa  File to write information to
                   // file_name in the SPIFFS.
        dataFile.println("Hello IOT World.");  // Writes string information and
                                               // newlines to the dataFile.
        dataFile.close();  // Close the file when writing is complete.
        M5.Lcd.println("Finished Writing data to SPIFFS");
    } else {
        M5.Lcd.println(
            "SPIFFS Failed to Begin.\nYou need to Run SPIFFS_Add.ino first");
    }
     M5.update();  // Check whether the key is pressed. 
    if (M5.BtnA.isPressed()) {  // If the button is pressed. 
        if (SPIFFS.exists(
                file_name)) {  // Check whether the file_name file exists in the
                               // flash memory.  
            M5.Lcd.println("FOUND.");
            M5.Lcd.println(file_name);
        } else {
            M5.Lcd.println("NOT FOUND.");
            M5.Lcd.println(file_name);
        }
        File dataFile = SPIFFS.open(
            file_name,
            "r");  // Create aFile object dafaFile to read information to
                   // file_name in the SPIFFS.
        for (int i = 0; i < dataFile.size();
             i++) {  // Reads file contents and outputs file information through
                     // the M5.Lcd port monitor.
            M5.Lcd.print((char)dataFile.read());
        }
        dataFile.close();  // Close the file after reading the file.
        delay(200);
    }
    }
  }else if(doScan){
    BLEDevice::getScan()->start(0);
  }
} // End of loop