#define COUNT_LOW   1500
#define COUNT_HIGH  8500
#define TIMER_WIDTH 16
#include "esp32-hal-ledc.h"
#include <M5StickCPlus.h>
#include "BLEDevice.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("a370d780-3c47-495a-9ff2-248a822c933b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("56aadc63-0354-4b7c-bb8b-ee2ba0f6b3d1");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLECharacteristic *pCharacteristic;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

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
    delay(10000);
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
    // put your setup code here, to run once:
    M5.begin();
    BLEDevice::init("");

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 5 seconds.
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    M5.Lcd.print(doConnect);
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
    ledcSetup(1, 50, TIMER_WIDTH);
    ledcAttachPin(26, 1);
    M5.Lcd.setCursor(25, 80, 4);
    M5.Lcd.print("SERVO");
}

void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    M5.Lcd.print(doConnect);
    M5.Lcd.print("Inside loop if");
    if (connectToServer()) {
      M5.Lcd.print("We are now connected to the BLE Server.");
    } else {
      M5.Lcd.print("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      M5.Lcd.print("The characteristic value was: ");
      M5.Lcd.print(value.c_str());
      if(value =="HOT"){
        // put your main code here, to run repeatedly:
        for (int i = COUNT_LOW; i < COUNT_HIGH; i = i + 100) {
          ledcWrite(1, i);
          delay(50);
        }
      }
      else {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(25, 80, 4);
        M5.Lcd.print("Turning Fan Off");
      }
    }
    String newValue = "Time since boot: " + String(millis()/1000);
    M5.Lcd.print("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
}
