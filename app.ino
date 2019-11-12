#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

#define print console.println

PROGMEM uint8_t font[] = 
#include "font.h"
;

#define console Serial1

int inc = 2;
int i = 0;
int gaugevalue = 0;
char line[100];

// Eve2Display(int cs, int pdn, int interrupt); 
Eve2Display display(PB4,PB3,PB5);

#define PIN_SPEAKER PA4
#define DURATION    250

void beep() {
  uint32_t timeout = millis() + 50;
  
  while (millis() < timeout) {
    digitalWrite(PIN_SPEAKER, HIGH);
    delayMicroseconds(DURATION);
    digitalWrite(PIN_SPEAKER, LOW);
    delayMicroseconds(DURATION);
  }
}


void setup() {
  console.begin(115200);
  console.println("\x1B[2J\x1b[H");
  console.println("\nEVE2-Arduino_STM32 setup");
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);

  display.begin();

  display.dlStart();
  display.romfont(1,31);
  display.romfont(2,32);
  display.romfont(3,33);
  display.romfont(4,34);
  display.fgcolor(0xFF0000);
  display.bgcolor(0x001100);
  display.dlEnd();
  fontSetup();
  buttonSetup();
  circleSetup();

  pinMode(PIN_SPEAKER, OUTPUT);
}

void loop() {
  gauge();
}

void loopAll() {
  switch ((millis() / 10000) % 16) {
    case 0:
      buttonLoop();
      break;
    case 1:
      spinner();
      break;
    case 2:
      fontCustom();
      break;
    case 3:
      gradient();
      break;
    case 4:
      keys();
      break;
    case 5:
      clock();
      break;
    case 6:
      scrollbar();
      break;
    case 7:
      progress();
      break;
    case 8:
      toggle();
      break;
    case 9:
      slider();
      break;
    case 10:
      number();
      break;
    case 11:
      buttonLoop();
      break;
    case 12:
      gauge();
      break;
    case 13:
      text();
      break;
    case 14:
      dro();
      break;
    case 15:
      fontSize();
      break;
  }


}

void spinner() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 50, 255));
  display.spinner(display.center,display.middle,0,2);
  display.dlEnd();
}

void toggle() { // not functioning
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  console.println(i&0x01);
  display.toggle(display.center,display.middle,80,30,0,((i++)&0x01)*65535,"yes\xFFno");
  display.dlEnd();
  delay(1000);
}

void keys() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.keys(display.center-200,display.middle-50,400,100,28,0,"ABCDE");
  display.dlEnd();
  delay(1000);
}

void slider() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.slider(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
  display.dlEnd();
  i += 5;
  delay(100);
}

void scrollbar() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.scrollbar(20,display.middle,display.width - 50,20,0,i%100,20,100);
  display.dlEnd();
  console.println(i);
  i += 5;
  delay(100);
}

void progress() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.progress(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
  display.dlEnd();
  i += 5;
  delay(100);
}

void number() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.number(display.center,display.middle,4,OPT_CENTER|OPT_SIGNED,i++);
  display.dlEnd();
  delay(100);
}

void buttonSetup() {
  display.dlStart();
  // display.rotate(2);
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));

  display.tag(1);
  display.button(5,10,80,50,31,OPT_CENTER,"1");
  display.tag(2);
  display.button(95,10,80,50,31,OPT_CENTER,"2");
  display.tag(3);
  display.button(185,10,80,50,31,OPT_CENTER,"3");

  display.tag(4);
  display.button(5,70,80,50,31,OPT_CENTER,"4");
  display.tag(5);
  display.button(95,70,80,50,31,OPT_CENTER,"5");
  display.tag(6);
  display.button(185,70,80,50,31,OPT_CENTER,"6");

  display.tag(7);
  display.button(5,130,80,50,31,OPT_CENTER,"7");
  display.tag(8);
  display.button(95,130,80,50,31,OPT_CENTER,"8");
  display.tag(9);
  display.button(185,130,80,50,31,OPT_CENTER,"9");

  display.tag(10);
  display.button(5,190,80,50,29,OPT_CENTER,"Delete");
  display.tag(11);
  display.button(95,190,80,50,31,OPT_CENTER,"0");
  display.tag(12);
  display.button(185,190,80,50,29,OPT_CENTER,"Enter");
  display.dlEnd();
  delay(1000);
}

void circleSetup() {
  display.dlStart();
  display.circle(60,60,50,0xFFFFFF,5,0);
  //display.circle(260,60,50,0xFFFFFF,2,1);
  //display.circle(160,160,50,0,0,0);
  display.dlEnd();
}

uint32_t touchTimeout;
void buttonLoop() {
  uint32_t now = millis();
  if (now > touchTimeout) {
    uint8_t t = display.touched();
    if (t) {
      beep();
      sprintf(line,"%-6d touch %d",now);
      console.println(line);
    }
    touchTimeout = now + 200;
  }
}

void gauge() {
  sprintf(line,"%d",gaugevalue);
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.text(display.center,display.height - 40,3,OPT_CENTER,line);
  display.gauge(display.center,display.middle,display.height/1.9,OPT_FLAT|OPT_NOBACK|OPT_NOPOINTER,10,10,gaugevalue,100);

  display.cmd(COLOR_RGB(255, 0, 0));
  display.gauge(display.center,display.middle,display.height/1.9,OPT_FLAT|OPT_NOBACK|OPT_NOTICKS,10,10,gaugevalue,100);

  display.dlEnd();

  gaugevalue += inc;
  if ((gaugevalue > 100) || (gaugevalue < 0)) {
    inc = -inc;
    gaugevalue += inc;
    delay(100);
  }

  delay(50);
}

