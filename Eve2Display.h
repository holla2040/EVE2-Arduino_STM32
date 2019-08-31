#ifndef _EVE2DISPLAY_H
#define _EVE2DISPLAY_H

#include <stdint.h>
#include "eve2.h"

enum {SILENT,FATAL,ERROR,WARNING,INFO,DEBUG,DEBUG1,DEBUG2,VERBOSE};

class Eve2Display {
  public:  
    Eve2Display(int cs, int pdn, int audio); 
    void begin();
    void hostCommand(uint8_t command);
    void dlStart();
    void dlEnd();
    void cmd(uint32_t command);

    void dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val);
    void text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str);
    void romfont(uint32_t font, uint32_t romslot);

    void test();
    void printRAM_DL();
    void printRAM_CMD(uint32_t address, uint16_t length);
    void printCommands();
    void log(uint8_t level, char *msg, uint32_t v);

  private:
    int pinCS;
    int pinPDN;
    int pinAUDIO;
    int pinMOSI;
    int pinMISO;
    int pinCLK;

    uint8_t logLevel;

    uint8_t rd8(uint32_t address);
    uint16_t rd16(uint32_t address);
    uint32_t rd32(uint32_t address);

    void wr8(uint32_t address, uint8_t parameter);
    void wr16(uint32_t address, uint16_t parameter);
    void wr32(uint32_t address, uint32_t parameter);
    void spiEnable();
    void spiDisable();
    uint32_t  commands[4096/8]; // buffer for commands
    uint16_t  commandIndex;     // index to next write location
    uint16_t  ramCommandOffset; // this is in bytes on 4 byte boundary
        // offset from RAM_CMD where next command is written 
        // after dlswap is sent, REG_CMD_WRITE is set to this, co-processor starts processing commands
};

#endif
