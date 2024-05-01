#include <WiFi.h>
#include "debug.h"
#include "esp_camera.h"
#include "src/esp_32_cam/setupCam.h"
#include "src/zinu/Zinu.h"
#include "src/moving/moving_signals.h"
#include "src/moving/motors_moving.h"

// ===== wifi =====
#define UDP_PORT 12345
#define SSID     "EVA AP"
#define PASSWORD "12345678"
IPAddress apIP(192,168,4,1);
IPAddress subnet(255,255,255,0);

Zinu *zinu;
camera_fb_t* frameBuffer = NULL;
byte magi_signal;

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
  infof("SSID da rede: %s\n", WiFi.softAPSSID());
}

void setup() {
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(IN3_B, OUTPUT);
  pinMode(IN4_B, OUTPUT);

  Serial.begin(115200);

  wifi_ap_setup();
  
  esp_err_t ret = init_camera();
  while(ret != ESP_OK){
      infoln("Camera falhou ao iniciar");
      ret = init_camera();
      delay(500);
  }
  infoln("Camera iniciada.");
  setupLedFlash();
  flashLight(255);
  delay(500);
  flashLight(0);

  zinu = new Zinu(UDP_PORT);
  while (!zinu->connected) {
    infoln("EVA ainda não foi conectado ao MAGI.");
    zinu->handShake();
    delay(1000);
  }
  infoln("EVA foi conectado ao MAGI.");
}

void loop() {
  magi_signal = zinu->checkIncomingSignal();

  switch (zinu->state) {
    case DATA_REQUEST:
      flashLight(255);
      debugln("Capturando Frame");
      frameBuffer = esp_camera_fb_get();
      if(!frameBuffer){
        errorln("Falha ao capturar frame");
        flashLight(0);
        break;
      }
      zinu->state = SENDING_DATA;
      break;
    case SENDING_DATA:
      debugln("Enviando dados");
      zinu->sendData(frameBuffer->buf, frameBuffer->len);
      if(frameBuffer){
        esp_camera_fb_return(frameBuffer);
        frameBuffer = NULL;
      }
      zinu->state = STAND_BY;
      break;
    case RESETING:
      // resete imagem e tudo mais
      break;
  }

  switch (magi_signal) {
    case MOVE_FORWARD:
      reset_move();
      move_forward();
      delay(600);
      reset_move();
      break;
    case MOVE_BACK:
      reset_move();
      move_back();
      delay(600);
      reset_move();
      break;
    case TURN_RIGHT:
      reset_move();
      turn_right();
      delay(400);
      reset_move();
      break;
    case TURN_LEFT:
      reset_move();
      turn_left();
      delay(400);
      reset_move();
      break;
    case STOP:
      reset_move();
      move_stop();
      delay(50);
      reset_move();
      break;
  }
}
