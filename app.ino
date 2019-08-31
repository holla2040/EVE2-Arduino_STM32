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

uint16_t i;
void loop() {
  char line[100];
  uint32_t s = micros();
  display.dlStart();
  display.cmd(CLEAR(1,1,1));
  display.cmd(COLOR_RGB(0,255,0));
  display.dial(150,150,100,0,i);
  display.cmd(COLOR_RGB(255,255,255));
  sprintf(line,"count=%u",i);
  display.text(0,0,1,0,line);
  display.text(200,20,21,0,line);
  display.dlEnd();

  console.println(micros() - s);
  i += 1 << 10;
  delay(20);
}

