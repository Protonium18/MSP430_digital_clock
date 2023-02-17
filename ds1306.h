#pragma once
#ifndef DS1306_H_
#define DS1306_H_

#include "seriallib.h"

#define REG_SECONDS 0x00
#define REG_MINUTES 0x01
#define REG_HOURS 0x02

uint8_t BCDToDec(uint8_t number);
uint8_t DecToBCD(uint8_t number);

class DS1306 : public serialobj{
    public:
        DS1306(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT);
        uint8_t readData(uint8_t address);
        void writeData(uint8_t address, uint8_t data);
        void writeTime(uint8_t seconds, uint8_t minutes, uint8_t hours);
        void readTime(uint8_t* data_array);
        void burstWriteData(uint8_t address, uint8_t* data, uint8_t len);
    private:

};



#endif /* DS1306_H_ */
