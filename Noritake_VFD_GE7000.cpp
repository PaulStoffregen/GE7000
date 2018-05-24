/*******************************************************
GE7000 Display Command Functions

Refer to the display datasheet for more details on
each command.

Noritake Co., Inc.
Version: 1.0 (ME-N64-0)
Support Contact: support.ele@noritake.com
*******************************************************/

#include <stdlib.h>
#include <string.h>
#include <GE7000_Interface.h>
#include <Noritake_VFD_GE7000.h>
#include <alloca.h>
#include <avr/pgmspace.h>

/**
Send a backspace command to the display.

@return none
*/
void Noritake_VFD_GE7000::GE7000_back() {
    command(0x08);
}

/**
Move the cursor forward one space.

@return none
*/
void Noritake_VFD_GE7000::GE7000_forward() {
    command(0x09);
}

/**
Send a line feed command to the dispaly.

@return none
*/
void Noritake_VFD_GE7000::GE7000_lineFeed() {
    command(0x0a);
}

/**
Move the cursor to the home position (top left).

@return none
*/
void Noritake_VFD_GE7000::GE7000_home() {
    command(0x0b);
}

/**
Send a carriage return command to the display.

@return none
*/
void Noritake_VFD_GE7000::GE7000_carriageReturn() {
    command(0x0d);
}

/**
Set the cursor position.

0x0000 <= x <= 0x01ff
0x0000 <= y <= 0x0007

@param  x    X axis cursor position (1 pixel / unit)
@param  y    Y axis cursor position (8 pixels / unit)
@return none
*/
void Noritake_VFD_GE7000::GE7000_setCursor(unsigned x, unsigned y) {
    command(0x1f);
    command('$');
    command_xy(x, y);
}

/**
Clear the screen and move the cursor to the home position.

@return none
*/
void Noritake_VFD_GE7000::GE7000_clearScreen() {
    command(0x0c);
}

/**
Turn the cursor ON.
When the cursor is on, it will appear as a blinking 1x8 line.

@return none
*/
void Noritake_VFD_GE7000::GE7000_cursorOn() {
    command(0x1f, 'C', 1);
}

/**
Turn the cursor OFF.

@return none
*/
void Noritake_VFD_GE7000::GE7000_cursorOff() {
    command(0x1f, 'C', 0);
}

/**
Initialize the GE7000 display.

The following happens during initialization:
	- Jumper settings are not re-loaded.
	- Contents of receive buffer remain in memory.

@return none
*/
void Noritake_VFD_GE7000::GE7000_init() {
    io->init();
    command(0x1b);
    command('@');
}

/**
Performs a hardware reset on the module.

@return none
*/
void Noritake_VFD_GE7000::GE7000_reset() {
    io->hardReset();
}

/**
Enable or disable the use of multi-byte characters.

enable = 0x00: 2-byte character mode OFF
enable = 0x01: 2-byte character mode ON

@param  enable Enable or disable the use of multi-byte characters.
@return none
*/
void Noritake_VFD_GE7000::GE7000_useMultibyteChars(bool enable) {
    if ((this->modelClass - 7000) / 100 == 9) {
        us_command('g', 0x02);
        command(enable);
    }
}

/**
Select your desired character type to use.

				Character Type   | Code Type |      1st byte      |      2nd byte 
code = 0x00:       Japanese      | JIS X0208 | 0x81 <= c1 <= 0x9f | 0x40 <= c2 <= 0x7e
					             |(SHIFT-JIS)| 0xe0 <= c1 <= 0xef | 0x80 <= c2 <= 0xfc
code = 0x01:        Koream       | KSC5601-87| 0xa1 <= c1 <= 0xfe | 0xa1 <= c2 <= 0xfe
code = 0x02: Simplified Chinese  | GB2312-80 | 0xa1 <= c1 <= 0xfe | 0xa1 <= c2 <= 0xfe
code = 0x03: Traditional Chinese | Big-5     | 0xa1 <= c1 <= 0xfe | 0x40 <= c2 <= 0x7e
																  | 0xa1 <= c2 <= 0xfe

@param  code The desired character type.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setMultibyteCharset(uint8_t code) {
    if ((this->modelClass - 7000) / 100 == 9) {
        us_command('g', 0x0f);
        command(code);
    }
}

/**
Turn custom characters ON or OFF.
Characters already displayed are not affected.

enable = 0x00: Custom character OFF.
	(If a custom character is not defined, built-in characters are used.)
enable = 0x01: Custom character ON.

@param  enable
@return none
*/
void Noritake_VFD_GE7000::GE7000_useCustomChars(bool enable) {
    command(0x1b, '%', enable);
}

