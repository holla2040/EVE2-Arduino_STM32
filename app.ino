#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

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
  display.dlEnd();
}

void loop() {
  keys();
  //clock();
  //scrollbar();
  //progress();
  //toggle();

  //slider();
  //number();
  //button();
  //gauge();
}

void toggle() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.toggle(display.middle,display.width,100,20,OPT_CENTER,(i++)&0x01,"no\xffyes");
  display.dlEnd();
  delay(1000);
}

void keys() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.toggle(display.middle,display.width,100,20,OPT_CENTER,(i++)%5,"ABCDE");
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
  display.scrollbar(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
  display.dlEnd();
  i += 5;
  delay(100);
}

void progress() {
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255, 255, 255));
  display.slider(20,display.middle,display.width - 50,20,OPT_CENTER,i%100,100);
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
  display.clock(display.center,display.middle,125,0,h,m,s,0);
  display.dlEnd();

  h += 1;
  h %= 12;
  m += 2;
  m %= 60;
  s += 3;
  s %= 60;

  delay(100);
}
