/*******************************************************
Establishes the SPI interface with the GE7000 module.

Noritake Co., Inc.
Version: 1.0 (ME-N64-0)
Support Contact: support.ele@noritake.com
*******************************************************/

class GE7000_Serial_SPI : public GE7000_Interface {

protected:
    unsigned OUT_PIN:4;
    unsigned BUSY_PIN:4;
    unsigned SCK_PIN:4;
    unsigned RESET_PIN:4;
    unsigned CS_PIN:4;
    
public:
    GE7000_Serial_SPI(uint8_t out, uint8_t busy, uint8_t sck, uint8_t reset, uint8_t cs):
        OUT_PIN(out), BUSY_PIN(busy), SCK_PIN(sck), RESET_PIN(reset), CS_PIN(cs)
    {
    }
    
    void init() {
    	LOWER(OUT);
    	RAISE(SCK);
        RAISE(RESET);
        RAISE(CS);
    	DIRECTION(CS, 1);
    	DIRECTION(RESET, 1);
    	DIRECTION(OUT, 1);
    	DIRECTION(SCK, 1);
    	DIRECTION(BUSY, 0);
    }
    
    void write(uint8_t data) {
        LOWER(CS);
        while (CHECK(BUSY));
    	for (uint8_t i = 0x80; i; i >>= 1) {
    		LOWER(SCK);
    		_delay_us(1);
    		if (data & i) RAISE(OUT); else LOWER(OUT);
    		RAISE(SCK);
    		_delay_us(1);
    	}
    	_delay_us(17);
        RAISE(CS);
    }
    
    void hardReset() {
        init();
        LOWER(CS);
        LOWER(RESET);
    	_delay_ms(1);
    	RAISE(RESET);
        _delay_ms(100);
    	RAISE(CS);
    }
};