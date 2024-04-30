#pragma once
#include <WiFiUdp.h>

#ifndef DATAPACK_SIZE
#define DATAPACK_SIZE
#define MAX_BYTES_UDP 1460
#endif

// states
const int NO_CONNECTION = 0;
const int DATA_REQUEST = 1;
const int SENDING_DATA = 2;
const int RESETING = 3;
const int STAND_BY = 4;

// Ping signals
const int NO_PING = 0;
const int HANDSHAKE = 1;
const int SEND_DATA_REQUEST = 2;
const int RECEIVING_DATA = 3;
const int RESET = 4;
// ===== =====

class Zinu {
    public:
        int state;
        bool connected = false;

        Zinu(int port);
        bool handShake();
        byte checkIncomingSignal();
        void sendData(uint8_t* bufferData, size_t bufferSize);
        void sendSignal(byte signal);
        
    private:
        WiFiUDP socket;
        int last_package_bytes;
        int num_packages;
        byte* signalbuffer = new byte[1];
        void countPackets(size_t numBytes);
        void sendResponseSignal(byte signal);
        void readIncomingSignal();
};
