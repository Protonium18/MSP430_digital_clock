#include <msp430.h> 
#include "seriallib.h"
#include "tm1637.h"
#include <stdint.h>
#include "ds1306.h"


DS1306 rtc(1, 2, 3, 4, 5);
TM1637 screen(2, 1, NULL_PIN, 0, NULL_PIN);

uint8_t MODE = 0;
uint8_t BLINK = 0;
uint8_t TIME[3];
uint8_t SELECTION = 0;
uint8_t TIME_MODE = 1;

void writeTime(uint8_t blink = 0);
void readTime();
void writeMinutes();
void writeHours();
void RTCWriteTime();
void writeSeconds();
void clearTime();

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	P2OUT = 0;
	P1SEL |= BIT0;

	TA0CTL |= TAIE;
	TA0CTL |= MC_1 | TASSEL_0;
	TA0CCR0 = 16383;

	screen.clear();

	screen.commandWrite(0x8F, 1);


	//initializing port 2, pins 2-6 for high-to-low interrupts with pullup resistor
	P2OUT |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
	P2REN |= BIT2| BIT3 | BIT4 | BIT5 | BIT6;
	P2IE |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
	P2DIR &= ~(BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
	P2IES = 0;
	P2IES |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
    P2IFG = 0;


	readTime();
	writeTime();
/*
	static uint8_t alarm_mask[4] = {0x80, 0x80, 0x80, 0x80};
	rtc.burstWriteData(0x87, alarm_mask, 4);
	rtc.writeData(0x8F, 0x01);
*/
    __bis_SR_register(GIE + LPM3_bits);

	while(1){

	}

	return 0;
}

void clearTime(){
    //clearing internal time
    TIME[0] = 0;
    TIME[1] = 0;
    TIME[2] = 0;
}

void incrementTime(){
    if(TIME_MODE == 0){
        if(SELECTION == 0){
            TIME[2]++;
            if(TIME[2] == 24){
                TIME[2] = 0;
            }
            writeHours();
        }
        if(SELECTION == 1){
            TIME[1]++;
            if(TIME[1] == 60){
                TIME[1] = 0;
            }
            writeMinutes();
        }
    }

    if(TIME_MODE == 1){
        if(SELECTION == 0){
            TIME[1]++;
            if(TIME[1] == 60){
                TIME[1] = 0;
            }
            writeMinutes();
        }
        if(SELECTION == 1){
            TIME[0]++;
            if(TIME[0] == 60){
                TIME[0] = 0;
            }
            writeSeconds();
        }
    }

    TA0CTL |= TACLR;



}

void writeTime(uint8_t blink){
    if(TIME_MODE == 0){
        screen.doubleWrite(0, TIME[2], blink);
        screen.doubleWrite(2, TIME[1]);
    }
    else if(TIME_MODE == 1){
        screen.doubleWrite(0, TIME[1], 1);
        screen.doubleWrite(2, TIME[0]);
    }

}

void writeMinutes(){
    if(TIME_MODE == 0){
        screen.doubleWrite(2, TIME[1]);
    }
    if(TIME_MODE == 1){
        screen.doubleWrite(0, TIME[1], 1);
    }
}

void writeHours(){
    screen.doubleWrite(0, TIME[2], 1);
}

void writeSeconds(){
    screen.doubleWrite(2, TIME[0], 0);
}

void readTime(){
    rtc.readTime(TIME);
}

void RTCWriteTime(){
    if(TIME_MODE == 0){
        rtc.writeTime(0, TIME[1], TIME[2]);
    }
    else if (TIME_MODE == 1){
        rtc.writeTime(TIME[0], TIME[1], 0);
    }
}

#pragma vector = PORT2_VECTOR
    __interrupt void PORT_2_INTERRUPT(void){
        if(P2IFG & BIT2){
            if(MODE == 0){
                readTime();
                writeTime();
                rtc.readData(0x07);

                TA0CTL |= TACLR;
            }
        }//alarm


        if(P2IFG & BIT3){
            if(MODE == 0){
                MODE = 1;
                BLINK = 0;
                TA0CCR0 = 10000;
            }
            else if(MODE == 1){
                RTCWriteTime();
                writeTime();
                MODE = 0;
                BLINK = 0;
                SELECTION = 0;
                TACCR0 = 16383;
            }

            P2IFG &= ~BIT3;
        }

        if(P2IFG & BIT4){
            if(MODE == 0){
                MODE = 2;
                TA0CTL |= TACLR;
                clearTime();
                writeTime();
            }

            else if(MODE == 2){
                MODE = 0;
                readTime();
                writeTime();
            }

            //mode 1 code, changing selection of digits
            if(MODE == 1){
                if(SELECTION == 1){
                    writeTime();
                    SELECTION = 0;
                }
                else{
                    writeTime();
                    SELECTION++;
                }
            }


            P2IFG &= ~BIT4;
        }

        if(P2IFG & BIT5){
            //toggling between hrs-mins and mins-secs
            if(MODE == 0 || MODE == 2){
                if(TIME_MODE == 0){
                    TIME_MODE = 1;
                }
                else if(TIME_MODE == 1){
                    TIME_MODE = 0;
                }


                //if in time counting mode, clear internal time and reset
                if(MODE == 2){
                    clearTime();
                    TA0CTL |= TACLR;
                }

                writeTime();



            }
            //if in edit mode 1, increment time when pressed
            else if(MODE == 1){
                incrementTime();
            }
            P2IFG &= ~BIT5;

        }


}

#pragma vector = PORT1_VECTOR
    __interrupt void PORT_1_INTERRUPT(void){
        if(P2IFG & BIT3){
            while(1){

            }
            P2IFG &= ~BIT3;
        }


}

#pragma vector = TIMER0_A1_VECTOR
    __interrupt void TIMER0_INTERRUPT(void){

        //mode 0 (clock mode)
        if(MODE == 0){
            readTime();
            if(TIME_MODE == 0){
                if(BLINK == 0){
                    writeTime(0);
                    BLINK = 1;
                }

                else if(BLINK == 1){
                    writeTime(1);
                    BLINK = 0;
                }
            }
            else if(TIME_MODE == 1){
                writeTime(0);
            }
        }

        //edit time (mode 1), blink digit currently selected
        if(MODE == 1){
            if(BLINK == 0){
                if(SELECTION == 0){
                    screen.clear(0, 2);
                }
                if(SELECTION == 1){
                    screen.clear(2, 2);
                }

                BLINK = 1;
            }
            else if(BLINK == 1){
                if(SELECTION == 0){
                    if(TIME_MODE == 0){
                        writeHours();
                    }
                    else if(TIME_MODE == 1){
                        writeMinutes();
                    }

                }
                if(SELECTION == 1){
                    if(TIME_MODE == 0){
                        writeMinutes();
                    }
                    if(TIME_MODE == 1){
                        writeSeconds();
                    }

                }

                BLINK = 0;
            }
        }
        //keeping track of time locally, need to fix because of counter, not correct (counter is 16383 while crystal is 32768hz, so it's counting by half seconds!!)
        if(MODE == 2){
            TIME[0]++;
            if(TIME[0] == 60){
                TIME[1]++;
                TIME[0] = 0;
                if(TIME[1] == 60){
                    TIME[2]++;
                    TIME[1] = 0;
                    if(TIME[2] == 24){
                        TIME[2] = 0;
                    }
                }
            }

        writeTime(1);
        }

        TA0CTL &= ~TAIFG;
    }
