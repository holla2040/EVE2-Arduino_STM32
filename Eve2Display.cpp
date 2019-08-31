#include <SPI.h>
#define EVE2_43
#include "Eve2Display.h"
#include "eve2.h"

SPISettings spiSettings(20000000, MSBFIRST, SPI_MODE0);  
#define console Serial1

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
  delay(1);
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
  wr8(RAM_REG  + REG_TOUCH_MODE,      0x02);    // set touch on: continous - this is default
  wr8(RAM_REG  + REG_TOUCH_ADC_MODE,  0x01);    // set ADC mode: differential - this is default
  wr8(RAM_REG  + REG_TOUCH_OVERSAMPLE,15);      // set touch oversampling to max

  wr16(RAM_REG + REG_PWM_HZ  ,  0x00FA);        // Backlight PWM frequency
  wr8(RAM_REG +  REG_PWM_DUTY , 0x7F);          // Backlight PWM duty (off)

  wr8(RAM_REG +  REG_GPIO_DIR,0x80 | rd8(RAM_REG + REG_GPIO_DIR));
  wr8(RAM_REG +  REG_GPIO,0x080 | rd8(RAM_REG + REG_GPIO));//enable display bit

  // make display list with bluescreen

  wr32(RAM_DL+0, CLEAR_COLOR_RGB(0,0,0));
  wr32(RAM_DL+4, CLEAR(1,1,1));
  wr32(RAM_DL+8, DISPLAY());
  wr8(RAM_REG  + REG_DLSWAP, DLSWAP_FRAME);          // swap display lists
  wr8(RAM_REG  + REG_PCLK, 5);                       // after this display is visible on the LCD

  ramCommandOffset = 0;

/*
  should print 813
  char line[100];
  sprintf(line,"%X%X",rd8(CHIP_ID),rd8(CHIP_ID+1));
  console.println(line);
*/
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
  SPI.write(a[2]&0x3F);  
  SPI.write(a[1]);      
  SPI.write(a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 1);
  spiDisable();
  return buf[0];
}

uint16_t Eve2Display::rd16(uint32_t address) {
  uint8_t buf[2];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((a[2]&0x3F));  
  SPI.write(a[1]);      
  SPI.write(a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 2);
  spiDisable();
  return buf[0] + ((uint16_t)buf[1] << 8);
}

uint32_t Eve2Display::rd32(uint32_t address) {
  uint8_t buf[4];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write(a[2]&0x3F);  
  SPI.write(a[1]);      
  SPI.write(a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 4);
  spiDisable();
  return buf[0] + ((uint32_t)buf[1] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[3] << 24);
}

void Eve2Display::wr8(uint32_t address, uint8_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write(a[2]&0x3F | 0x80);  
  SPI.write(a[1]);      
  SPI.write(a[0]);              
  SPI.write(parameter);
  spiDisable();
}

void Eve2Display::wr16(uint32_t address, uint16_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write(a[2]&0x3F | 0x80); 
  SPI.write(a[1]); 
  SPI.write(a[0]);
  SPI.write(&parameter,2);
  spiDisable();
}

void Eve2Display::wr32(uint32_t address, uint32_t parameter) {
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write(a[2]&0x3F | 0x80);  
  SPI.write(a[1]);     
  SPI.write(a[0]);              
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

void Eve2Display::dlStart() {
  commandIndex = 0;
  cmd(CMD_DLSTART);
}

void Eve2Display::cmd(uint32_t command) {
  commands[commandIndex++] = command;
}

void Eve2Display::dlEnd() {
/*
  first write commands to RAM_CMD (4K circular co-process command buffer), then
  REG_CMD_WRITE is updated with 4k circular buffer offset, which is commandSize (u32) * 4 in bytes
*/
  uint32_t address;
  uint8_t *a = (uint8_t*)&address;
  uint32_t commandsSize; // in bytes

  cmd(CMD_DISPLAY);
  cmd(CMD_SWAP);

  commandsSize = commandIndex * FT_CMD_SIZE; // this is byte count

  address = RAM_CMD + ramCommandOffset; // ramCommandOffset is where we left off last time

  spiEnable();
  SPI.write(a[2]&0x3F | 0x80);  
  SPI.write(a[1]);     
  SPI.write(a[0]);              
  SPI.write(&commands,commandsSize);
  spiDisable();

  ramCommandOffset += commandsSize;
  ramCommandOffset %= FT_CMD_FIFO_SIZE; // ramCommandOffset is where we should write next time
  address = RAM_REG + REG_CMD_WRITE; 

  spiEnable();
  SPI.write(a[2]&0x3F | 0x80);  
  SPI.write(a[1]);     
  SPI.write(a[0]);              
  SPI.write(&ramCommandOffset,FT_CMD_SIZE);
  spiDisable();
}

void Eve2Display::test() {
  console.println("test");
  dlStart();
  cmd(CLEAR(1,1,1));
  cmd(COLOR_RGB(192,26,26));
  //dial(50,50,20,0,37);
  dial(150,150,40,0,37);
  dlEnd();
}

void Eve2Display::dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val) {
  cmd(CMD_DIAL);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)options << 16) | r );
  cmd((uint32_t)val );
}

void Eve2Display::printRAM_DL() {
  char temp[100];
  console.println("\nRAM_DL");
  console.println("-----------------------");
  for (uint32_t a = RAM_DL;a < (RAM_DL + 100);a += 4) {
    sprintf(temp,"0x%08X 0x%08X",a,rd32(a));
    console.println(temp);
  }
  console.println();
}

void Eve2Display::printCommands() {
  char temp[100];
  console.println("\ncommands");
  console.println("-----------------------");
  for (uint32_t i = 0;i < commandIndex;i++) {
    sprintf(temp,"0x%-3d 0x%08X",i,commands[i]);
    console.println(temp);
  }
  console.println();
}

void Eve2Display::printRAM_CMD(uint32_t address, uint16_t length) {
  char temp[100];
  console.println("\nRAM_CMD");
  console.println("-----------------------");
  for (uint32_t i = 0;i < length;i++) {
  sprintf(temp,"0x%08X 0x%08X",address+i*FT_CMD_SIZE,rd32(address + i*FT_CMD_SIZE));
    console.println(temp);
  }
  console.println();
}

void Eve2Display::log(uint8_t level,char *msg, uint32_t v) {
  if (level >= logLevel) {
    char line[100];
    sprintf(line,"%-8d %-30s 0x%08X %u",millis(),msg,v,v);
    console.println(line);
  }
}
