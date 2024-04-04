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
        case HANDSHAKE:
            if (this->connected) this->sendResponseSignal(1);
            break;
        case SEND_DATA_REQUEST:
            this->state = DATA_REQUEST;
            break;
        case RESET:
            this->state = RESETING;
            break;
        default:
            break;
    }
}

void Zinu::sendResponseSignal(byte signal)
{
    this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
    this->socket.write(&signal, sizeof(signal));
    this->socket.endPacket();
}

void Zinu::sendData(uint8_t* bufferData, size_t bufferSize) {
    // Conta e enviar pacotes
    this->countPackets(bufferSize);
    this->sendResponseSignal(this->num_packages);
    // ---

    int base = 0;
    int packetCounter = 0;
    int local_buffer_size;
    bool cond1;
    bool cond2; 
    bool cond3;

    do {
        Serial.println("\nIniciando envio");
        local_buffer_size = packetCounter == this->num_packages ? this->last_package_bytes : MAX_BYTES_UDP;
        Serial.print("Tamanho do buffer local: ");
        Serial.println(local_buffer_size);
        uint8_t localbuffer[local_buffer_size];
        // Loading buffer
        for (size_t i = 0; i < local_buffer_size; i++) {
            localbuffer[i] = bufferData[base + i];
        }
        base += MAX_BYTES_UDP;
        Serial.print("base: ");
        Serial.println(base);
        Serial.print("Contador de pacotes: ");
        Serial.println(packetCounter);

        // Aguardando sinal de "RECEIVING_DATA"
        do {
            this->readIncomingSignal();
        } while (*this->signalbuffer == NO_PING);

        this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
        this->socket.write(localbuffer, sizeof(localbuffer));
        this->socket.endPacket();
        packetCounter++;
        Serial.println("Um pacote enviado");
        
        Serial.print("Sinal do buffer: ");
        Serial.println(*this->signalbuffer);
        cond1 = *this->signalbuffer == RECEIVING_DATA;
        cond2 = *this->signalbuffer == SEND_DATA_REQUEST;
        cond3 = packetCounter < this->num_packages;
        Serial.print("Condição 1: ");
        Serial.println(cond1);
        Serial.print("Condição 2: ");
        Serial.println(cond2);
        Serial.print("Condição 3: ");
        Serial.println(cond3);

    } while (cond1 && cond3);
    Serial.println("Enviado. Zerando num_pacotes");
    this->num_packages = 0;
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
    if (numBytes % MAX_BYTES_UDP == 0 || (numBytes/MAX_BYTES_UDP) < 1)
    {
        this->num_packages = 1;
        this->last_package_bytes = 0;
        return;
    };
    this->num_packages = (numBytes / MAX_BYTES_UDP) + 1;
    this->last_package_bytes = numBytes % MAX_BYTES_UDP;
}

void Zinu::readIncomingSignal() {
    this->signalbuffer[0] = NO_PING;
    if (this->socket.parsePacket() > 0) {
        this->socket.read(this->signalbuffer, sizeof(this->signalbuffer));
        Serial.print("Atualizando ping para: ");
        Serial.println(*this->signalbuffer);
    }
} 