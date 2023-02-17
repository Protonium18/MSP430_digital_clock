#include "seriallib.h"

serialobj::serialobj(uint8_t PORT, uint8_t SERIAL_OUT, uint8_t SERIAL_IN, uint8_t CLOCK, uint8_t CHIP_SELECT){
#ifdef __MSP430_HAS_PORT1_R__
    if(PORT == 1){
        PORT_OUT = &P1OUT;
        PORT_IN = &P1IN;
        PORT_DIR = &P1DIR;
    }
#endif
#ifdef __MSP430_HAS_PORT2_R__
    else if(PORT == 2){
        PORT_OUT = &P2OUT;
        PORT_IN = &P2IN;
        PORT_DIR = &P2DIR;
    }
#endif
#ifdef __MSP430_HAS_PORT3_R__
    else if(PORT == 3){
        PORT_OUT = &P3OUT;
        PORT_IN = &P3IN;
        PORT_DIR = &P3DIR;
    }
#endif
#ifdef __MSP430_HAS_PORT4_R__
    else if(PORT == 4){
        PORT_OUT = &P4OUT;
        PORT_IN = &P4IN;
        PORT_DIR = &P4DIR;
    }
#endif
#ifdef __MSP430_HAS_PORT5_R__
    else if(PORT == 5){
        PORT_OUT = &P5OUT;
        PORT_IN = &P5IN;
        PORT_DIR = &P5DIR;
    }
#endif
#ifdef __MSP430_HAS_PORT6_R__
    else if(PORT == 6){
        PORT_OUT = &P6OUT;
        PORT_IN = &P6IN;
        PORT_DIR = &P6DIR;
    }
#endif

    if(SDA_OUT != NULL_PIN){
        SDA_OUT = 0x01 << SERIAL_OUT;
        *PORT_DIR |= SDA_OUT;
    }

    if(SDA_IN != NULL_PIN){
        SDA_IN = 0x01 << SERIAL_IN;
        *PORT_DIR &= ~SDA_IN;
    }

    if (CLK != NULL_PIN){
        CLK = 0x01 << CLOCK;
        *PORT_DIR |= CLK;
    }

    if(SELECT != NULL_PIN){
        SELECT = 0x01 << CHIP_SELECT;
        *PORT_DIR |= SELECT;
    }

}

uint8_t serialobj::byteOut(uint8_t data, uint8_t direction){
    for(uint8_t i = 0; i < 8; i++){
        if(direction == 0 && (data & 0x01 << i)){
            *PORT_OUT |= SDA_OUT;
        }
        else if(direction == 1 && (data & 0x80 >> i)){
            *PORT_OUT |= SDA_OUT;
        }

        *PORT_OUT |= CLK;
        *PORT_OUT &= ~CLK;
        *PORT_OUT &= ~SDA_OUT;
    }

    return 0;
}

uint8_t serialobj::bytesOut(uint8_t* data_array, uint16_t len, uint8_t direction){
    for(uint16_t i = 0; i < len; i++){
        this->byteOut(data_array[i], direction);
    }

    return 0;
}

uint8_t serialobj::byteIn(uint8_t direction){
    uint8_t data = 0;

    for(uint8_t i = 0; i < 8; i++){
       *PORT_OUT |= CLK;
       if(direction == 0 && (*PORT_IN & SDA_IN)){
           data |= 0x01 << i;
       }

       else if(direction == 1 && (*PORT_IN & SDA_IN)){
           data |= 0x80 >> i;
       }
       *PORT_OUT &= ~CLK;
   }
   return data;
}

uint8_t serialobj::bytesIn(uint8_t* data_array, uint16_t len, uint8_t direction){
    for(uint16_t i = 0; i < len; i++){
        data_array[i] = this->byteIn(direction);
    }

    return 0;
}

void serialobj::chipSelect(uint8_t out){
    if(out){
        *PORT_OUT |= SELECT;
    }
    else{
        *PORT_OUT &= ~SELECT;
    }
}

void serialobj::setSDA(uint8_t out){
    if(out){
        *PORT_OUT |= SDA_OUT;
    }
    else{
        *PORT_OUT &= ~SDA_OUT;
    }
}

uint8_t serialobj::readSDA(){
    if(*PORT_IN & SDA_IN){
        return 1;
    }
    else{
        return 0;
    }
}

void serialobj::setCLK(uint8_t out){
    if(out){
        *PORT_OUT |= CLK;
    }
    else{
        *PORT_OUT &= ~CLK;
    }
}
