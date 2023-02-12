#include "arducam_hm01b0.h"

uint8_t image_data[96 * 96];
struct arducam_config config;
// the following hearder may be for their processding sketch
//uint8_t header[2] = {0x55, 0xAA};
char buf[4];

void setup() {
  // This is the usb serial
  Serial.begin(115200);

  // serial1 is not the usb serial, its TRX is GPIO 0 and 1 
  // Serial1.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
  config.sccb            = i2c0;
  config.sccb_mode       = I2C_MODE_16_8;
  config.sensor_address  = 0x24;
  config.pin_sioc        = PIN_CAM_SIOC;
  config.pin_siod        = PIN_CAM_SIOD;
  config.pin_resetb      = PIN_CAM_RESETB;
  config.pin_xclk        = PIN_CAM_XCLK;
  config.pin_vsync       = PIN_CAM_VSYNC;
  config.pin_y2_pio_base = PIN_CAM_Y2_PIO_BASE;
  config.pio             = pio0;
  config.pio_sm          = 0;
  config.dma_channel     = 0;
  arducam_init(&config);

}

void loop() {
  // use serial event to trigger image capture, the python code 
  // will send something to make the buffer not empty. And this
  // behavior will trigger the following routine.
  if (Serial.available()> 0){
    // clear the serial input buffer by read it all
    while (Serial.read() > 0){}
    
    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    arducam_capture_frame(&config, image_data);
    Serial.write(image_data,96 *96);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
}
