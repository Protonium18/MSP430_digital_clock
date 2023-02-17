#include "tm1637.h"


TM1637::TM1637(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT)
: serialobj(PORT, SERIAL_OUT, SERIAL_IN, CLOCK, CHIP_SELECT){

}

void TM1637::commandWrite(uint8_t command, uint8_t end){
    this->setSDA(1);
    this->setCLK(1);

    this->setSDA(0);
    this->setCLK(0);
    this->byteOut(command, 0);

    this->setCLK(1);
    this->setCLK(0);

    if(end == 1){
        this->setCLK(1);
        this->setSDA(1);
    }
}

void TM1637::dataWrite(uint8_t data, uint8_t end){
    this->byteOut(data, 0);
    this->setCLK(1);
    this->setCLK(0);

    if(end == 1){
        this->setCLK(1);
        this->setSDA(1);
    }
}

uint8_t TM1637::singleWrite(uint8_t position, uint8_t number, uint8_t hexadecimal, uint8_t colon){

    if((position > 3 || number > 9) && hexadecimal == 0){
        return 1;
    }

    this->commandWrite(TM1637_DIGITS[position]);
    this->commandWrite(CMD_DATA_WRITE, 0);
    if(colon){
        this->dataWrite(TM1637_NUMBERS[number] | 0x80, 1);
    }

    else{
        this->dataWrite(TM1637_NUMBERS[number], 1);
    }


    return 0;
}

uint8_t TM1637::doubleWrite(uint8_t position, uint8_t number, uint8_t colon){

    if(position > 2 || number > 99){
        return 1;
    }

    uint8_t tens = 0;
    uint8_t ones = 0;

    while(number >= 10){
        number -= 10;
        tens++;
    }
    while (number > 0){
        number -= 1;
        ones++;
    }

    this->singleWrite(position, tens);

    if(colon){
        this->singleWrite(position+1, ones, 0, 1);
    }

    else{
        this->singleWrite(position+1, ones);
    }


    return 0;
}

void TM1637::hexadecimalWrite(uint8_t position, uint8_t number){

    uint8_t num_1 = 0;
    num_1 = (0xF0 & number) >> 4;
    uint8_t num_2 = 0;
    num_2 = (0x0F & number);

    this->singleWrite(position, num_1, 1);
    this->singleWrite(position+1, num_2, 1);
}

void TM1637::clear(uint8_t position, uint8_t length){

    for(int i = 0; i < length; i++){
        this->commandWrite(TM1637_DIGITS[i] + position);
        this->commandWrite(CMD_DATA_WRITE, 0);
        this->dataWrite(0, 1);
    }
}
