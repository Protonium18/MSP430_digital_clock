#pragma once
#ifndef SERIALLIB_H_
#define SERIALLIB_H_
#include <msp430.h>
#include <stdint.h>

#define MSB_FIRST 1
#define LSB_FIRST 0
#define NULL_PIN 99 //Defines a null pin for unused pins.



class serialobj{
    public:
        serialobj(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT);
        uint8_t byteOut(uint8_t data, uint8_t direction = LSB_FIRST);
        uint8_t bytesOut(uint8_t* data_array, uint16_t len, uint8_t direction = LSB_FIRST);
        uint8_t byteIn(uint8_t direction = LSB_FIRST);
        uint8_t bytesIn(uint8_t* data_array, uint16_t len, uint8_t direction = LSB_FIRST);
        void chipSelect(uint8_t out);
        void setSDA(uint8_t out);
        uint8_t readSDA();
        void setCLK(uint8_t out);

    private:
        uint8_t SDA_IN;
        uint8_t SDA_OUT;
        uint8_t CLK;
        uint8_t SELECT;

        volatile uint8_t* PORT_OUT;
        volatile uint8_t* PORT_IN;
        volatile uint8_t* PORT_DIR;
};



#endif /* SERIALLIB_H_ */
