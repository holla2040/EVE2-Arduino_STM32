#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

#define console Serial4
PROGMEM uint8_t font[] = 
#include "font.h"
;


int inc = 2;
int i = 0;
int gaugevalue = 0;
char line[100];

int dir = -1;
float t2 = 15.2;

uint8_t navTabSelected = 0;
#define NAVTABENTRYLEN 10
#define NAVTABHEIGHT 50 
#define NAVTABWIDTH  92
char navTabEntries[][NAVTABENTRYLEN] = {"Home","Run","System","Control","Relays"};
// navTabs are tagged starting at 240

// Eve2Display(int cs, int pdn, int interrupt); 
Eve2Display display(PB4,PB3,PB5);

void setup() {
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
  console.begin(115200);
  console.println("\x1B[2J\x1b[H");
  console.println("EVE2-Arduino_STM32 setup");

  pinMode(PC13,OUTPUT);

  display.begin();
  display.tone(2000,20);
  // display.calibrate();

  display.dlStart();
  display.romfont(1,31);
  display.romfont(2,32);
  display.romfont(3,33);
  display.romfont(4,34);
  display.fgcolor(0xFF0000);
  display.bgcolor(0x001100);
  display.dlEnd();

  // fontSetup();
  // buttonSetup();
  // circleSetup();

  navBarSetup();

//  pinMode(PIN_SPEAKER, OUTPUT);
  console.println("setup done");
  //primitives();
  //rect();
  statusSetup();



}

void loop() {
  //dro();
  //gauge();
  //fontSize();
  //navBarLoop();
  //loopAll();
  //rect();
  statusLoop();
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
  display.cmd(COLOR_RGB(0xFF32FF));
  display.spinner(display.center,display.middle,0,2);
  display.dlEnd();
}

void toggle() { // not functioning
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  console.println(i&0x01);
  display.toggle(display.center,display.middle,80,30,0,((i++)&0x01)*65535,"yes\xFFno");
  display.dlEnd();
  delay(1000);
}

void keys() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  display.keys(display.center-200,display.middle-50,400,100,28,0,"ABCDE");
  display.dlEnd();
  delay(1000);
}

void slider() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  display.slider(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
  display.dlEnd();
  i += 5;
  delay(100);
}

void scrollbar() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  display.scrollbar(20,display.middle,display.width - 50,20,0,i%100,20,100);
  display.dlEnd();
  console.println(i);
  i += 5;
  delay(100);
}

void progress() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  display.progress(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
  display.dlEnd();
  i += 5;
  delay(100);
}

void number() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));
  display.number(display.center,display.middle,4,OPT_CENTER|OPT_SIGNED,i++);
  display.dlEnd();
  delay(100);
}

void buttonSetup() {
  display.dlStart();
  // display.rotate(2);
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0xFFFFFF));

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
      display.tone(2000,20);
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

  display.cmd(COLOR_RGB(0xFF0000));
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
  display.cmd(COLOR_RGB(0xFF0000));
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
  display.gradient(10,10,COLOR_RGB(0xFFFF00), display.width-10,display.height-10,COLOR_RGB(0x00FFFF));
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
  display.cmd(CLEAR_COLOR_RGB(0x000000));
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
  display.cmd(CLEAR_COLOR_RGB(0xFFFFFF)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0x000000));
  display.text(0,0,14,0,"+123.456");
  display.dlEnd();
  delay(100); 
}

