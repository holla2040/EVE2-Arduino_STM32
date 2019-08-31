#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

#define console Serial1

// Eve2Display(int cs, int pdn, int audio); 
Eve2Display display(PA4,PA3,PA2);

void setup() {
  console.begin(115200);
  console.println("\x1B[2J\x1b[H");
  console.println("\nEVE2-Arduino_STM32 setup");

  display.begin();
  display.dlStart();
  display.romfont(1,34);
  display.dlEnd();
}

int inc = 3;
int i = inc;
void loop() {
  char line[100];
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(255,0,0));
  display.gauge(display.center,display.middle,display.height/1.9,OPT_FLAT|OPT_NOBACK,10,10,i,100);
  display.dlEnd();

  if ((i > 100) || (i < 1)) inc = -inc;
  i += inc;

  delay(20);
}