uint8_t h,m,s;
void clock() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.clock(display.center,display.middle,125,OPT_FLAT|OPT_NOBACK|OPT_NOHANDS,h,m,s,0);
  display.cmd(COLOR_RGB(255,0,0));
  display.clock(display.center,display.middle,125,OPT_FLAT|OPT_NOBACK|OPT_NOTICKS,h,m,s,0);
  display.dlEnd();

  h += 1;
  h %= 12;
  m += 2;
  m %= 60;
  s += 3;
  s %= 60;

  delay(50);
}

void gradient() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.gradient(10,10,COLOR_RGB(255, 255, 0), display.width-10,display.height-10,COLOR_RGB(0, 255, 255));
  display.dlEnd();
  delay(1000);
}

void text() {
  sprintf(line,"helloworld %d",millis());
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.text(display.center,display.middle,30,OPT_CENTER,line);
  delay(50);
}

void fontSetup() {
  display.loadRAM(RAM_G + 1000,font,sizeof(font));

  display.dlStart();
  display.cmd(CLEAR_COLOR_RGB(0,0,0));
  display.cmd(CLEAR(1,1,1));
  display.bitmaphandle(14);

// 40 pt
  display.bitmapsource(1000+148 - (32*3*47)); // see AN_014
  display.bitmaplayout(L1,3,47);
  display.bitmapsize(NEAREST,BORDER,BORDER,24,47);

/* 80
  display.bitmapsource(1000+148 - (32*6*94)); // see AN_014
  display.bitmaplayout(L1,6,94);
  display.bitmapsize(NEAREST,BORDER,BORDER,48,94);
*/

/* an_014 
  display.bitmapsource(-1252);
  display.bitmaplayout(L1,3,25);
  display.bitmapsize(NEAREST,BORDER,BORDER,18,25);
*/

  display.setfont(14,RAM_G + 1000);
  // display.setfont2(14,RAM_G + 1000,32);

  display.dlEnd();
}

void fontCustom() {
  display.dlStart();
  display.cmd(CLEAR_COLOR_RGB(255,255,255)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0,0,0));
  display.text(0,0,14,0,"+123.456");
  display.dlEnd();
  delay(100); 
}

void dro() {
  float y;
  float t = millis()/1000000.0;
  display.dlStart();
  // display.rotate(2);
  display.cmd(CLEAR_COLOR_RGB(255,255,255)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0,0,0));
  display.text(0,0,29,0,"X");
  display.text(0,84,29,0,"Y");
  display.text(0,169,29,0,"Z");

  // X
  y = 999*sin(t);
  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(255,0,0));
  } else {
    display.cmd(COLOR_RGB(0,0,0));
  }
  sprintf(line,"%04.03f",y);
  display.text(370,-15,4,OPT_RIGHTX,line);

  // Y
  y = 999*sin(3 + t);
  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(255,0,0));
  } else {
    display.cmd(COLOR_RGB(0,0,0));
  }
  sprintf(line,"%04.03f",y);
  display.text(370,70,4,OPT_RIGHTX,line);

  // Z 
  y = 999*sin(4 + millis()/10000000.0);
  sprintf(line,"%04.03f",y);
  display.cmd(COLOR_RGB(200,200,200));
  display.text(373,158,4,OPT_RIGHTX,line);

  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(255,0,0));
  } else {
    display.cmd(COLOR_RGB(0,0,0));
  }
  display.text(370,155,4,OPT_RIGHTX,line);

  display.dlEnd();
  delay(150);
}


void fontSize() {
  display.dlStart();
  display.rotate(2);
  display.cmd(CLEAR_COLOR_RGB(255,255,255)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0,0,0));


  display.text(1,1,16,0,"16 The quick brown fox jumps over the lazy dog");
  display.text(1,30,18,0,"18 The quick brown fox jumps over the lazy dog");
  display.text(1,60,20,0,"20 The quick brown fox jumps over the lazy dog");
  display.text(1,90,21,0,"21 The quick brown fox jumps over the lazy dog");
  display.text(1,120,22,0,"22 The quick brown fox jumps over the lazy dog");
  display.text(1,150,23,0,"23 The quick brown fox jumps over the lazy dog");
  display.text(1,180,24,0,"24 The quick brown fox jumps over the lazy dog");
  display.text(1,205,25,0,"25 The quick brown fox jumps over the lazy dog");
  display.text(1,250,26,0,"26 The quick brown fox jumps over the lazy dog");
  display.text(1,270,27,0,"27 The quick brown fox jumps over the lazy dog");
  display.text(1,300,28,0,"28 The quick brown fox jumps over the lazy dog");
  display.text(1,330,29,0,"29 The quick brown fox jumps over the lazy dog");
  display.text(1,360,30,0,"30 The quick brown fox jumps over the lazy dog");
  display.text(1,390,31,0,"31 The quick brown fox jumps over the lazy dog");
  display.text(1,430,1,0,"1 The quick brown fox jumps over the lazy dog");
  display.text(1,470,2,0,"2 The quick brown fox jumps over the lazy dog");
  display.text(1,520,3,0,"3 The quick brown fox jumps over the lazy dog");
  display.text(1,580,4,0,"4 The quick brown fox jumps over the lazy dog");

  display.text(1,670,14,0,"14 +123.456"); // custom font is only numbers and +-.
  display.text(1,710,23,0,"note 14 is custom mono font with numbers-only char set");

  display.dlEnd();
  delay(2000);
  display.dlStart();
  display.rotate(0);
  display.dlEnd();
}
