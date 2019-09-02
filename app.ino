#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

#include "font.h"

#define console Serial1

int inc = 2;
int i = 0;
char line[100];

// Eve2Display(int cs, int pdn, int audio); 
Eve2Display display(PA4,PA3,PA2);

void setup() {
  console.begin(115200);
  console.println("\x1B[2J\x1b[H");
  console.println("\nEVE2-Arduino_STM32 setup");
  display.begin();

  display.dlStart();
  display.romfont(1,31);
  display.romfont(2,32);
  display.romfont(3,33);
  display.romfont(4,34);
  display.fgcolor(0xFF0000);
  display.bgcolor(0x001100);
  display.dlEnd();
  fontLoad();
}

void loop() {
  //spinner();
  //gradient();
  //keys();
  clock();
  //scrollbar();
  //progress();
  //toggle();

  //slider();
  //number();
  //button();
  //gauge();
  //text();
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

void button() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.button(display.center,display.middle,150,50,31,OPT_CENTER,"button");
  display.dlEnd();
  delay(1000);
}

void gauge() {
  sprintf(line,"%d",i);

  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.text(display.center,display.height - 40,3,OPT_CENTER,line);
  display.gauge(display.center,display.middle,display.height/1.9,OPT_FLAT|OPT_NOBACK|OPT_NOPOINTER,10,10,i,100);

  display.cmd(COLOR_RGB(255, 0, 0));
  display.gauge(display.center,display.middle,display.height/1.9,OPT_FLAT|OPT_NOBACK|OPT_NOTICKS,10,10,i,100);

  display.dlEnd();

  i += inc;
  if ((i > 100) || (i < 0)) {
    inc = -inc;
    i += inc;
    delay(300);
  }

  delay(20);
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
  delay(47);
}

void fontLoad() {
  console.println(RAM_G+1000);
  display.loadRAM(RAM_G + 1000,ibm_plex_mono_semibold_52_metric,148);
  display.loadRAM(RAM_G + 1000 + 148,ibm_plex_mono_semibold_52_data,sizeof(ibm_plex_mono_semibold_52_data));
  display.dlStart();
  display.setbitmap(RAM_G + 1000,L1,32,55);
  display.setfont(1,RAM_G + 1000);
  display.dlEnd();
};