/**
Helper function for GE7000_defineCustomChar.

@param  src  The data source.
@param  col  The desired column.
@return none
*/
static inline uint8_t getColumn(const uint8_t *src, int col) {
    uint8_t out = 0;
    for (int i=0; i<8; i++)
        if (src[i] & (1<<(4-col))) out += 1<<(7-i);
    return out;
}

/**
Define a custom character into RAM.

0x20 <= code <= 0xff

@param  code   Desired character type.
@param  format Desired character format.
@param  data   The custom character data.
@return none
*/
void Noritake_VFD_GE7000::GE7000_defineCustomChar(uint8_t code, FontFormat format, const uint8_t *data) {
    command(0x1b, '&', 0x01);
    command(code);
    command(code);
    
    switch (format) {
    case CUUFormat:
        command(5);
        for (uint8_t i=0; i<5; i++)
            command(getColumn(data, i));
        break;
    
    case GU70005x7Format:
        command(5);
        print((const char*)data, 5);
        break;
    case GU70007x8Format:
        command(7);
        print((const char*)data, 7);
        break;
    }
}

/**
Delete a custom character from RAM.

0x20 <= code <= 0xff

@param  code The desired character code to delete.
@return none
*/
void Noritake_VFD_GE7000::GE7000_deleteCustomChar(uint8_t code) {
    command(0x01b, '?', 0x01);
    command(code);
}

/**
Select the desired ASCII variant.
Characters already displayed are not affected.
This function refers to the "7.2.3.6 International font set" command.

Font set codes:
code = 0x00: America
code = 0x01: France
code = 0x02: Germany
code = 0x03: England
code = 0x04: Denmark 1
code = 0x05: Sweden
code = 0x06: Italy
code = 0x07: Spain 1
code = 0x08: Japan
code = 0x09: Norway
code = 0x0a: Denmark 2
code = 0x0b: Spain 2
code = 0x0c: Latin America
code = 0x0d: Korea

@param  code The desird font set to be used.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setAsciiVariant(uint8_t code) {
    if (code < 0x0d)
        command(0x1b, 'R', code);
}

/**
Select the desired table type.
Characters already displayed are not affected.
This function refers to the "7.2.3.7 Character Table type" command.

Character table codes:
code = 0x00: PC437(USA - Euro std)
code = 0x01: Katakana - Japanese
code = 0x02: PC850 (Multilingual)
code = 0x03: PC860 (Portuguese)
code = 0x04: PC863 (Canadian-French)
code = 0x05: PC865 (Nordic)
code = 0x10: WPC1252
code = 0x11: PC866 (Cyrillic #2)
code = 0x12: PC852 (Latin 2)
code = 0x13: PC858

@param  code The desired table type code.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setCharset(uint8_t code) {
    if (code < 0x05 || (0x10 <= code && code <= 0x13))
        command(0x1b, 't', code);
}

/**
Set the desired scroll mode.
This funtion refers to the following commands:
7.2.2.1 Over-write mode
7.2.2.2 Vertical scroll mode
7.2.2.3 Horizontal scroll mode

mode = 0x01: Over-write mode
mode = 0x02: Vertical scroll mode
mode = 0x03: Horizontal scroll mode

@param  mode  The desired scroll mode.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setScrollMode(ScrollMode mode) {
    command(0x1f);
    command(mode);
}

/**
Set the horizontal scroll speed.

speed = 0x00: Instantaneous speed
speed = 0x01: T ms / 2 pixels
speed = 0x02 - 0x1f: (speed - 1) * T ms / pixel

Scroll base period "T" depends on the screen mode, character size, etc.

@param  speed  The desired scroll speed.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setHorizScrollSpeed(uint8_t speed) {
    command(0x1f, 's', speed);
}

/**
Turns invert display mode OFF.

@return none
*/
void Noritake_VFD_GE7000::GE7000_invertOff() {
    command(0x1f, 'r', 0);
}

