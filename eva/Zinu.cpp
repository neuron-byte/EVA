#include "Zinu.h"
#include "wifi_conf.h"
#include <WiFiUdp.h>

Zinu::Zinu()
{
    while (!socket.begin(UDP_PORT))
    {
        Serial.println("Erro ao iniciar servidor UDP");
        Serial.println("Tentando iniciar servidor UDP...");
        delay(50);
    }
}


//8bits like
byte Zinu::listener(uint8_t* bufferData, size_t bufferSize)
{
    byte *signal = this->readSignal();
    switch (*signal)
    {
    case 1:
        this->countingPackages(bufferSize);
        this->signalResponse(this->num_packages);
        break;
    case 2:
        this->dataResponse_8bits(bufferData);
        break;
    case 3:
        //reset
        break;
    default:
        //erro
        break;
    }

    return *signal;
}


void Zinu::signalResponse(byte signal)
{
    this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
    this->socket.write(&signal, sizeof(signal));
    this->socket.endPacket();
}

//8bits like
void Zinu::dataResponse_8bits(uint8_t* bufferData)
{   
    int base = 0;
    int head = MAX_BYTES_UDP-1;
    for(int packageNumber; packageNumber< this->num_packages;packageNumber++){
        base += MAX_BYTES_UDP;
        head += MAX_BYTES_UDP;
        uint8_t localbuffer[1460];        
        byte *signal = this->readSignal();
        if(*signal == 1){
            this->signalResponse(-2); //received a wrong sinal : dataResponse error
            break;
        }

        if(*signal == 3){
            //RESET
        }
    }
}

byte Zinu::handShake()
{
    byte *signal = this->readSignal();
    if (*signal == 0)
    {
        this->signalResponse(1); // successful connection
        return 1;
    }

    this->signalResponse(-1); // received a wrong signal : handshake error
    return 0;
}

void Zinu::countingPackages(size_t numBytes)
{
    byte remainder = numBytes % MAX_BYTES_UDP;
    if (remainder == 0)
    {
        num_packages = 1;
        return;
    };
    num_packages = (numBytes / MAX_BYTES_UDP) + remainder;
}

byte* Zinu::readSignal()
{
    if (this->socket.parsePacket())
    {
        byte* buffer = new byte[1];
        this->socket.read(buffer, sizeof(buffer));
        return buffer;
    }
}   


