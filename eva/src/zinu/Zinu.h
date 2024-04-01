#include <WiFiUdp.h>

// states
const int WAITING = 0;
const int SENDING_DATA = 1;
const int RESETING = 2;

// Ping signals
const int HANDSHAKE = 0;
const int SEND_DATA_REQUEST = 1;
const int RESET = 2;

class Zinu {
    public:
        Zinu(int port);
        bool handShake();
        byte checkIncomingSignal();
        uint8_t bufferData;
        size_t bufferSize;
        int state;
        bool connected = false;

    private:
        WiFiUDP socket;
        int num_packages;
        int last_package_bytes;
        byte* signalbuffer = new byte[1];
        void sendResponseSignal(byte signal);
        void sendResponseData(uint8_t* bufferData);
        void countPackets(size_t numBytes);
        byte* readIncomingSignal();
};
