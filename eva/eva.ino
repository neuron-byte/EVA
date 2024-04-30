#include <WiFi.h>
#include "debug.h"
#include "src/zinu/Zinu.h"
#include "src/esp_32_cam/setupCam.h"
#include "esp_camera.h"#include "src/moving/moving_signals.h"
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

// L298N
#define IN1_A 14
#define IN2_A 13
#define IN3_B 12
#define IN4_B 11 

byte moves[8] = {0,1,0,1,0,2,0,1};
byte s = 0;
Zinu *zinu;
camera_fb_t* frameBuffer = NULL;

void wifi_ap_setup() {
  Serial.println("Configurando ponto de acesso.");
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

void move_forward(){
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    digitalWrite(IN3_B, HIGH);
    digitalWrite(IN4_B, LOW);
}

void move_back(){
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    digitalWrite(IN3_B, LOW);
    digitalWrite(IN4_B, HIGH);
}

void turn_right(){
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    digitalWrite(IN3_B, LOW);
    digitalWrite(IN4_B, HIGH);
}

void turn_left(){
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    digitalWrite(IN3_B, HIGH);
    digitalWrite(IN4_B, LOW);
}


void moving(){
  switch (moves[s])
  {
  case 0:
    move_forward();
    break;
  case 1:
    turn_left();
    break;
  case 2:
    turn_right();
    break;
  }

  s = s==7 ? 0 : s++;
}

void setup() {
  data = "Ola, acabei de enviar uma imagem para voce!";
  dataptr = (uint8_t*) data;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(IN3_B, OUTPUT);
  pinMode(IN4_B, OUTPUT);

  Serial.begin(115200);

  wifi_ap_setup();
  //camera_loop();
  esp_err_t ret = init_camera();
  while(ret != ESP_OK){
      Serial.println("Camera falhou ao iniciar");
      ret = init_camera();
      delay(500);
  }
  Serial.println("Camera iniciada.");
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
  Serial.println("EVA foi conectado ao MAGI.");
}


void loop() {
  magi_signal = zinu->checkIncomingSignal();

  switch (zinu->state) {
    case DATA_REQUEST:
      moving();
      blink_led(LED_BUILTIN);
      //Serial.println("Iniciando camera.");
      //Serial.println("Camera iniciada.");
      //Serial.println("Capturando Frame");
      flashLight(255);
      frameBuffer = esp_camera_fb_get();
      if(!frameBuffer){
        //Serial.println("Error: Falha ao capturar frame");
        flashLight(0);
        break;
      }
      zinu->state = SENDING_DATA;
      
      break;
    case SENDING_DATA:
      //Serial.println("Enviando dados");
      blink_led(LED_BUILTIN);
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
