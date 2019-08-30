#ifndef _EVE2DISPLAY_H
#define _EVE2DISPLAY_H

#include <stdint.h>
#include "eve2.h"

class Eve2Display {
  public:  
    Eve2Display(int cs, int pdn, int audio); 
    void begin();
    void hostCommand(uint8_t command);

  private:
    int pinCS;
    int pinPDN;
    int pinAUDIO;
    int pinMOSI;
    int pinMISO;
    int pinCLK;

    uint8_t rd8(uint32_t address);
    uint16_t rd16(uint32_t address);
    uint32_t rd32(uint32_t address);

    void wr8(uint32_t address, uint8_t parameter);
    void wr16(uint32_t address, uint16_t parameter);
    void wr32(uint32_t address, uint32_t parameter);
    void spiEnable();
    void spiDisable();
};

#endif