void dro() {
  float y;
  float t = millis()/1000000.0;
  display.dlStart();
  display.rotate(2);
  display.cmd(CLEAR_COLOR_RGB(0xFFFFFF)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0x000000));
  display.text(0,0,29,0,"X");
  display.text(0,84,29,0,"Y");
  display.text(0,169,29,0,"Z");

  // X
  y = 999*sin(t);
  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(0xFF0000));
  } else {
    display.cmd(COLOR_RGB(0x000000));
  }
  sprintf(line,"%04.03f",y);
  display.text(370,-15,4,OPT_RIGHTX,line);

  // Y
  y = 999*sin(3 + t);
  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(0xFF0000));
  } else {
    display.cmd(COLOR_RGB(0x000000));
  }
  sprintf(line,"%04.03f",y);
  display.text(370,70,4,OPT_RIGHTX,line);

  // Z 
  y = 999*sin(4 + millis()/10000000.0);
  sprintf(line,"%04.03f",y);
  display.cmd(COLOR_RGB(0xC8C8C8));
  display.text(373,158,4,OPT_RIGHTX,line);

  if (abs(y) > 800.0) {
    display.cmd(COLOR_RGB(0xFF0000));
  } else {
    display.cmd(COLOR_RGB(0x000000));
  }
  display.text(370,155,4,OPT_RIGHTX,line);

  display.dlEnd();
  delay(40);
}


void fontSize() {
  display.dlStart();
  display.rotate(2);
  display.cmd(CLEAR_COLOR_RGB(0xFFFFFF)); 
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0x000000));


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
}


void primitives() {
  display.dlStart();
  display.rotate(2);
  display.clear(0xFFFFFF);
  display.cmd(VERTEX_FORMAT(4)); // 1/16

  // circle
  display.cmd(COLOR_RGB(0x7F2020));
  display.cmd(POINT_SIZE(500));
  display.cmd(BEGIN(POINTS));
  display.cmd(VERTEX2II(192,133,0,0));
  display.cmd(END());

  // letter
  display.cmd(COLOR_RGB(0x7F2020));
  display.cmd(BEGIN(BITMAPS));
  display.cmd(VERTEX2II(220, 110, 4, 'F'));
  display.cmd(END());

  // line
  display.cmd(BEGIN(LINES));
  display.cmd(LINE_WIDTH(3*16));
  display.cmd(VERTEX2II(50, 45, 0, 0));
  display.cmd(VERTEX2II(110, 55, 0, 0));
  display.cmd(END());

  // rect
  display.cmd(COLOR_RGB(0x007700) );
  display.cmd(LINE_WIDTH(16) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F(100 * 16,400 * 16) );
  display.cmd(VERTEX2F(200 * 16,500 * 16) );
  display.cmd(END());

  // round rect
  display.cmd(COLOR_RGB(0x7F0000) );
  display.cmd(LINE_WIDTH(15 * 16) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F((300+15) * 16,(400+15) * 16) );
  display.cmd(VERTEX2F((400-15) * 16,(500-15) * 16) );
  display.cmd(END());

#define BORDER 1
  // rect with border
  display.cmd(LINE_WIDTH(16) );
  display.cmd(COLOR_RGB(0x000000) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F(100 * 16,600 * 16) );
  display.cmd(VERTEX2F(200 * 16,700 * 16) );
  display.cmd(END());
  display.cmd(COLOR_RGB(0xFFFFFF) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F((100+BORDER) * 16,(600+BORDER) * 16) );
  display.cmd(VERTEX2F((200-BORDER) * 16,(700-BORDER) * 16) );
  display.cmd(END());

  // round rect with border
  display.cmd(LINE_WIDTH(15 * 16) );
  display.cmd(COLOR_RGB(0x000000) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F((300+15) * 16,(600+15) * 16) );
  display.cmd(VERTEX2F((400-15) * 16,(700-15) * 16) );
  display.cmd(END());
  display.cmd(COLOR_RGB(0xFFFFFF) );
  display.cmd(BEGIN(RECTS) );
  display.cmd(VERTEX2F((300+15+BORDER) * 16,(600+15+BORDER) * 16) );
  display.cmd(VERTEX2F((400-15-BORDER) * 16,(700-15-BORDER) * 16) );
  display.cmd(END());

  display.dlEnd();
}

  
void navBarAdd() {
  for (uint8_t i = 0; i < (sizeof(navTabEntries)/NAVTABENTRYLEN); i++) {
    if (i == navTabSelected ) {
      display.fgcolor(GREENSTOP);
      display.rgbcolor(WHITE);
    } else {
      display.fgcolor(GREENPALE);
      display.rgbcolor(BLACK);
    }
    display.tag(i+1);
    display.button(3+((3+NAVTABWIDTH)*i),10,NAVTABWIDTH,NAVTABHEIGHT,24,OPT_CENTER,navTabEntries[i]);
  }
}

