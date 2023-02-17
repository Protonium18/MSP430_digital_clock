
#pragma once
#ifndef TM1637_H_
#define TM1637_H_
#include "seriallib.h"

#define NUM_0 0x3F
#define NUM_1 0x06
#define NUM_2 0x5B
#define NUM_3 0x4F
#define NUM_4 0x66
#define NUM_5 0x6D
#define NUM_6 0x7D
#define NUM_7 0x07
#define NUM_8 0x7F
#define NUM_9 0x6F
#define NUM_10 0x77
#define NUM_11 0x7C
#define NUM_12 0x39
#define NUM_13 0x5E
#define NUM_14 0x79
#define NUM_15 0x71

#define DIGIT_0 0xC0
#define DIGIT_1 0xC1
#define DIGIT_2 0xC2
#define DIGIT_3 0xC3

#define CMD_SCREEN_ON 0x88
#define CMD_SCREEN_OFF 0x80
#define CMD_BRIGHTNESS_MAX 0x8F
#define CMD_DATA_WRITE 0x40

static uint8_t TM1637_NUMBERS[16] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_10, NUM_11, NUM_12, NUM_13, NUM_14, NUM_15};
static uint8_t TM1637_DIGITS[4] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3};

class TM1637 : public serialobj{

    public:
        TM1637(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT);
        uint8_t singleWrite(uint8_t position, uint8_t number, uint8_t hexadecimal = 0, uint8_t colon = 0);
        uint8_t doubleWrite(uint8_t position, uint8_t number, uint8_t colon = 0);
        void hexadecimalWrite(uint8_t position, uint8_t number);
        void commandWrite(uint8_t command, uint8_t end = 1);
        void dataWrite(uint8_t data, uint8_t end = 1);
        void clear(uint8_t position = 0, uint8_t length = 4);

    private:



};



#endif /* TM1637_H_ */
