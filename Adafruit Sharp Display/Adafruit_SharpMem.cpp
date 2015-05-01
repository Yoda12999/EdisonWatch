/*********************************************************************
This is an Edison library for Sharp Monochrome displays. It is based
on an Arduino library provided by Adafruit.

Written by Limor Fried/Ladyada for Adafruit Industries.
Modified by Gabriel "Yodal" Smith for Edison.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include "Adafruit_SharpMem.h"

/**************************************************************************
    Sharp Memory Display Connector
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      7   VDD             3.3-5.0V (into LDO supply)
      6   VDDA            3.3V out
      9   VSS
      10  VSSA
      1   SCLK            Serial Clock
      2   MOSI            Serial Data Input
      3   CS              Serial Chip Select
      8   EXTMODE         COM Inversion Select (Low = SW clock/serial)
      4   EXTCOMIN        External COM Inversion Signal
      5   DISP            Display On(High)/Off(Low)

 **************************************************************************/

#define SHARPMEM_BIT_WRITECMD   (0x01)
#define SHARPMEM_BIT_VCOM       (0x02)
#define SHARPMEM_BIT_CLEAR      (0x04)
#define TOGGLE_VCOM             do { _sharpmem_vcom = _sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM; } while(0);

byte displayBuffer[(SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8];
byte changedLines[SHARPMEM_LCDHEIGHT / 8];

byte spiBuffer[((SHARPMEM_LCDWIDTH + 16) * SHARPMEM_LCDHEIGHT + 16) / 8];

/* ************* */
/* CONSTRUCTORS  */
/* ************* */
Adafruit_SharpMem::Adafruit_SharpMem(uint8_t bus) :
    Adafruit_GFX(SHARPMEM_LCDWIDTH, SHARPMEM_LCDHEIGHT) {

    spi = new mraa::Spi(bus);
    spi->lsbmode(true);
    spi-frequency(1000000);

    // // Set the vcom bit to a defined state
    _sharpmem_vcom = SHARPMEM_BIT_VCOM;
}

void Adafruit_SharpMem::begin() {
    setRotation(2);
}

/* ************** */
/* PUBLIC METHODS */
/* ************** */

/**************************************************************************/
/*!
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
*/
/**************************************************************************/
void Adafruit_SharpMem::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if((x >= SHARPMEM_LCDWIDTH) || (y >= SHARPMEM_LCDHEIGHT)) {
        return;
    }

    if(color) {
        displayBuffer[(y * SHARPMEM_LCDWIDTH + x) / 8] |= (128 >> x % 8);
    } else {
        displayBuffer[(y * SHARPMEM_LCDWIDTH + x) / 8] &= ~(128 >> x % 8);
    }
    
    changedLines[y / 8] |= (1 << y % 8)
}

/**************************************************************************/
/*!
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)

    @return     1 if the pixel is enabled, 0 if disabled
*/
/**************************************************************************/
uint8_t Adafruit_SharpMem::getPixel(uint16_t x, uint16_t y) {
    if((x >= SHARPMEM_LCDWIDTH) || (y >= SHARPMEM_LCDHEIGHT)) {
        return 0;
    }
    return displayBuffer[(y * SHARPMEM_LCDWIDTH + x) / 8] & (128 >> x % 8) ? 1 : 0;
}

/**************************************************************************/
/*!
    @brief Clears the screen
*/
/**************************************************************************/
void Adafruit_SharpMem::clearDisplay() {
    memset(displayBuffer, 0xff, (SHARPMEM_LCDWIDTH * SHARPMEM_LCDHEIGHT) / 8);
    memset(changedLines, 0x00, SHARPMEM_LCDHEIGHT)/ 8);
    // Send the clear screen command rather than doing a HW refresh (quicker)
    spiBuffer[0] = _sharpmem_vcom | SHARPMEM_BIT_CLEAR;
    spiBuffer[1] = 0x00;
    spi->write(spiBuffer, 2);
    TOGGLE_VCOM;
}

/**************************************************************************/
/*!
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void Adafruit_SharpMem::refresh(void) {
    uint16_t spiBufLen = 0;

    spiBuffer[spiBufLen++] = SHARPMEM_BIT_WRITECMD | _sharpmem_vcom;
    TOGGLE_VCOM;

    for(uint16_t i = 0; i < SHARPMEM_LCDHEIGHT; i++) {
        if(changedLines) {
            spiBuffer[spiBufLen++] = i;
            memcpy(spiBuffer + spiBufLen, displayBuffer + i * SHARPMEM_LCDHEIGHT, SHARPMEM_LCDWIDTH / 8);
            spiBufLen += SHARPMEM_LCDWIDTH / 8;
            spiBuffer[spiBufLen++] = 0x00;
        }
    }
    spiBuffer[spiBufLen++] = 0x00;
    
    spi->write(spiBuffer, spiBufLen);
    
    memset(changedLines, 0x00, SHARPMEM_LCDHEIGHT)/ 8);
}