/**
Turns invert display mode ON.

@return none
*/
void Noritake_VFD_GE7000::GE7000_invertOn() {
    command(0x1f, 'r', 1);
}

/**
Set the desired composition mode.
This function refers to the "7.2.1.6 Write mixture display mode" command.

mode = 0x00: Normal display write (not mixture display)
mode = 0x01: OR display write
mode = 0x02: AND display write
mode = 0x03: XOR display write
Default = 0x00

@param  mode The desired composition mode.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setCompositionMode(CompositionMode mode) {
    command(0x1f, 'w', mode);
}

/**
Set the display's brightness.

level = 0x00: 00.0% brightness
level = 0x01: 12.5% brightness
level = 0x02: 25.0% brightness
level = 0x03: 37.5% brightness
level = 0x04: 50.0% brightness
level = 0x05: 62.5% brightness
level = 0x06: 75.0% brightness
level = 0x07: 87.5% brightness
level = 0x08: 100% brightness
Default = 0x08

@param  level
@return none
*/
void Noritake_VFD_GE7000::GE7000_setScreenBrightness(unsigned level) {
    if (level == 0)
        GE7000_displayOff();
    else if (level <= 100) {
        GE7000_displayOn();
        command(0x1f, 'X', (level*10 + 120)/125);
    }
}

/**
Suspends the program and waits for a user defined amount of time.

@param  time    The amount of time for the MCU to wait.
@return none
*/
void Noritake_VFD_GE7000::GE7000_wait(uint8_t time) {
    us_command('a', 0x01);
    command(time);
}

/**
Scrolls the screen based on certain parameters.

@param  x       The amount of pixels for the screen to scroll horizontally.
@param  y       The amount of pixels for the screen to scroll vertically.
@param times    The number of times the screen will scroll
@param speed    The speed at which scrolling occurs.
@return none
*/
void Noritake_VFD_GE7000::GE7000_scrollScreen(unsigned x, unsigned y, unsigned times, uint8_t speed) {
    unsigned pos = (x*this->lines)+(y/8);
    us_command('a', 0x10);
    command(pos);
    command(pos>>8);
    command(times);
    command(times>>8);
    command(speed);
}

/**
Resets the blink settings on the module to all zeros.

@return none
*/
void Noritake_VFD_GE7000::GE7000_blinkScreen() {
    us_command('a', 0x11);
    command(0);
    command(0);
    command(0);
    command(0);
}

/**
Blinks the GUD series module screen based on user input parameters.

@param  enable   Enables or disables screen blinking.
<ul>
                <li> 0x00 - Normal display
                <li> 0x01 - Blink display (alternatively Normal and Blank display)
                <li> 0x02 - Blink display (alternatively Normal and Reverse display) </ul>
@param  reverse  Enables or disables the reverse blinking pattern.
@param  onTime   The time that the display stays ON during blinking
@param  offTtime The time that the displays stays OFF during blinking.
@return none
*/
void Noritake_VFD_GE7000::GE7000_blinkScreen(bool enable, bool reverse, uint8_t onTime, uint8_t offTime, uint8_t times) {
    us_command('a', 0x11);
    command(enable? (reverse? 2: 1): 0);
    command(onTime);
    command(offTime);
    command(times);
}

/**
Turns tha display OFF.

@return none
*/
void Noritake_VFD_GE7000::GE7000_displayOff() {
    us_command('a', 0x40);
    command(0);
}

/**
Turns tha display ON.

@return none
*/
void Noritake_VFD_GE7000::GE7000_displayOn() {
    us_command('a', 0x40);
    command(0x01);
}

