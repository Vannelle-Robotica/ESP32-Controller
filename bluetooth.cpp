#include "bluetooth.h"

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"


namespace Controller {


    void ServerCallback::onConnect(BLEServer *) {
        this->connected = true;
    }


    void ServerCallback::onDisconnect(BLEServer *server) {
        server->startAdvertising();
        this->connected = false;
    }


    CharacteristicCallback::CharacteristicCallback(void (*onReceive)(const std::string &))
            : onReceive(onReceive) {
    }


    void CharacteristicCallback::onWrite(BLECharacteristic *characteristic) {
        std::string value = characteristic->getValue();
        onReceive(value);
    }


    Bluetooth::Bluetooth(const std::string &name, void (*onReceive)(const std::string &))
            : serverCallback(new ServerCallback()) {
        // Initialize BLE device
        BLEDevice::init(name);

        // Initialize BLE server & service
        server = BLEDevice::createServer();
        server->setCallbacks(serverCallback);
        service = server->createService(SERVICE_UUID);

        // Create characteristic for reading & writing
        characteristic = service->createCharacteristic(
                CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_NOTIFY |
                BLECharacteristic::PROPERTY_READ |
                BLECharacteristic::PROPERTY_WRITE
        );

        // Add BLE2902 descriptor and start service
        characteristic->addDescriptor(new BLE2902());
        characteristic->setCallbacks(new CharacteristicCallback(onReceive));
        service->start();

        // Start advertising bluetooth server
        server->startAdvertising();
    }


    Bluetooth::~Bluetooth() {
        server->removeService(service);

        // Deallocate bluetooth server
        delete characteristic;
        delete service;
        delete server;
    }


    void Bluetooth::write(const std::string &msg) {
        characteristic->setValue(msg);
        characteristic->notify();
    }


    bool Bluetooth::isConnected() {
        return serverCallback->connected;
    }
}
