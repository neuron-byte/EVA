//*************************************************************************
//	OV7670 (non FIFO) Sample program for ESP32 
//
//  OUTPUT:	ILI9341 TFT(320x240) SPI 
//					option XPT2046 Touchscreen
//
//*************************************************************************
#include <Wire.h>
#include <SPI.h>
#include <OV7670.h>

const camera_config_t cam_conf = {
	.D0	= 27,
	.D1 = 17,
	.D2 = 16,
	.D3 = 15,
	.D4 = 14,
	.D5 = 13,
	.D6 = 12,
	.D7 = 4,
	.XCLK = 32,
	.PCLK = 33,
	.VSYNC = 34,

	.xclk_freq_hz = 10000000,			// XCLK 10MHz
	.ledc_timer		= LEDC_TIMER_0,
	.ledc_channel = LEDC_CHANNEL_0	
};
//	SSCB_SDA(SIOD) 	--> 21(ESP32)
//	SSCB_SCL(SIOC) 	--> 22(ESP32)
//	RESET   --> 3.3V
//	PWDN		--> GND
//	HREF		--> NC

OV7670 cam;

void setup() {
  Serial.begin(9600);
	Wire.begin();
	Wire.setClock(400000);
	
  Serial.println("OV7670 camera Init...");     
  esp_err_t err = cam.init(&cam_conf, QVGA, RGB565);		// カメラを初期化
	if(err != ESP_OK)		Serial.println("cam.init ERROR");

	cam.setPCLK(2, DBLV_CLK_x4);	// PCLK 設定 : 10MHz / (pre+1) * 4 --> 13.3MHz  
	cam.vflip( false );		// 画面１８０度回転
 
  Serial.printf("cam MID = %X\n\r",cam.getMID());
  Serial.printf("cam PID = %X\n\r",cam.getPID());

//	cam.colorbar(true);			// カラーバーを表示する場合

}

void loop(void) {
	uint16_t y, *buf;

	while(1){
		for( y = 0; y < 240; y++){			
			buf = cam.getLine( y+1 );								
			Serial.printf("L:%X\n",buf);
  	}
  	
	}
}