/**
Sets the screen saver mode for the GE7000 series module.

Screen saver modes:
<ul>
<li> 0x00 - Display power OFF
<li> 0x01 - Display power ON
<li> 0x02 - All dots OFF
<li> 0x03 - All dots ON
<li> 0x04 - Repeat blink display with normal and reverse display
</ul>
@param  mode    The desired screensaver mode. (see ScreenSaver)
@return none
*/
void Noritake_VFD_GE7000::GE7000_screenSaver(ScreenSaver mode) {
    us_command('a', 0x40);
    command(mode);
}

/**
Sets the font style for the GE7000 series module.

@param  proportional Enable or disable a proportional font style.
@param  evenSpacing  Enable or disable even spacing between characters.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setFontStyle(bool proportional, bool evenSpacing) {
    us_command('g', 0x03);
    command(proportional*2 + evenSpacing);
}

/**
Sets the font size for the GE7000 series module.

0x01 <= x <= 0x04
0x01 <= y <= 0x02

Default:
x = 0x01
y = 0x01

@param  x    The X size of the font.
@param  y    The Y size of the font.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setFontSize(uint8_t x, uint8_t y, bool tall) {
    if (x<=4 && y<=2) {        
        us_command('g', 0x40);
        command(x);
        command(y);
		
		us_command('g', 0x01);
		command(tall + 1);
    }
}

/**
Selects the current window.

@param  window  The desired window to select. (0-4)
@return none
*/
void Noritake_VFD_GE7000::GE7000_selectWindow(uint8_t window) {
    if (window <= 4)
        command(0x10 + window);
}

/**
Defines a specific window for the GUD series module.

@param  window  The number of the window to be created. (1-4)
@param  x       The x coordinate of the new window.
@param  y       The y coordinate of the new window.
@param  width   The width of the new window.
@param  height  The height of the new window.
@return none
*/
void Noritake_VFD_GE7000::GE7000_defineWindow(uint8_t window, unsigned x, unsigned y, unsigned width, unsigned height) {
    us_command('w', 0x02);
    command(window);
    command(0x01);
    command_xy(x, y);
    command_xy(width, height);
}

/**
Deletes a previously defined window.

@param  window  The number of the window to be deleted. (1-4)
@return none
*/
void Noritake_VFD_GE7000::GE7000_deleteWindow(uint8_t window) {
    us_command('w', 0x02);
    command(window);
    command(0);
    command_xy(0, 0);
    command_xy(0, 0);
}

/**
Joins all of the screens into one base screen.
This function refers to the "5.2.7.1 Write screen mode select" command.

@return none
*/
void Noritake_VFD_GE7000::GE7000_joinScreens() {
    us_command('w', 0x10);
    command(0x01);
}

/**
Separates all of the screens into four separate screens.
This function refers to the "5.2.7.1 Write screen mode select" command.

@return none
*/
void Noritake_VFD_GE7000::GE7000_separateScreens() {
    us_command('w', 0x10);
    command(0);
}

/**
Prints a character to the display.

@param  c     The character to be sent.
@return none
*/
void Noritake_VFD_GE7000::print(char c) {
    command(c);
}

/**
Prints a string of characters to the display.

@param *str  The string to be sent.
@return none
*/
void Noritake_VFD_GE7000::print(const char *str) {
    while (*str)
        io->write(*str++);
}

/**
Prints a character buffer to the display.

@param  *buffer The buffer to be sent.
@param  size    The size of the buffer.
@return none
*/
void Noritake_VFD_GE7000::print(const char *buffer, size_t size) {
    while (size--)
        print(*buffer++);
}

/**
Prints a long value to the display.

@param  number The number to be sent.
@param  base   The desired base of the number.
@return none
*/
void Noritake_VFD_GE7000::print(long number, uint8_t base) {
    if (number < 0) {
        print('-');
        number = -number;
    }
    printNumber(number, base);
}

