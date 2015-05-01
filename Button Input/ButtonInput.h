#pragma once

#import <mraa.h>
#import <Gpio.h>

class EncoderInput {
private:
    mraa::Gpio* botButt;
    mraa::Gpio* cenButt;
    mraa::Gpio* topButt;

    void pressCallback();
    
    void (*buttonCallback)(unsigned char);

public:
    static unsigned char BOT_BUTT_MASK = 1;
    static unsigned char CEN_BUTT_MASK = 2;
    static unsigned char TOP_BUTT_MASK = 4;

    EncoderInput();
    
    void registerButtonCallback(void (*fptr)(unsigned char));
    
    bool getBotButtonPressed();
    bool getCenButtonPressed();
    bool getTopButtonPressed();
}
