#ifndef _EVE2DISPLAY_H
#define _EVE2DISPLAY_H

#include <stdint.h>
#include "eve2.h"

/*
Widget          CMD_FGCOLOR CMD_BGCOLOR COLOR_RGB
CMD_TEXT        NO          NO          YES
CMD_BUTTON      YES         NO          YES(label)
CMD_GAUGE       NO          YES         YES(needle and mark)
CMD_KEYS        YES         NO          YES(text)
CMD_PROGRESS    NO          YES         YES
CMD_SCROLLBAR   YES(Inner)  YES(Outer)  NO
CMD_SLIDER      YES(Knob)   YES(Right)  YES(Left)
CMD_DIAL        YES(Knob)   NO          YES(Marker)
CMD_TOGGLE      YES(Knob)   YES(Bar)    YES(Text)
CMD_NUMBER      NO          NO          YES
CMD_CALIBRATE   YES(Adot)   YES(Oudot)  NO
CMD_SPINNER     NO          NO          YES
*/

#define BLACK   0x000000
#define GRAY    0x808080
#define LTGRAY  0xD3D3D3
#define SILVER  0xC0C0C0
#define WHITE   0xFFFFFF
#define MAROON  0x800000
#define RED     0xFF0000
#define OLIVE   0x808000
#define YELLOW  0xFFFF00
#define LTYELLOW 0xFBEDCB
#define GREEN   0x008000
#define GREENSTOP  0x33A532
#define GREENPALE  0xD9FFB3
#define LTGREEN 0x00F000
#define LIME    0x00FF00
#define TEAL    0x008080
#define AQUA    0x00FFFF
#define NAVY    0x000080
#define BLUE    0x0000FF
#define PURPLE  0x800080
#define FUCHSIA 0xFF00FF


enum {SILENT,FATAL,ERROR,WARNING,INFO,DEBUG,DEBUG1,DEBUG2,VERBOSE};

class Eve2Display {
  public:  
    Eve2Display(int cs, int pdn, int audio); 
    uint16_t center, middle;
    uint16_t height, width;
    void begin();
    void hostCommand(uint8_t command);
    void dlStart();
    void dlEnd();
    void cmd(uint32_t command);
    void calibrate();

    /* debug */
    void test();
    void printRAM_DL();
    void printRAM(uint32_t address, uint32_t length);
    void printCommands();
    void log(uint8_t level, char *msg, uint32_t v);

    /* memory */
    void loadRAM(uint32_t dst, uint8_t *src, uint32_t len);
    void setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height);
    void setfont(uint32_t font, uint32_t ptr);
    void setfont2(uint32_t font, uint32_t ptr,uint32_t firstchar);
    void bitmaphandle(uint8_t handle);
    void bitmapsource(uint32_t addr);
    void bitmaplayout(uint8_t format, uint16_t stride, uint16_t height);
    void bitmapsize(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height);

    /* widgets */
    void romfont(uint32_t font, uint32_t romslot);
    void loadimage(uint32_t ptr, uint32_t options);
    void drawramimage(uint32_t ptr, uint32_t options, uint16_t x, uint16_t y);
    void clear(uint32_t color);
    void fgcolor(uint32_t color);
    void bgcolor(uint32_t color);
    void rgbcolor(uint32_t color);
    void circle(uint16_t x, uint16_t y, uint16_t r, uint8_t bordersize, uint32_t bordercolor, uint8_t filled);
    void gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);
    void dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val);
    void text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str);
    void button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str);
    void number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num);
    void slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range);
    void toggle(uint16_t x, uint16_t y, uint16_t w, uint16_t font, uint16_t options, uint16_t state,const char *str);
    void progress(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range);
    void scrollbar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t size,uint16_t range);
    void keys(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str);
    void clock(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms);
    void gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1);
    void spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale);

    void tag(uint8_t i);
    uint8_t touched();
    void rotate(uint32_t r);

    void rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t borderWidth, uint8_t borderRadius, uint32_t borderColor, uint32_t backgroundColor);

    void spiEnable();
    void spiDisable();

  private:
    int pinCS;
    int pinPDN;
    int pinINT;

    uint8_t logLevel;

    void cmdString(const char *str);
    uint8_t displayListInProgress;

    void wr8(uint32_t address, uint8_t parameter);
    void wr16(uint32_t address, uint16_t parameter);
    void wr32(uint32_t address, uint32_t parameter);
    uint8_t rd8(uint32_t address);
    uint16_t rd16(uint32_t address);
    uint32_t rd32(uint32_t address);
    uint32_t  commands[4096/8]; // buffer for commands
    uint16_t  commandIndex;     // index to next write location
    uint16_t  ramCommandOffset; // this is in bytes on 4 byte boundary
        // offset from RAM_CMD where next command is written 
        // after dlswap is sent, REG_CMD_WRITE is set to this, co-processor starts processing commands
    void capTouchUpload(void);
    void coProWrCmdBuf(const uint8_t *buff, uint32_t count);
    void wait4CoProFIFO(uint32_t room);
    uint16_t CoProFIFO_FreeSpace(void);
};

#endif
