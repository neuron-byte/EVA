#include "Zinu.h"

Zinu::Zinu(int port)
{
    while (!socket.begin(port))
    {
        Serial.println("Erro ao iniciar servidor UDP");
        Serial.println("Tentando iniciar servidor UDP...");
        delay(50);
    }
}

//8bits like
void Zinu::checkIncomingSignal() {
    this->readIncomingSignal();
    switch (*this->signalbuffer) {
        case SEND_DATA_REQUEST:
            this->state = SENDING_DATA;
            break;
        case RESET:
            this->state = RESETING;
            break;
    }
}

void Zinu::sendResponseSignal(byte signal)
{
    this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
    this->socket.write(&signal, sizeof(signal));
    this->socket.endPacket();
}

//8bits like
void Zinu::sendData(uint8_t* bufferData)
{   
    // int base = 0;
    // int head = MAX_BYTES_UDP-1;
    // int packageCounter = 1;
    // byte* signal;
    
    // do{
    //     base+= MAX_BYTES_UDP;
    //     head+= MAX_BYTES_UDP;
    //     int bufferSize = packageCounter==this->num_packages ? this->last_package_bytes : MAX_BYTES_UDP;
    //     uint8_t localbuffer[bufferSize];
    //     this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
    //     this->socket.write(localbuffer, sizeof(localbuffer));
    //     this->socket.endPacket();
    //     packageCounter++;
    //     signal = this->readSignal();
    //     Serial.print("Signal: ");
    //     Serial.println(*signal);
    // }while(*signal == 2 && (packageCounter<=this->num_packages));
    // switch (*signal)
    // {
    // case 3:
    //     Serial.println("Reseting");
    //     this->signalResponse(1);
    //     break;
    // case 2:
    //     Serial.println("Sended!");
    //     this->signalResponse(1);
    //     break;
    // default:
    //     Serial.println("Wrong Signal");
    //     this->signalResponse(0);
    //     break;
    // }
    // this->num_packages = 0;
}

bool Zinu::handShake()
{ 
    this->readIncomingSignal();
    if (this->signalbuffer[0] == HANDSHAKE) {
        this->sendResponseSignal(HANDSHAKE); // successful connection
        this->connected = true;
        return this->connected;
    }
    return false;
}

void Zinu::countPackets(size_t numBytes) {
    Serial.printf("%i\n", numBytes);
    if (numBytes % MAX_DATA_SIZE == 0 || (numBytes/MAX_DATA_SIZE) < 1)
    {
        this->num_packages = 1;
        this->last_package_bytes = 0;
        return;
    };
    this->num_packages = (numBytes / MAX_DATA_SIZE) + 1;
    this->last_package_bytes = numBytes % MAX_DATA_SIZE;
    Serial.printf("%i\n", num_packages);
}

void Zinu::readIncomingSignal() {
    this->signalbuffer[0] = 0;
    if (this->socket.parsePacket()) {
        this->socket.read(this->signalbuffer, sizeof(this->signalbuffer));
    }
} 