/**
Prints an integer to the display.

@param  number The number to be sent.
@param  base   The desired base of the number.
@return none
*/
void Noritake_VFD_GE7000::print(int number, uint8_t base) {
    print((long)number, base);
}

/**
Prints an unsigned long to the display.

@param  number The number to be sent.
@param  base   The desired base of the number.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned long number, uint8_t base) {
    printNumber(number, base);
}

/**
Prints an unsigned integer to the display.

@param  number The number to be sent.
@param  base   The desired base of the number.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned number, uint8_t base) {
    print((unsigned long)number, base);
}

/**
Performs a carriage return and line feed on the GE7000 display.

@return none
*/
void Noritake_VFD_GE7000::crlf() {
    GE7000_carriageReturn();
    GE7000_lineFeed();
}

/**
Prints a character to the display and then performs
a carriage return and line feed.

@param  c    The character to be written.
@return none
*/
void Noritake_VFD_GE7000::println(char c) {
    print(c);
    crlf();
}

/**
Prints a string of characters to the display and then
performs a carriage return and line feed.

@param  *str The string of characters to be written.
@return none
*/
void Noritake_VFD_GE7000::println(const char *str) {
    print(str);
    crlf();
}

/**
Prints a buffer of characters to the display and then
performs a carriage return and line feed.

@param  *buffer The character buffer to be written.
@param  size    Size of the character buffer.
@return none
*/
void Noritake_VFD_GE7000::println(const char *buffer, size_t size) {
    print(buffer, size);
    crlf();
}

/**
Prints a specific base long to the display and then performs a carriage
return and line feed.

@param  number The number to be written.
@param  base   The desired number base to be used.
@return none
*/
void Noritake_VFD_GE7000::println(long number, uint8_t base) {
    print(number, base);
    crlf();
}

/**
Prints a specific base number to the display and the performs abort
carriage return and line feed.

@param  number The number to be written.
@param  base   The desired number base to be used.
@return none
*/
void Noritake_VFD_GE7000::println(int number, uint8_t base) {
    println((long) number, base);
}

/**
Prints a specific base unsigned long to the display and then performs a carriage
return and line feed.

@param  number The number to be written.
@param  base   The desired number base to be used.
@return none
*/
void Noritake_VFD_GE7000::println(unsigned long number, uint8_t base) {
    print(number, base);
    crlf();
}

/**
Prints a specific base unsigned number to the display and the performs abort
carriage return and line feed.

@param  number The number to be written.
@param  base   The desired number base to be used.
@return none
*/
void Noritake_VFD_GE7000::println(unsigned number, uint8_t base) {
    println((unsigned long) number, base);
}

/**
Prints a specific base unsigned long to the display and then performs a carriage
return and line feed. This function is recursive.

@param  number The number to be written.
@param  base   The desired number base to be used.
@return none
*/
void Noritake_VFD_GE7000::printNumber(unsigned long number, uint8_t base) {
    if (number/base)
        printNumber(number/base, base);
    number %= base;
    print(number + (number < 10? '0': 'A' - 10));
}

/**
Draws an image to the display.

@param  width  The width of the image to be drawn.
@param  height The height of the image to be drawn.
@param  *data  The data of the image to be drawn.
@return none
*/
void Noritake_VFD_GE7000::GE7000_drawImage(unsigned width, uint8_t height, const uint8_t *data) {
    if (height > this->height) return;
    us_command('f', 0x11);
    command_xy(width, height);
    command((uint8_t) 1);
    for (unsigned i = 0; i<(height/8)*width; i++)
        command(data[i]);
}

/**
Enter "Memory re-write mode" from "normal mode".
DO NOT power ON/OFF during memory re-write mode, this may corrupt FROM data.
FROM re-write cycles are limited, so do not use this command unless absolutely necessary.

@return none
*/
void Noritake_VFD_GE7000::GE7000_memoryRewriteStart(){
	command(0x1c);
	command(0x7c);
	command(0x4d);
	command(0xd0);
	command(0x4d);
	command(0x4f);
	command(0x44);
	command(0x45);
	command(0x49);
	command(0x4e);
}

