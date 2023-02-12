#include "DEV_Config.h"
#include "LCD_st7735.h"
#include <stdio.h>
#include <stdlib.h>
#include <hardware/uart.h>
int t_d = 200;
void setup() {
  Serial.begin(9600);
  // open the serial to contiunue...
  // otherwise the program will be stucked
  // in this loop.
  while(!Serial){
    ;
  }
  Serial.print(F("Hello! ST7735 TFT Test\n"));
  ST7735_Init();
  Serial.println(F("Initialized\n"));
  ST7735_DrawImage(0, 0, 80, 160, arducam_logo);
  delay(t_d);
  ST7735_FillScreen(ST7735_BLACK);
  delay(t_d);
  testlines(ST7735_GREEN);
  delay(t_d);
  testlines(ST7735_YELLOW);
  delay(t_d);
  testlines(ST7735_BLUE);
  delay(t_d);
  testlines(ST7735_RED);
  delay(t_d);
  ST7735_FillScreen(ST7735_BLACK);
  
  ST7735_WriteString(0,0,"Hello", Font_11x18, ST7735_YELLOW, ST7735_BLACK);
  delay(100);
  ST7735_WriteString(0,30,"world", Font_11x18, ST7735_YELLOW, ST7735_BLACK);
  delay(100);
  ST7735_WriteString(0,50,"GOOD", Font_16x26, ST7735_GREEN, ST7735_BLACK);
  delay(100);
  ST7735_WriteString(0,78,"AFTERNOON", Font_7x10, ST7735_GREEN, ST7735_BLACK);
  
}

void loop() {
  
  delay(500);

}

void testlines(uint16_t color) {
  ST7735_FillScreen(ST7735_BLACK);
  for (int16_t x = 0; x < ST7735_WIDTH; x ++) {
      ST7735_FillRectangle(0, 0, x, ST7735_HEIGHT - 1, color);
  }

}
