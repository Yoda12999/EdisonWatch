#ifndef encoderinput_h
#define encoderinput_h

#import <mraa.h>
#import <Gpio.h>

class EncoderInput {
private:
    mraa::Gpio* enA;
    mraa::Gpio* enB;
    mraa::Gpio* enButt;
    
    bool prevA, prevB;
    
    void turnCallback();
    void pressCallback();
    
    void (*turnCallback)(EncoderDir ed);
    void (*buttonCallback)(ButtonPress bp);

public:
    enum EncoderDir {
        CW,
        CCW
    }
    
    enum ButtonPress {
        PRESSED,
        RELEASED
    }

    EncoderInput();
    void registerTurnCallback(void (*fptr)(EncoderDir));
    void registerPressCallback(void (*fptr)(ButtonPress));
    ButtonPress getButtonState();
}

#endif