/**
End "Memory re-write mode" and return to "normal mode".
This command is only valid in "Memory re-write mode".

@return none
*/
void Noritake_VFD_GE7000::GE7000_memoryRewriteEnd(){
	command(0x45);
	command(0xba);
}

/**
Save an image to FROM.

This function deals with each bank of memory in FROM 1 individually.
This means that when writing an image to FROM, the entire bank must be
written to, even if the image is smaller than the bank.

@param  bank    The FROM bank to be written to.
@param  length  The length of the image data array.
@param  data    The image data array.
@return none
*/
void Noritake_VFD_GE7000::GE7000_setFROMImage(uint8_t bank, unsigned long length, const uint8_t *data){
	command(0x42);
	command(0xbd);
	command(bank);
	//Write the image to FROM bank.
	for(unsigned i = 0; i < length; i++){
		command(data[i]);
	}
	//Fill the rest of the FROM bamk with zeros.
	for(unsigned i = 0; i < 32768 - length; i++){
		command(0x00);
	}
}

/**
Draws an image that is stored in FROM.

Only works on the 7933B module.

@param  address The address of the image to be drawn.
@param  srcHeight The height of the defined source image.
@param  width The width of the image in dots (pixels).
@param  height The height of the image in dots (pixels).
@return none
*/
void Noritake_VFD_GE7000::GE7000_drawFROMImage(unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height) {
    if (height > this->height) return;
    us_command('f', 0x10);
    command(0x01);
    command(address);
    command(address>>8);
    command(address>>16);
    command(srcHeight/8);
    command((srcHeight/8)>>8);
    command_xy(width, height);
    command((uint8_t) 1);
}

/**
Draws a filled-in rectangle on the display.

@param  x0   The x coordinate of the top-left corner of the rectangle.
@param  y0   The y coordinate of the top-left corner of the rectangle.
@param  x1   The x coordinate of the bottom-right corner of the rectangle.
@param  y1   The y coordinate of the bottom-right corner of the rectangle.
@param  on   Either enables (logic 1) or disables (logic 0) the rectangle from being drawn.
@return none
*/
void Noritake_VFD_GE7000::GE7000_fillRect(unsigned x0, unsigned y0, unsigned x1, unsigned y1, bool on) {
    x0 = min(this->width, x0);
    x1 = min(this->width, x1);
    y0 = min(this->height, y0);
    y1 = min(this->height, y1);
    if (y1<=y0 || x1<=x0) return;
    uint8_t bufw = 8, bufh = (y1-y0+7)/8*8;
    uint8_t *buf = (uint8_t*)alloca(bufh/8 * bufw);
    for (unsigned x = 0; x < x1-x0; x += bufw) {
        uint8_t part = (x + bufw < x1-x0)? bufw: (x1-x0) - x;
        memset(buf, 0, bufh/8 * bufw);
        if (on)
            for (uint8_t col = 0; col < part; col++) {
                for (uint8_t py = y0 % 8; py < y0 % 8 + min(y1-y0, 8); py++)
                    buf[col*bufh/8] |= 1 << (7-py);
                for (uint8_t row = (y0+7)/8; row < y1/8; row++)
                    buf[row - y0/8 + col*bufh/8] = 0xff;
                if (y0/8 != y1/8)
                    for (uint8_t py = 0; py < y1 % 8; py++)
                        buf[(y1-y0)/8 + col*bufh/8] |= 1 << (7-py);
            }
        GE7000_setCursor(x + x0, y0);
        GE7000_drawImage(bufw, bufh, buf);
    }
}

/**
Sends a byte command to the display.

@param  data The command data to be sent.
@return none
*/
void Noritake_VFD_GE7000::command(uint8_t data) {
    io->write(data);
}

/**
Sends xy coordinates to the display.
Y coordinates are divided into character columns.

@param  x    The desired x coordinate.
@param  y    The desired y coordinate.
@return none
*/
void Noritake_VFD_GE7000::command_xy(unsigned x, unsigned y) {
    command(x);
    command(x>>8);
    y /= 8;
    command(y);
    command(y>>8);
}