void navBarLoop() {
  uint32_t now = millis();
  if (now > touchTimeout) {
    uint8_t t = display.touched();
    if (t) {
      navTabSelected = t-1;
      display.tone(2000,20);
      sprintf(line,"%-6d touch %d",now,navTabSelected);
      console.println(line);
      navBarSetup(); 
    }
    touchTimeout = now + 200;
  }
}

void rect() {
  display.dlStart();
  display.rotate(2);
  display.clear(0xFFFFFF);

  //      rect(x,   y,    width, height, borderWidth, borderRadius, borderColor, backgroundColor);

  display.rect(000, 000,  80,    100,    0,           0,            BLACK,       RED);
  display.rect( 80, 100,  80,    100,    2,           0,            BLACK,       WHITE   );
  display.rect(160, 200,  80,    100,    2,           5,            TEAL,        MAROON);
  display.rect(240, 300,  80,    100,    0,           20,           PURPLE,      LIME);
  display.rect(320, 400,  80,    100,    5,           0,            BLACK,       WHITE   );
  display.rect(400, 500,  80,    100,    0,           0,            0x000077,    0xAAAAAA);

  display.dlEnd();
}

void navBarSetup() {
  display.dlStart();
  display.rotate(3);
  display.clear(LTYELLOW);
  navBarAdd();
  display.dlEnd();
}

void statusSetup() {
  display.dlStart();
  display.rotate(3);
  display.clear(LTYELLOW);
  navBarAdd();

  display.rect(5,15+NAVTABHEIGHT,470,730,2,0,BLACK,WHITE);
  display.cmd(COLOR_RGB(BLACK));
  display.text(20,70,30,0,"First Stage Temperature");
  display.text(20,90,4,0,"27"); // bolding
  display.text(21,91,4,0,"27");
  display.text(22,92,4,0,"27");

  t2 += dir*0.01;
  if (t2 < 12.5) dir = 1;
  if (t2 > 18.5) dir = -1;
  sprintf(line,"%d",int(t2));
  display.text(20,200,30,0,"Second Stage Temperature");
  display.text(20,220,4,0,line);
  display.text(21,221,4,0,line);
  display.text(22,222,4,0,line);

  display.text(20,330,30,0,"Pump Status");
  display.text(20,350,4,0,"RUNNING");
  display.text(21,351,4,0,"RUNNING");
  display.text(22,351,4,0,"RUNNING");

  sprintf(line,"%d",39240+millis()/1000);
  display.text(20,460,30,0,"Hour Meter");
  display.text(20,480,4,0,line);
  display.text(21,481,4,0,line);
  display.text(22,482,4,0,line);

  display.text(20,590,30,0,"Hours Since Regeneration");
  //display.text(20,610,4,0,"312");
  display.text(21,611,4,0,"312");
  display.text(21,609,4,0,"312");
  display.text(19,611,4,0,"312");
  display.text(19,609,4,0,"312");

  display.dlEnd();
}

void statusLoop() {
  uint32_t now = millis();
  if (now > touchTimeout) {
    uint8_t t = display.touched();
    if (t) {
      navTabSelected = t-1;
      display.tone(2000,20);
      sprintf(line,"%-6d touch %d",now,navTabSelected);
      console.println(line);
    }
    touchTimeout = now + 200;
    statusSetup(); 
  }
}
