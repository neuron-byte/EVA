#include <WiFi.h>
#include "cam_output.h"
#include "src/zinu/Zinu.h"
#include "src/cam/OV7670.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 
#endif

// ===== wifi =====
#define UDP_PORT 12345
#define SSID     "EVA AP"
#define PASSWORD "12345678"
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
  Serial.print("SSID da rede: ");
  Serial.println(WiFi.softAPSSID());
  Serial.print("Endereço de IP: ");
  Serial.println(myIP);
}

void blink_led(int pin) {
  digitalWrite(pin, HIGH);
  delay(100);
  digitalWrite(pin, LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  wifi_ap_setup();

  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  zinu = new Zinu(UDP_PORT);

  while (!zinu->connected) {
  Serial.println("EVA ainda não foi conectado ao MAGI.");
  zinu->handShake();
  delay(1000);
  }
}

void loop() {
  zinu->checkIncomingSignal();

  switch (zinu->state) {
    case SENDING_DATA:
      blink_led(LED_BUILTIN);
      camera->oneFrame();
      Serial.print("Frame size: ");
      Serial.println(camera->xres * camera->yres * 2);
      zinu->state = STAND_BY;
      break;
    case RESETING:
      // resete imagem e tudo mais
      break;
  }
}