/**
Sends xy coordinates to the display.

@param  x    The desired x coordinate.
@param  y    The desired y coordinate.
@return none
*/
void Noritake_VFD_GE7000::command_xy1(unsigned x, unsigned y) {
    command(x);
    command(x>>8);
    command(y);
    command(y>>8);
}

/**
A commonly used command set that is generally called by
other functions. Starts witha unit separator command.

@param  group The command group identifier.
@param  cmd   The desired command.
@return none
*/
void Noritake_VFD_GE7000::us_command(uint8_t group, uint8_t cmd) {
   command(0x1f);
   command(0x28);
   command(group);
   command(cmd);
}

/**
Sends a group of three commands to the display.

@param  prefix Usually a unit separator.
@param  group  The command group identifier
@param  cmd    The desired command.
@return none
*/
void Noritake_VFD_GE7000::command(uint8_t prefix, uint8_t group, uint8_t cmd) {
   command(prefix);
   command(group);
   command(cmd);
}

/**
Prints a buffer of characters to the display at a specific xy coordinate.

@param  x       The desired x coordinate.
@param  y       The desired y coordinate.
@param  *buffer The character buffer to be written.
@param  len     The length of the buffer.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned x, uint8_t y, const char *buffer, uint8_t len) {
    if (this->generation) {
        us_command('d', 0x30);
        command_xy1(x, y);
        command(0);
        command(len);
        while (len--)
            command(*buffer++);
    }
}

/**
Prints a character string to the display at a specific xy coordinate.

@param  x    The desired x coordinate.
@param  y    The desired y coordinate.
@param  *str The string to be written.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned x, uint8_t y, const char *str) {
    if (this->generation) {
        print(x, y, str, strlen(str));
    }
}

/**
Prints a character to the display at a specific xy coordinate.

@param  x    The desired x coordinate.
@param  y    The desired y coordinate.
@param  c    The character to be written.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned x, uint8_t y, char c) {
    if (this->generation) {
        print(x, y, &c, 1);
    }
}

/**
Prints the desired base of a number at a specific xy coordinate.

@param  x      The desired x coordinate.
@param  y      The desired y coordinate.
@param  number The number to be written.
@param  base   The number base to be used.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned x, uint8_t y, int number, uint8_t base) {
    if (this->generation) {
        if (number < 0) {
            print(x, y, '-');
            print(-1, y, (unsigned)-number, base);        
        } else
            print(x, y, (unsigned)number, base);
    }
}

/**
Prints the desired base of an unsigned number at a specific xy coordinate.

@param  x      The desired x coordinate.
@param  y      The desired y coordinate.
@param  number The number to be written.
@param  base   The number base to be used.
@return none
*/
void Noritake_VFD_GE7000::print(unsigned x, uint8_t y, unsigned number, uint8_t base) {
    if (this->generation) {
        char buf[16], *p = buf + sizeof buf;
        do
            *--p = number % base + (number % base < 10? '0': 'A' - 10);
        while (number /= base);
        print(x, y, p, buf + sizeof buf - p);
    }
}

/**
Draws the passed in image on the display.

@param   x      The top-left x coordinate of the image being drawn.
@param   y      The top-left y coordinate of the image being drawn.
@param   width  The width of the image being drawn.
@param   height The height of the image being drawn.
@param   *data  The data of the image being drawn.
@returns none
*/
void Noritake_VFD_GE7000::GE7000_drawImage(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data) {
    if (this->generation) {
        us_command('d', 0x21);
        command_xy1(x, y);
        command_xy1(width, height);
        command(0x01);
        for (unsigned i = 0; i<(height/8)*width; i++)
            command(data[i]);
    }
}

