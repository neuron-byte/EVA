#include "Zinu.h"
#include "../../debug.h"

Zinu::Zinu(int port) {
    this->signalbuffer[0] = NO_PING;
    while (!socket.begin(port)) {
        infoln("Erro ao iniciar servidor UDP");
        infoln("Tentando iniciar servidor UDP...");
        delay(50);
    }
} 

//8bits like
byte Zinu::checkIncomingSignal() {
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
    }
    return *this->signalbuffer;
}

void Zinu::sendSignal(byte signal) {
    this->sendResponseSignal(signal);
}

void Zinu::sendResponseSignal(byte signal) {
    this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
    this->socket.write(&signal, sizeof(signal));
    this->socket.endPacket();
}

void Zinu::sendData(uint8_t* bufferData, size_t bufferSize) {
    debugln("sendData");
    // Conta e enviar pacotes
    this->countPackets(bufferSize);
    this->sendResponseSignal(this->num_packages);
    // ---

    debugf("Quantidade de pacotes: %d\n", this->num_packages);
    debugf("Tamanho do dado: %d\n", bufferSize);

    int base = 0;
    int packetCounter = 1;
    int local_buffer_size;
    do {
        debugf("packetCounter: %d\n", packetCounter);
        local_buffer_size = packetCounter == this->num_packages ? this->last_package_bytes : MAX_BYTES_UDP;
        debugf("local_buffer_size: %d\n", local_buffer_size);
        uint8_t localbuffer[local_buffer_size];
        // Loading buffer
        for (size_t i = 0; i < local_buffer_size; i++) {
            localbuffer[i] = bufferData[base + i];
        }
        base += MAX_BYTES_UDP;

        // Aguardando sinal de "RECEIVING_DATA"
        do {
            this->readIncomingSignal();
            debugln("Aguardando sinal de prontidão.");
        } while (*this->signalbuffer == NO_PING);

        this->socket.beginPacket(this->socket.remoteIP(), this->socket.remotePort());
        this->socket.write(localbuffer, sizeof(localbuffer));
        this->socket.endPacket();
        packetCounter++;
    } while (*this->signalbuffer == RECEIVING_DATA && packetCounter <= this->num_packages);
    debugln("Dado enviado. Zerando num_pacotes");
    this->num_packages = 0;
    debugln("-------------------");
}

bool Zinu::handShake() { 
    debugln("handShake");
    this->readIncomingSignal();
    if (this->signalbuffer[0] == HANDSHAKE) {
        this->sendResponseSignal(HANDSHAKE); // successful connection
        this->connected = true;
        debugln("Realizado com sucesso.");
        debugln("-------------------");
        return this->connected;
    }
    debugln("-------------------");
    return false;
}

void Zinu::countPackets(size_t numBytes) {
    debugln("countPackets");
    // if (numBytes % MAX_BYTES_UDP == 0 || (numBytes/MAX_BYTES_UDP) < 1) {
    //     this->num_packages = 1;
    //     this->last_package_bytes = 0;
    //     return;
    // };
    this->num_packages = (numBytes / MAX_BYTES_UDP) + 1;
    this->last_package_bytes = numBytes % MAX_BYTES_UDP;
    debugf("num_packages: %d\n", this->num_packages);
    debugf("last_package_bytes: %d\n", this->last_package_bytes);
    debugln("-------------------");
}

void Zinu::readIncomingSignal() {
    this->signalbuffer[0] = NO_PING;
    if (this->socket.parsePacket() > 0) {
        this->socket.read(this->signalbuffer, sizeof(this->signalbuffer));
        debugf("signalbuffer: %d\n", this->signalbuffer[0]);
    }
} 