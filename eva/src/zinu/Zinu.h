#include <WiFiUdp.h>

#ifndef DATA_SIZE
#define DATA_SIZE
#define MAX_BYTES_UDP 1460
#define MAX_DATA_SIZE 1455
#endif

// states
const int NO_CONNECTION = 0;
const int SENDING_DATA = 1;
const int RESETING = 2;
const int STAND_BY = 3;

// Ping signals
const int HANDSHAKE = 1;
const int SEND_DATA_REQUEST = 2;
const int RESET = 3;

class Zinu {
    public:
        Zinu(int port);
        bool handShake();
        void checkIncomingSignal();
        void sendData(uint8_t* bufferData);
        int state;
        bool connected = false;

    private:
        WiFiUDP socket;
        int last_package_bytes;
        int num_packages
        void countPackets(size_t numBytes);
        byte* signalbuffer = new byte[1];
        void sendResponseSignal(byte signal);
        void readIncomingSignal();
};



typedef struct {
    uint8_t signal;
    uint32_t packet_id;
} PacketHeader;

typedef struct {
    PacketHeader header;
    byte *data;
} Packet;
