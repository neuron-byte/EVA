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
byte Zinu::checkIncomingSignal() {
    byte *incommingSignal = this->readIncomingSignal(); 
    switch (*incommingSignal) {
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
void Zinu::sendResponseData(uint8_t* bufferData)
{   
    // int base = 0;
    // int head = MAX_BYTES_UDP-1;
    // for(int packageNumber; packageNumber< this->num_packages;packageNumber++){
    //     base += MAX_BYTES_UDP;
    //     head += MAX_BYTES_UDP;
    //     uint8_t localbuffer[1460];        
    //     byte *signal = this->readSignal();
    //     if(*signal == 1){
    //         this->sendResponseSignal(-2); //received a wrong sinal : dataResponse error
    //         break;
    //     }

    //     if(*signal == 3){
    //         //RESET
    //     }
    // }
}

bool Zinu::handShake()
{ 
    byte *incommingSignal = this->readIncomingSignal();
    if (*incommingSignal == HANDSHAKE) {
        this->sendResponseSignal(HANDSHAKE); // successful connection
        this->connected = true;
        return true;
    }
    return false;
}

void Zinu::countPackets(size_t numBytes)
{
    // byte remainder = numBytes % MAX_BYTES_UDP;
    // if (remainder == 0)
    // {
    //     num_packages = 1;
    //     return;
    // };
    // num_packages = (numBytes / MAX_BYTES_UDP) + remainder;
}

byte* Zinu::readIncomingSignal()
{
    // Em algum momento isso aqui vai dar memory leak
    // Porque?
    byte* buffer = new byte[1];
    buffer[0] = -1;
    this->socket.read(buffer, sizeof(buffer));
    Serial.println(*buffer);
    return buffer;
}   

