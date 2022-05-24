#ifndef CONTROLLER_BLUETOOTH_H
#define CONTROLLER_BLUETOOTH_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


namespace Controller {

    class CharacteristicCallback : public BLECharacteristicCallbacks {
    private:
        friend class Bluetooth;

        CharacteristicCallback(void (*)(const std::string &));

        void onRead(BLECharacteristic *characteristic);

        void (*onReceive)(const std::string &);
    };

    class Bluetooth {
    public:
        Bluetooth(const std::string &, void (*)(const std::string &));

        ~Bluetooth();

        void write(const std::string &);

        bool isConnected();

    private:
        BLEServer *server;
        BLEService *service;
        BLECharacteristic *characteristic;
    };
}

#endif
