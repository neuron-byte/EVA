#include <WiFi.h>
#include "src/zinu/Zinu.h"
#include "src/esp_32_cam/setupCam.h"
#include "esp_camera.h"
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
camera_fb_t* frameBuffer = NULL;

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
  delay(50);
  digitalWrite(pin, LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

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
  Serial.println("EVA ainda não foi conectado ao MAGI.");
  zinu->handShake();
  delay(1000);
  }
  Serial.println("EVA foi conectado ao MAGI.");
}


void loop() {
  zinu->checkIncomingSignal();
  switch (zinu->state) {
    case DATA_REQUEST:
      blink_led(LED_BUILTIN);
      Serial.println("Iniciando camera.");
      Serial.println("Camera iniciada.");
      Serial.println("Capturando Frame");
      flashLight(255);
      frameBuffer = esp_camera_fb_get();
      if(!frameBuffer){
        Serial.println("Error: Falha ao capturar frame");
        flashLight(0);
        break;
      }
      zinu->state = SENDING_DATA;
      break;
    case SENDING_DATA:
      Serial.println("Enviando dados");
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
}
