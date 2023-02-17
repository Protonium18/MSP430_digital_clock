#include "ds1306.h"

uint8_t DecToBCD(uint8_t number){
    uint8_t data_out = 0;
    data_out = (number/10 << 4) | (number%10);


    return data_out;
}

uint8_t BCDToDec(uint8_t number){
    uint8_t data_out = 0;
    data_out = (((number & 0xF0) >> 4) * 10) + (number & 0x0F);

    return data_out;

}

DS1306::DS1306(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT)
: serialobj(PORT, SERIAL_OUT, SERIAL_IN, CLOCK, CHIP_SELECT){

    this->chipSelect(0);

}

uint8_t DS1306::readData(uint8_t address){
    uint8_t data = 0;

    this->chipSelect(1);
    this->byteOut(address, MSB_FIRST);
    data = this->byteIn(MSB_FIRST);
    this->chipSelect(0);

    return data;

}

void DS1306::writeData(uint8_t address, uint8_t data){

    this->chipSelect(1);
    this->byteOut(0x80 | address, MSB_FIRST);
    this->byteOut(data, MSB_FIRST);
    this->chipSelect(0);

}

void DS1306::writeTime(uint8_t seconds, uint8_t minutes, uint8_t hours){

    this->chipSelect(1);
    this->byteOut(0x80 | REG_SECONDS, MSB_FIRST);

    this->byteOut(DecToBCD(seconds), MSB_FIRST);
    this->byteOut(DecToBCD(minutes), MSB_FIRST);
    this->byteOut(DecToBCD(hours), MSB_FIRST);

    this->chipSelect(0);
}

void DS1306::readTime(uint8_t* data_array){
    this->chipSelect(1);

    this->byteOut(REG_SECONDS, MSB_FIRST);
    data_array[0] = BCDToDec(this->byteIn(MSB_FIRST));
    data_array[1] = BCDToDec(this->byteIn(MSB_FIRST));
    data_array[2] = BCDToDec(this->byteIn(MSB_FIRST));

    this->chipSelect(0);
}

void DS1306::burstWriteData(uint8_t address, uint8_t* data, uint8_t len){
    this->chipSelect(1);
    this->byteOut(address, MSB_FIRST);
    for(uint8_t i = 0; i < len; i++){
        this->byteOut(data[i]);
    }
    this->chipSelect(0);
}
