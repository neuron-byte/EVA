#include <WiFi.h>
#include "OV7670.h"
#include "BMP.h"
#include "ov7670_wire.h"
#include "wifi_conf.h"
#include "Zinu.h"
#include "actions_enum.h"


//#define EN_M1
//#define EN_M2
//#define DIRECTION_M1_A
//#define DIRECTION_M1_B
//#define DIRECTION_M2_A
//#define DIRECTION_M2_B

OV7670 *camera;
Zinu *zinu;
PROGMEM uint8_t grayscaleBuffer[19200];
const size_t sizeGrayScaleBuffer = sizeof(grayscaleBuffer);

void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando Unidade 0 Evangelion");
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Conectando ao WiFi...");
        delay(800);
    }
    Serial.println("Conectado ao Wifi");
    Serial.print("IP da unidade EVA 0: ");
    Serial.println(WiFi.localIP());
    zinu = new Zinu();
    while (!zinu->handShake())
    {
        Serial.println("Unidade EVA ainda não conectada a MAGI");
        delay(500);
    }
    
    camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
    Serial.println("Unidade 0 Evangelion iniciada");
}

//Pega um frame da camera
void takeFrame(){
    camera->oneFrame();
    convertToGrayscale();
}
//Converte para grayscale
void convertToGrayscale(){
    unsigned char* frameToConvert = camera->frame;
}
//Faz o EVA virar a direita
void turnRight(){}
//Faz o EVA virar a esquerda
void turnLeft(){}
//Faz o EVA ir a frente
void moveFoward(){}
//Faz o EVA dar ré
void moveBackward(){}

void loop(){
    camera->oneFrame();
    byte signal = zinu->listener(camera->frame,camera->frameBytes); //120x160x1(1byte = uint8_t)
    switch (signal)
    {
    case MOVE_FORWARD:
        moveFoward();
        break;
    case MOVE_BACKWARD:
        moveBackward();
        break;
    case MOVE_LEFT:
        turnLeft();
        break;
    case MOVE_RIGHT:
        turnRight();
        break;
    case TAKE_FRAME:
        takeFrame();
        break;
    default:
        break;
    }
}