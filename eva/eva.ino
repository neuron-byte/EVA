#include "src/zinu/zinu.h"
#include "src/cam/OV7670.h"
#include <WiFi.h>

#define LED_BUILTIN 2 

#define UDP_PORT 12345
#define SSID     "EVA AP"
#define PASSWORD "12345678"

// ===== Camera pinout =====
const int SIOD = 21; //SDA
const int SIOC = 22; //SCL
const int VSYNC = 34;
const int HREF = 35;
const int XCLK = 32;
const int PCLK = 33;
const int D0 = 27;
const int D1 = 17;
const int D2 = 16;
const int D3 = 15;
const int D4 = 14;
const int D5 = 13;
const int D6 = 12;
const int D7 = 4;

// ===== IP =====
IPAddress apIP(192,168,4,1);
IPAddress subnet(255,255,255,0);

Zinu *zinu;
OV7670 *camera;


void wifi_ap_setup() {
  Serial.println("Configurando ponto de acesso.");

  if (!WiFi.softAP(SSID, PASSWORD)) {
    Serial.println("Criação do Soft AP falhou.");
    delay(1000);
  }

  if (!WiFi.softAPConfig(apIP, apIP, subnet)) {
    Serial.println("Configuração do Soft AP falhou.");
    delay(1000);
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Endereço de IP: ");
  Serial.println(myIP);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  wifi_ap_setup();

  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  zinu = new Zinu(UDP_PORT);

  while (!zinu->handShake())
  {
      Serial.println("SENDER não conectado ao RECEIVER.");
      delay(1000);
  }
}

void loop() {
  if (!zinu->connected) {
    ESP.restart();
  }
  
  zinu->checkIncomingSignal();

  switch (zinu->state) {
    case WAITING:
      // do nothing
      break;
    case SENDING_DATA:
      camera->oneFrame();
      Serial.print("Frame size: ");
      Serial.println(camera->xres * camera->yres * 2);
      zinu->state = WAITING;
      break;
    case RESETING:
      // resete imagem e tudo mais
      break;
  }
}
