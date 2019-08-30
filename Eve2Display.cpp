#include <SPI.h>
#define EVE2_43
#include "Eve2Display.h"

SPISettings spiSettings(20000000, MSBFIRST, SPI_MODE0);  

Eve2Display::Eve2Display(int cs, int pdn, int audio) {
  pinCS     = cs;
  pinPDN    = pdn;
  pinAUDIO  = audio;
};

void Eve2Display::begin() {
  pinMode(pinCS,OUTPUT);
  pinMode(pinPDN,OUTPUT);
  pinMode(pinAUDIO,OUTPUT);
  SPI.begin();

  digitalWrite(pinAUDIO, LOW);

  digitalWrite(pinPDN, HIGH);
  digitalWrite(pinPDN, LOW);
  digitalWrite(pinPDN, HIGH);

  hostCommand(HCMD_CLKEXT);
  hostCommand(HCMD_ACTIVE);

  while (rd8(RAM_REG + REG_ID) != 0x7C) {};

  // turn off screen output during startup
   wr8(RAM_REG + REG_PCLK, 0);              // Pixel Clock Output disable

  // screen config
  wr16(RAM_REG + REG_HCYCLE  , HCYCLE);
  wr16(RAM_REG + REG_HOFFSET , HOFFSET);
  wr16(RAM_REG + REG_HSYNC0  , HSYNC0);
  wr16(RAM_REG + REG_HSYNC1  , HSYNC1);
  wr16(RAM_REG + REG_VCYCLE  , VCYCLE);
  wr16(RAM_REG + REG_VOFFSET , VOFFSET);
  wr16(RAM_REG + REG_VSYNC0  , VSYNC0);
  wr16(RAM_REG + REG_VSYNC1  , VSYNC1);
   wr8(RAM_REG + REG_SWIZZLE , SWIZZLE);
   wr8(RAM_REG + REG_PCLK_POL, PCLK_POL);
  wr16(RAM_REG + REG_HSIZE   , HSIZE);
  wr16(RAM_REG + REG_VSIZE   , VSIZE);
   wr8(RAM_REG + REG_CSPREAD , CSPREAD);
   wr8(RAM_REG + REG_DITHER  , DITHER);

  // touch & audio config
  wr16(RAM_REG + REG_TOUCH_RZTHRESH,  1200);    // set touch resistance threshold
   wr8(RAM_REG + REG_TOUCH_MODE,      0x02);    // set touch on: continous - this is default
   wr8(RAM_REG + REG_TOUCH_ADC_MODE,  0x01);    // set ADC mode: differential - this is default
   wr8(RAM_REG + REG_TOUCH_OVERSAMPLE,15);      // set touch oversampling to max

  wr16(RAM_REG + REG_PWM_HZ  ,  0x00FA);        // Backlight PWM frequency
   wr8(RAM_REG + REG_PWM_DUTY , 0x7F);          // Backlight PWM duty (off)

   wr8(RAM_REG + REG_GPIO_DIR,0x80 | rd8(RAM_REG + REG_GPIO_DIR));
   wr8(RAM_REG + REG_GPIO,0x080 | rd8(RAM_REG + REG_GPIO));//enable display bit

  // make display list with bluescreen

   wr32(RAM_DL+0, CLEAR_COLOR_RGB(0,100,0));
   wr32(RAM_DL+4, CLEAR(1,1,1));
   wr32(RAM_DL+8, DISPLAY());
   wr8(RAM_REG + REG_DLSWAP, DLSWAP_FRAME);          // swap display lists
   wr8(RAM_REG + REG_PCLK, 5);                       // after this display is visible on the LCD
}

void Eve2Display::hostCommand(uint8_t command) {
  spiEnable();
  SPI.transfer(command);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  spiDisable();
}

uint8_t  Eve2Display::rd8(uint32_t address) {
  uint8_t buf[1];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 1);
  spiDisable();
  return buf[0];
}

uint16_t Eve2Display::rd16(uint32_t address) {
  uint8_t buf[2];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 2);
  spiDisable();
  return buf[0] + ((uint16_t)buf[1] << 8);
}

uint32_t Eve2Display::rd32(uint32_t address) {
  uint8_t buf[4];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 4);
  spiDisable();
  return buf[0] + ((uint32_t)buf[1] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[3] << 24);
}

void Eve2Display::wr8(uint32_t address, uint8_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write((uint8_t)(a[2]&0x3F | 0x80));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(parameter);
  spiDisable();
}

void Eve2Display::wr16(uint32_t address, uint16_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write((uint8_t)a[2]&0x3F | 0x80); 
  SPI.write((uint8_t)a[1]); 
  SPI.write((uint8_t)a[0]);
  SPI.write(&parameter,2);
  spiDisable();
}

void Eve2Display::wr32(uint32_t address, uint32_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write((uint8_t)(a[2]&0x3F | 0x80));  
  SPI.write((uint8_t)a[1]);     
  SPI.write((uint8_t)a[0]);              
  SPI.write(&parameter,4);
  spiDisable();
}

void Eve2Display::spiEnable() {
  SPI.beginTransaction(spiSettings);
  digitalWrite(pinCS, LOW);
}

void Eve2Display::spiDisable() {
  digitalWrite(pinCS, HIGH);
  SPI.endTransaction();
}
