#import "EncoderInput.h"

EncoderInput::EncoderInput() {
    enB = new mraa::Gpio(MRAA_INTEL_EDISON_GP46);
    enButt = new mraa::Gpio(MRAA_INTEL_EDISON_GP47);
    enA = new mraa::Gpio(MRAA_INTEL_EDISON_GP48);
    
    enA->dir(mraa::DIR_IN);
    enB->dir(mraa::DIR_IN);
    enButt->dir(mraa::DIR_IN);
    
    turnCallback = NULL;
    buttonCallback = NULL;
    
    enA->isr(mraa::EDGE_BOTH, &encoderTurnCallback, NULL);
    enB->isr(mraa::EDGE_BOTH, &encoderTurnCallback, NULL);
    enButt->isr(mraa::EDGE_BOTH, &encoderPressCallback, NULL);
    
    prevA = enA->read() == 1;
    prevB = enB->read() == 1;
    //TODO: check for errors
}

void EncoderInput::turnCallback() {
    if(turnCallback != NULL) {
        bool curA, curB;
        curA = enA->read() == 1;
        curB = enB->read() == 1;
        
        if((prevA && prevB && !curB) || (prevA && !prevB && !curA) || (!prevA && prevB && curA) || (!prevA && !prevB && curB)) {
            turnCallback(CW);
        } else {
            turnCallback(CCW);
        }
        
        prevA = curA;
        prevB = curB;
    }
}

void EncoderInput::pressCallback() {
    if(buttonCallback != NULL) {
        buttonCallback(enButt->read() == 1 ? PRESSED : RELEASED);
    }
}

void EncoderInput::registerTurnCallback(void (*fptr)(EncoderInput::EncoderDir)) {
    turnCallback = fptr;
}

void EncoderInput::registerPressCallback(void (*fptr)(EncoderInput::ButtonPress)) {
    buttonCallback = fptr;
}

EncoderInput::ButtonPress EncoderInput::getButtonState() {
    return enButt->read() == 1 ? PRESSED : RELEASED;
}
