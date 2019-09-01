#include <SPI.h>
#include "Eve2Display.h"

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
  height = VSIZE;
  width =  HSIZE;
  center = HSIZE/2;
  middle = VSIZE/2;
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
  // printRAM_CMD(RAM_CMD,40);
}

/* -- debug ------------------------------------------------------ */
void Eve2Display::test() {
  console.println("test");
  dlStart();
  cmd(CLEAR(1,1,1));
  cmd(COLOR_RGB(192,26,26));
  //dial(50,50,20,0,37);
  dial(150,150,40,0,37);
  dlEnd();
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

/* -- widgets ------------------------------------------------------ */
void Eve2Display::romfont(uint32_t font, uint32_t romslot) {
  cmd(CMD_ROMFONT);
  cmd(font);
  cmd(romslot);
}

void Eve2Display::cmdString(const char *str) {
  uint16_t len = strlen(str);
  memcpy(&commands[commandIndex],str,len+1);  // this only works with little endian, it does null terminate if len NOT % FT_CMD_SIZE
  commandIndex += len/4;
  if (len % FT_CMD_SIZE) { 
    // otherwise we did a fractional write BUT it already includes the null but commandIndex is sly by 1 because of int division above
    commandIndex++;
  } else { 
    // len % FT_CMD_SIZE so we to add a null termination command
    cmd(0x00); // null terminate this str
  }

}

void Eve2Display::dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val) {
  cmd(CMD_DIAL);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)options << 16) | r );
  cmd((uint32_t)val );
}

void Eve2Display::gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range) {
  cmd(CMD_GAUGE);
  cmd(((uint32_t)y       << 16) | x );
  cmd(((uint32_t)options << 16) | r );
  cmd(((uint32_t)minor   << 16) | major );
  cmd(((uint32_t)range   << 16) | val );
}

void Eve2Display::text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str) {
  cmd(CMD_TEXT);
  cmd(((uint32_t)y       << 16) | x );
  cmd(((uint32_t)options << 16) | font );
  cmdString(str);
}

void Eve2Display::button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str) {
  cmd(CMD_BUTTON);
  cmd(((uint32_t)y << 16) | x ); // Put two 16 bit values together into one 32 bit value - do it little endian
  cmd(((uint32_t)h << 16) | w );
  cmd(((uint32_t)options << 16) | font );
  cmdString(str);
}

void Eve2Display::number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num) {
  cmd(CMD_NUMBER);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)options << 16) | font );
  cmd(num);
}

void Eve2Display::slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range) { 
  cmd(CMD_SLIDER);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)h << 16) | w );
  cmd(((uint32_t)val << 16) | options );
  cmd((uint32_t)range );
}


void Eve2Display::toggle(uint16_t x, uint16_t y, uint16_t w, uint16_t font, uint16_t options, uint16_t state,const char *str) { 
  cmd(CMD_TOGGLE);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)w << 16) | font );
  cmd(((uint32_t)options << 16) | state );
  cmdString(str);
}

void Eve2Display::progress(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range) { 
  cmd(CMD_PROGRESS);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)h << 16) | w );
  cmd(((uint32_t)val << 16) | options );
  cmd((uint32_t)range );
}

void Eve2Display::scrollbar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range) { 
  cmd(CMD_SCROLLBAR);
  cmd(((uint32_t)y << 16) | x );
  cmd(((uint32_t)h << 16) | w );
  cmd(((uint32_t)val << 16) | options );
  cmd((uint32_t)range );
}

void Eve2Display::keys(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str) {
  cmd(CMD_KEYS);
  cmd(((uint32_t)y << 16) | x ); // Put two 16 bit values together into one 32 bit value - do it little endian
  cmd(((uint32_t)h << 16) | w );
  cmd(((uint32_t)options << 16) | font );
  cmdString(str);
}

void Eve2Display::clock(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms) {
  cmd(CMD_GAUGE);
  cmd(((uint32_t)y       << 16) | x );
  cmd(((uint32_t)options << 16) | r );
  cmd(((uint32_t)h       << 16) | m );
  cmd(((uint32_t)s       << 16) | ms );
}

void Eve2Display::gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1) {
  cmd(CMD_GRADIENT);
  cmd(((uint32_t)y0      << 16) | x0 );
  cmd(rgb0);
  cmd(((uint32_t)y1      << 16) | x1 );
  cmd(rgb1);
}

void Eve2Display::spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale) {
  cmd(CMD_SPINNER);
  cmd(((uint32_t)y      << 16) | x );
  cmd(((uint32_t)style   << 16) | scale );
}
