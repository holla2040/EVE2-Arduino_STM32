#include <stdio.h>
#include <stdint.h>
#include "Eve2Display.h"

#define console Serial1

// Eve2Display(int cs, int pdn, int audio); 
Eve2Display display(PA4,PA3,PA2);

void setup() {
  console.begin(115200);
  console.println("\nEVE2-Arduino_STM32 setup");

  display.begin();
}

void loop() {
}

