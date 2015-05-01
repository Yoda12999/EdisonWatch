/*********************************************************************
This is an Edison library for Sharp Monochrome displays. It is based
on an Arduino library provided by Adafruit.

Written by Limor Fried/Ladyada for Adafruit Industries.
Modified by Gabriel "Yodal" Smith for Edison.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#pragma once

#include <Adafruit_GFX.h>
#include <types.h>
#include <spi.h>

// LCD Dimensions
#define SHARPMEM_LCDWIDTH       (128)
#define SHARPMEM_LCDHEIGHT      (128)

class Adafruit_SharpMem : public Adafruit_GFX {
public:
    Adafruit_SharpMem(uint8_t bus);
    void begin(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint8_t getPixel(uint16_t x, uint16_t y);
    void clearDisplay();
    void refresh(void);

private:
    mraa:Spi* spi;
    
    uint8_t _sharpmem_vcom;

    void sendByte(uint8_t data);
};
