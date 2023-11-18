#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include "OV7670.h"
#include "BMP.h"
#include "ov7670_wire.h"
#include "wifi_conf.h"

OV7670 *camera;

void setup(){
    Serial.begin(115200);
    Serial.println("Iniciando Unidade 0 Evangelion");
    delay(500);
    camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
    Serial.println("Unidade 0 iniciada!");
}

void loop(){
    camera->oneFrame();
    unsigned char* currentFrame = camera->frame;
    int i = 0;
    for(int x = 0; x < camera->xres; x++)
        for(int y = 0; y < camera->yres; y++)
        {
            i = (y * camera->xres + x) << 1;
            uint16_t color = (currentFrame[i] | (currentFrame[i+1] << 8));
            Serial.printf("Cor: %x\n ", color);
        }  
    Serial.println("OUTRO FRAME");
    delay(1000);
}