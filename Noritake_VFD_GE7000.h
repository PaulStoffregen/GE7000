/*******************************************************
GE7000 Display Command Function Prototypes

Noritake Co., Inc.
Version: 1.0 (ME-N64-0)
Support Contact: support.ele@noritake.com
*******************************************************/

#ifndef NORITAKE_VFD_GE7000_H
#define NORITAKE_VFD_GE7000_H

#include <stdint.h>
#include <stddef.h>
#include <util/delay.h>

enum ImageMemoryArea {
    FlashImageArea = 1,
    ScreenImageArea = 2
};

enum ScrollMode {
    WrappingMode =    1,
    VertScrollMode =  2,
    HorizScrollMode = 3
};

enum CompositionMode {
    NormalCompositionMode = 0,
    OrCompositionMode =     1,
    AndCompositionMode =    2,
    XorCompositionMode =    3
};

enum ScreenSaver {
    AllDotsOffSaver = 2,
    AllDotsOnSaver =  3,
    InvertSaver =   4
};

enum LEDColor {
    NoLight =       0x000,
    BlueLight =     0x00f,
    GreenLight =    0x0f0,
    CyanLight =     0x0ff,
    RedLight =      0xf00,
    MagentaLight =  0xf0f,
    SmokeLight =    0xfff
};

enum FontFormat {
     GU70005x7Format =0,
     GU70007x8Format =1,
     CUUFormat =    2,
     LCDFormat =    CUUFormat     
};

class Noritake_VFD_GE7000 {

    void initialState();
    void printNumber(unsigned long number, uint8_t base);
    void printNumber(unsigned x, uint8_t y, unsigned long number, uint8_t base);
    void command(uint8_t data);
    void us_command(uint8_t group, uint8_t cmd);
    void command(uint8_t prefix, uint8_t group, uint8_t cmd);
    void command_xy(unsigned x, unsigned y);
    void command_xy1(unsigned x, unsigned y);
    void crlf();
    
    GE7000_Interface *io;    

public:
    uint8_t width;
    uint8_t height;
    uint8_t lines;
    unsigned modelClass;
    bool	generation; // GU-7***B = true
   

    Noritake_VFD_GE7000() {
        this->modelClass = 7000;
        this->generation = false;
    }

    void interface(GE7000_Interface &interface) {
        io = &interface;
		//io->init();
    }
    
    void begin(uint8_t width, uint8_t height) {
        this->width = width;
        this->height = height;
        this->lines = this->height / 8;
    }
    
    void isModelClass(unsigned modelClass) {
        this->modelClass = modelClass;
		io->getModelClass = modelClass;
    }

    
    void isGeneration(char c) {
        this->generation = toupper(c) == 'B';
    }

    void GE7000_back();
    void GE7000_forward();
    void GE7000_lineFeed();
    void GE7000_home();
    void GE7000_carriageReturn();
    void GE7000_setCursor(unsigned x, unsigned y);
    void GE7000_clearScreen();
    void GE7000_cursorOn();
    void GE7000_cursorOff();
    void GE7000_init();
    void GE7000_reset();
    void GE7000_useMultibyteChars(bool enable);
    void GE7000_setMultibyteCharset(uint8_t code);
    void GE7000_useCustomChars(bool enable);
    void GE7000_defineCustomChar(uint8_t code, FontFormat format, const uint8_t *data);
    void GE7000_deleteCustomChar(uint8_t code);
    void GE7000_setAsciiVariant(uint8_t code);
    void GE7000_setCharset(uint8_t code);
    void GE7000_setScrollMode(ScrollMode mode);
    void GE7000_setHorizScrollSpeed(uint8_t speed);
    void GE7000_invertOn();
    void GE7000_invertOff();
    void GE7000_setCompositionMode(CompositionMode mode);
    void GE7000_setScreenBrightness(unsigned level);
    void GE7000_wait(uint8_t time);
    void GE7000_scrollScreen(unsigned x, unsigned y, unsigned count, uint8_t speed);
    void GE7000_blinkScreen();
    void GE7000_blinkScreen(bool enable, bool reverse, uint8_t on, uint8_t off, uint8_t times);
    void GE7000_displayOn();
    void GE7000_displayOff();
    void GE7000_screenSaver(ScreenSaver mode);
    void GE7000_drawImage(unsigned width, uint8_t height, const uint8_t *data);
	void GE7000_memoryRewriteStart();
	void GE7000_memoryRewriteEnd();
	void GE7000_setFROMImage(uint8_t bank, unsigned long length, const uint8_t *data);
    void GE7000_drawFROMImage(unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height);
    void GE7000_setFontStyle(bool proportional, bool evenSpacing);
    void GE7000_setFontSize(uint8_t x, uint8_t y, bool tall);
    void GE7000_selectWindow(uint8_t window);
    void GE7000_defineWindow(uint8_t window, unsigned x, unsigned y, unsigned width, unsigned height);
    void GE7000_deleteWindow(uint8_t window);
    void GE7000_joinScreens();
    void GE7000_separateScreens();

    void print(char c);
    void print(const char *str);
    void print(const char *buffer, size_t size);
    void print(int number, uint8_t base);
    void print(unsigned number, uint8_t base);
    void print(long number, uint8_t base);
    void print(unsigned long number, uint8_t base);
    void println(char c);
    void println(const char *str);
    void println(const char *buffer, size_t size);
    void println(int number, uint8_t base);
    void println(unsigned number, uint8_t base);
    void println(long number, uint8_t base);
    void println(unsigned long number, uint8_t base);

    void GE7000_fillRect(unsigned x0, unsigned y0, unsigned x1, unsigned y1, bool on=true);
    
    void print(unsigned x, uint8_t y, char c);
    void print(unsigned x, uint8_t y, const char *str);
    void print(unsigned x, uint8_t y, const char *buffer, uint8_t len);
    void print(unsigned x, uint8_t y, int number, uint8_t base);
    void print(unsigned x, uint8_t y, unsigned number, uint8_t base);
    void GE7000_drawImage(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data);
    void GE7000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height, unsigned offsetx, unsigned offsety);
    void GE7000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, unsigned width, uint8_t height);
    
    void print_p(const char *str);
    void print_p(unsigned x, uint8_t y, const char *str);
    void print_p(unsigned x, uint8_t y, const char *buffer, uint8_t len);
    void GE7000_drawImage_p(unsigned width, uint8_t height, const uint8_t *data);
    void GE7000_drawImage_p(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data);
	void GE7000_displayRotate(uint8_t degrees);
};
#endif
