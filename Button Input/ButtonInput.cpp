#import "ButtonInput.h"

ButtonInput::ButtonInput() {
    enB = new mraa::Gpio(MRAA_INTEL_EDISON_GP46);
    enButt = new mraa::Gpio(MRAA_INTEL_EDISON_GP47);
    enA = new mraa::Gpio(MRAA_INTEL_EDISON_GP48);
    
    enA->dir(mraa::DIR_IN);
    enB->dir(mraa::DIR_IN);
    enButt->dir(mraa::DIR_IN);
    
    turnCallback = NULL;
    buttonCallback = NULL;
    
    enA->isr(mraa::EDGE_BOTH, &pressCallback, NULL);
    enB->isr(mraa::EDGE_BOTH, &pressCallback, NULL);
    enButt->isr(mraa::EDGE_BOTH, &pressCallback, NULL);
    
    prevA = enA->read() == 1;
    prevB = enB->read() == 1;
    //TODO: check for errors
}

void EncoderInput::pressCallback() {
    if(buttonCallback != NULL) {
        unsigned char buttons = 0;
        buttons |= botButt->read();
        buttons |= cenButt->read() << 1;
        buttons |= topButt->read() << 2;
        buttonCallback(buttons);
    }
}

void EncoderInput::registerButtonCallback(void (*fptr)(unsigned char)) {
    buttonCallback = fptr;
}

bool EncoderInput::getBotButtonPressed() {
    return botButt->read() == 1;
}

bool EncoderInput::getCenButtonPressed() {
    return cenButt->read() == 1;
}

bool EncoderInput::getTopButtonPressed() {
    return topButt->read() == 1;
}
