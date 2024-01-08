#include <WiFiUdp.h>
class Zinu
{
public:
    Zinu();
    byte handShake();
    byte listener(uint8_t* bufferData, size_t bufferSize);

private:
    WiFiUDP socket;
    int num_packages;
    int last_package_bytes;
    void signalResponse(byte signal);
    void dataResponse_8bits(uint8_t* bufferData);
    void countingPackages(size_t numBytes);
    byte* readSignal();
};