/**
Draws an image on the display.

@param   x         The top-left x coordinate of the image being drawn.
@param   y         The top-left y coordinate of the image being drawn.
@param   area      Indicates where the image will be drawn. Flash or screen.
@param   address   The address of the image to be drawn.
@param   srcHeight The height of the defined image.
@param   width     The width of the image being drawn.
@param   height    The height of the image being drawn.
@param   offsetx   The x coordinate offset needed to draw the image.
@param   offsety   The y coordinate offset needed to draw the image.
@returns none
*/
void Noritake_VFD_GE7000::GE7000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height, unsigned offsetx, unsigned offsety) {
    if (this->generation) {
        if (height > this->height) return;
        us_command('d', 0x20);
        command_xy1(x, y);
        command(area);
        command(address);
        command(address>>8);
        command(address>>16);
        command(srcHeight/8);
        command(srcHeight/8>>8);
        command_xy1(offsetx, offsety);
        command_xy1(width, height);
        command(0x01);
    }
}

/**
Draws an image on the display.

@param   x         The top-left x coordinate of the image being drawn.
@param   y         The top-left y coordinate of the image being drawn.
@param   area      Indicates where the image will be drawn. Flash or screen.
@param   address   The address of the image to be drawn.
@param   width     The width of the image being drawn.
@param   height    The height of the image being drawn.
@returns none
*/
void Noritake_VFD_GE7000::GE7000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, unsigned width, uint8_t height) {
    if (this->generation) {
        GE7000_drawImage(x, y, area, address, (height + 7) & ~7, width, height, 0, 0);
    }
}

/**
Program space print.

@param  *str The string of characters to be written.
@return none
*/
void Noritake_VFD_GE7000::print_p(const char *str) {
    while (pgm_read_byte(str))
        io->write(pgm_read_byte(str++));
}

/**
Program space print.

@param  x       The desired x coordinate.
@param  y       The desired y coordinate.
@param  *buffer The character buffer to be written.
@param  len     The length of the character buffer.
@return none
*/
void Noritake_VFD_GE7000::print_p(unsigned x, uint8_t y, const char *buffer, uint8_t len) {
    if (this->generation) {
        us_command('d', 0x30);
        command_xy1(x, y);
        command(0);
        command(len);
        while (len--)
            command(pgm_read_byte(buffer++));
    }
}

/**
Program space print.

@param  x    The desired x coordinate.
@param  y    The desired y coordinate.
@param  *str The character string to be written.
@return none
*/
void Noritake_VFD_GE7000::print_p(unsigned x, uint8_t y, const char *str) {
    if (this->generation) {
        const char *end = str;
        while (pgm_read_byte(end)) end++;
        print_p(x, y, str, end - str);
    }
}

/**
Program space image draw.

@param width The width of the image.
@param height The height of the image.
@param *data The data of the image to be drawn.
*/
void Noritake_VFD_GE7000::GE7000_drawImage_p(unsigned width, uint8_t height, const uint8_t *data) {
    if (height > this->height) return;
    us_command('f', 0x11);
    command_xy(width, height);
    command((uint8_t) 1);
    for (unsigned i = 0; i<(height/8)*width; i++)
        command(pgm_read_byte(data+i));
}

/**
Program space image draw.

@param  x      X coordinate for the image.
@param  y      Y coordinate for the image.
@param  width  Width of the image being drawn.
@param  height Height of the iamge beight drawn.
@param  data   Image data array
@return none
*/
void Noritake_VFD_GE7000::GE7000_drawImage_p(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data) {
    if (this->generation) {
        us_command('d', 0x21);
        command_xy1(x, y);
        command_xy1(width, height);
        command(0x01);
        for (unsigned i = 0; i<(height/8)*width; i++)
            command(pgm_read_byte(data+i));
    }
}

/**
Set the display's screen orientation to normal or rotated 180 degrees.

degrees = 0x00: Normal orientation
degrees = 0x01: 180 degree rotation
default is 0x00.

@param  degrees The desired display orientation.
@return none
*/
void Noritake_VFD_GE7000::GE7000_displayRotate(uint8_t degrees){
	if(this->modelClass == 7933){
		//Rotate display
		command(0x1f);
		command(0x28);
		command(0x77);
		command(0x80);
		command(degrees);
	}
	
}