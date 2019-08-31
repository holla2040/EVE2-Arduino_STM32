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
  button();
  //gauge();
}

void button() {
  console.println("button");
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
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
