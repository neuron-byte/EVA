#include <WiFi.h>
#include "debug.h"
#include "cam_output.h"
#include "src/zinu/Zinu.h"
#include "src/cam/OV7670.h"
#include "src/moving/moving_signals.h"
#include "src/moving/motors_moving.h"

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
// OV7670 *camera;
byte magi_signal;
const char* data;
uint8_t* dataptr;

void wifi_ap_setup() {
  infoln("Configurando ponto de acesso.");

  if (!WiFi.softAP(SSID, PASSWORD)) {
    infoln("Criação do Soft AP falhou.");
    delay(1000);
  }

  if (!WiFi.softAPConfig(apIP, apIP, subnet)) {
    infoln("Configuração do Soft AP falhou.");
    delay(1000);
  }

  IPAddress myIP = WiFi.softAPIP();
  infof("SSID da rede: %s\n", WiFi.softAPSSID());
}

void blink_led(int pin) {
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
}

void setup() {
  data = "Ola, acabei de enviar uma imagem para voce!";
  dataptr = (uint8_t*) data;
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  wifi_ap_setup();

  // camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  zinu = new Zinu(UDP_PORT);

  while (!zinu->connected) {
    infoln("EVA ainda não foi conectado ao MAGI.");
    zinu->handShake();
    delay(1000);
  }
}

void loop() {
  magi_signal = zinu->checkIncomingSignal();

  switch (zinu->state) {
    case DATA_REQUEST:
      blink_led(LED_BUILTIN);
      // camera->oneFrame();
      zinu->state = SENDING_DATA;
      break;
    case SENDING_DATA:
      blink_led(LED_BUILTIN);
      // zinu->sendData(camera->frame, (camera->xres * camera->yres * 2));
      zinu->sendData(dataptr, strlen(data));
      zinu->state = STAND_BY;
      break;
    case RESETING:
      // resete imagem e tudo mais
      break;
  }

  switch (magi_signal) {
    case MOVE_FORWARD:
      zinu->sendSignal(RECIVED);
      reset_move();
      move_forward();
      delay(500);
      reset_move();
      break;
    case MOVE_BACK:
      zinu->sendSignal(RECIVED);
      reset_move();
      move_back();
      delay(500);
      reset_move();
      break;
    case TURN_RIGHT:
      zinu->sendSignal(RECIVED);
      reset_move();
      turn_right();
      delay(500);
      reset_move();
      break;
    case TURN_LEFT:
      zinu->sendSignal(RECIVED);
      reset_move();
      turn_left();
      delay(500);
      reset_move();
      break;
    case STOP:
      zinu->sendSignal(RECIVED);
      reset_move();
      move_stop();
      delay(500);
      reset_move();
      break;
  }
  magi_signal = 0;
}
