#include <WiFi.h>
#include <WiFiUdp.h>
#include "OV7670.h"
#include "BMP.h"
#include "ov7670_wire.h"
#include "wifi_conf.h"

OV7670 *camera;
WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Unidade 0 Evangelion");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando ao WiFi...");
    delay(800);
  }
  Serial.println("Conectado ao Wifi");

  while (!udp.begin(UDP_PORT)) {
    Serial.println("Erro ao iniciar servidor UDP");
    Serial.println("Tentando iniciar servidor UDP...");
    delay(50);
  }
  Serial.println("Servidor UDP iniciado");
  delay(500);
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565, SIOD, SIOC, VSYNC, HREF, XCLK, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  Serial.println("Unidade 0 Evangelion iniciada");
  Serial.print("IP da unidade EVA 0: ");
  Serial.println(WiFi.localIP());
}

float convert_to_grayscale() {
}

void loop() {
  if (udp.parsePacket()) {
    Serial.println("Recebi algo!");
    byte bf[1];
    int len = udp.read(bf, sizeof(bf));
    if (bf[0] == 0) {
      uint8_t num = 1;
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(&num, sizeof(num));
      udp.endPacket();
    }
  }

  if (udp.parsePacket()) {
    Serial.println("Recebi algo2!");
    byte bf[1];
    int len = udp.read(bf, sizeof(bf));
    if (bf[0] == 0) {
      Serial.println("aqui?");
      uint8_t num = 254;
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(&num, sizeof(num));
      udp.endPacket();
    }
  }

  /*
    if (udp.parsePacket())
    {
        Serial.println("Recebi algo 2!");
        byte bf[1];
        int len = udp.read(bf, sizeof(bf));
        if (bf[0] == 0)
        {
            int i = 0;
            for (int x = 0; x < xres; x++)
                for (int y = 0; y < yres; y++)
                {
                    i = (y * xres + x) << 1;
                    udp.beginPacket(udp.remoteIP(), udp.remotePort());
                    udp.write(&i, sizeof(i));
                    udp.endPacket();
                }
        }
    }*/

  /*
    camera->oneFrame();
    unsigned char *currentFrame = camera->frame;
    int i = 0;
    byte n = ping(0);
    Serial.printf("R: %i", n);
    delay(1000);*/
}