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
        Serial.println("Contando pacotes e mandando");
        this->countingPackages(bufferSize);
        this->signalResponse(this->num_packages);
        break;
    case 2:
        Serial.println("Enviando pacotes");
        this->dataResponse_8bits(bufferData);
        Serial.println("Enviou tudo e recomeçando loop");
        break;
    case 3:
        Serial.println("Resetando");
        break;
    default:
        
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
    int packageCounter = 1;
    byte* signal;
    
    do{
        base+= MAX_BYTES_UDP;
        head+= MAX_BYTES_UDP;
        int bufferSize = packageCounter==this->num_packages ? this->last_package_bytes : MAX_BYTES_UDP;
        uint8_t localbuffer[bufferSize];
        this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
        this->socket.write(localbuffer, sizeof(localbuffer));
        this->socket.endPacket();
        packageCounter++;
        signal = this->readSignal();
        Serial.print("Signal: ");
        Serial.println(*signal);
    }while(*signal == 2 && (packageCounter<=this->num_packages));
    switch (*signal)
    {
    case 3:
        Serial.println("Reseting");
        this->signalResponse(1);
        break;
    case 2:
        Serial.println("Sended!");
        this->signalResponse(1);
        break;
    default:
        Serial.println("Wrong Signal");
        this->signalResponse(0);
        break;
    }
    this->num_packages = 0;
}


byte Zinu::handShake()
{
    byte *signal = this->readSignal();
    if (*signal == 0)
    {
        this->signalResponse(1); // successful connection
        return 1;
    }

    if(*signal == 4)
    {
       return 0; 
    }

    this->signalResponse(-1); // received a wrong signal : handshake error
    return 0;
}

void Zinu::countingPackages(size_t numBytes)
{   
    Serial.printf("%i\n", numBytes);
    if (numBytes % MAX_BYTES_UDP == 0 || (numBytes/MAX_BYTES_UDP) < 1)
    {
        this->num_packages = 1;
        this->last_package_bytes = 0;
        return;
    };
    this->num_packages = (numBytes / MAX_BYTES_UDP) + 1;
    this->last_package_bytes = numBytes%MAX_BYTES_UDP;
    Serial.printf("%i\n", num_packages);
}

byte* Zinu::readSignal()
{
    byte* buffer = new byte[1];
    while (true)
    {
        if(this->socket.parsePacket()>0)
        {
            this->socket.read(buffer, sizeof(buffer));   
            break;
        }
        delay(50);
    }
    return buffer;
}   
