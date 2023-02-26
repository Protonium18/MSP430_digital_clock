#pragma once
#ifndef DS1306_H_
#define DS1306_H_

#include "seriallib.h"

#define REG_READ_SECONDS 0x00
#define REG_READ_MINUTES 0x01
#define REG_READ_HOURS 0x02
#define REG_READ_WEEKDAY 0x03
#define REG_READ_DAY 0x04
#define REG_READ_MONTH 0x05
#define REG_READ_YEAR 0x06

#define REG_WRITE_SECONDS 0x80
#define REG_WRITE_MINUTES 0x81
#define REG_WRITE_HOURS 0x82
#define REG_WRITE_WEEKDAY 0x83
#define REG_WRITE_DAY 0x84
#define REG_WRITE_MONTH 0x85
#define REG_WRITE_YEAR 0x86

#define REG_READ_CTRL 0x0F
#define REG_WRITE_CTRL 0x8F

#define REG_WRITE_ALM0_SECONDS 0x87
#define REG_WRITE_ALM0_MINUTES 0x88
#define REG_WRITE_ALM0_HOURS 0x89
#define REG_WRITE_ALM0_DAYS 0x8A

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
        void writeAlarm(uint8_t seconds, uint8_t minutes, uint8_t hours);
        void alarmEnable();
        void alarmDisable();
    private:

};



#endif /* DS1306_H_ */
