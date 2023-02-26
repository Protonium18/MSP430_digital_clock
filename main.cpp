#include <msp430.h> 
#include "seriallib.h"
#include "tm1637.h"
#include <stdint.h>
#include "ds1306.h"
#include "defs.h"

DS1306 rtc(1, 2, 3, 4, 5);
TM1637 screen(2, 1, NULL_PIN, 0, NULL_PIN);

uint8_t MODE = 0;
uint8_t BLINK = 0;
uint8_t TIME[3];
uint8_t SELECTION = 0;
uint8_t TIME_MODE = 1;
uint8_t ALM = 0;
uint8_t ALARM_SET = 0;



void writeTime(uint8_t blink = 0);
void writeMinutes();
void writeHours();
void RTCWriteTime();
void writeSeconds();

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

	P1DIR |= BIT1 | BIT6 | BIT7;
	P1OUT = 0;


	//initializing port 2, pins 2-6 for high-to-low interrupts with pullup resistor

	P2SEL &= ~(BIT6 | BIT7);
	P2OUT |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
	P2REN |= BIT2| BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
	P2IE |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6;
	P2DIR &= ~(BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);
	P2IES = 0;
	P2IES |= BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
	P2IFG = 0;

	P1DIR |= BIT1 | BIT6 | BIT7;




	READ_TIME;
	writeTime();

	rtc.writeData(REG_WRITE_CTRL, 0x04);

    __bis_SR_register(GIE + LPM3_bits);

	return 0;
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

    TIMER_CLEAR;



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
                rtc.readData(0x07);
                ALM = 1;
            }
            P2IFG &= ~BIT2;
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

            else if(MODE == 2){
                CLEAR_TIME;
                TIMER_CLEAR;
                writeTime();
            }

            P2IFG &= ~BIT3;
        }

        if(P2IFG & BIT4){
            if(MODE == 0){
                MODE = 2;
                P2IE |= BIT7;
                CLEAR_TIME;
                writeTime();
            }

            else if(MODE == 2){
                CLEAR_TIME;
                MODE = 3;
            }
            else if(MODE == 3){
                volatile unsigned int i = 64000;
                while(!(P2IN & BIT4)){
                    i--;
                    if(i == 0){
                        MODE = 4;
                        writeTime();
                        LED_ALARM_ON;
                        ALARM_SET = 1;
                        break;
                    }
                }
            }

            else if(MODE == 4){
                MODE = 0;
                P2IE &= ~BIT7;
                LED_ALARM_OFF;
                BUZZER_OFF;
                ALM = 0;
                ALARM_SET = 0;
                READ_TIME;
                writeTime();
            }

            //mode 1 code, changing selection of digits
            if(MODE == 1 || MODE == 3){
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
            if(MODE == 0 || MODE == 2 || MODE == 4){
                if(TIME_MODE == 0){
                    TIME_MODE = 1;
                }
                else if(TIME_MODE == 1){
                    TIME_MODE = 0;
                }


                //if in time counting mode, clear internal time and reset
                if(MODE == 2){
                    CLEAR_TIME;
                    TIMER_CLEAR;
                }

                writeTime();



            }
            //if in edit mode 1, increment time when pressed
            else if(MODE == 1 || MODE == 3){
                incrementTime();
            }
            P2IFG &= ~BIT5;

        }

        if(P2IFG & BIT6){
            if(MODE == 0){
                volatile unsigned int timer = 64000;

                while(!(P2IN & BIT6)){
                    timer--;
                    if(timer == 0){
                        ALARM_SET = 0;
                        break;
                    }
                }

                if(ALARM_SET == 0){
                    rtc.writeAlarm(0, 0, 0);
                    LED_ALARM_OFF;
                    BUZZER_OFF;
                    RTC_ALARM_DISABLE;
                    rtc.alarmDisable();
                    ALM = 0;
                    ALARM_SET = 0;
                }

                else if(ALARM_SET == 1){
                    ALM = 0;
                    BUZZER_OFF;
                }



            }

            if(MODE == 1){
                rtc.writeAlarm(0, TIME[1], TIME[2]);
                LED_ALARM_ON;
                RTC_ALARM_ENABLE;
                rtc.alarmEnable();
                ALARM_SET = 1;

                MODE = 0;
                BLINK = 0;
                SELECTION = 0;
                TACCR0 = 16383;

                READ_TIME;
 /*               CLEAR_TIME;
                TIME[1] = BCDToDec(rtc.readData(0x08));
                TIME[2] = rtc.readData(0x0A);
*/
                writeTime(0);
                //while(1){

                //}

            }

            P2IFG &= ~BIT6;
        }

        //connected to 1 hz output on DS1306
        if(P2IFG & BIT7){
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
                }

            if(MODE == 4 && ALM == 0){
                if(TIME[0] != 0){
                    TIME[0]--;
                }
                else if(TIME[0] == 0){
                    if(TIME[1] != 0){
                        TIME[1]--;
                        TIME[0] = 59;
                    }
                    else if(TIME[1] == 0){
                        TIME[2]--;
                        TIME[1] = 59;
                    }

                }

                if(TIME[0] == 0 && TIME[1] == 0 && TIME[2] == 0){
                    ALM = 1;
                }


            }

            P2IFG &= ~BIT7;
        }


}

#pragma vector = PORT1_VECTOR
    __interrupt void PORT_1_INTERRUPT(void){
        if(P2IFG & BIT3){
            P2IFG &= ~BIT3;
        }


}

#pragma vector = TIMER0_A1_VECTOR
    __interrupt void TIMER0_INTERRUPT(void){

        //mode 0 (clock mode)
        if(MODE == 0 || MODE == 2 || MODE == 4){
            if(MODE == 0){
                READ_TIME;
            }

            if(TIME_MODE == 0 || ALM == 1){
                if(BLINK == 0){
                    writeTime(0);
                    BLINK = 1;
                    if(ALM == 1){
                        BUZZER_OFF;
                        LED_ALARM_OFF;
                        screen.clear();
                    }
                }

                else if(BLINK == 1){
                    writeTime(1);
                    BLINK = 0;
                    if(ALM == 1){
                        BUZZER_ON;
                        LED_ALARM_ON;
                    }
                }
            }
            else if(TIME_MODE == 1){
                writeTime(0);
            }
        }

        //edit time (mode 1), blink digit currently selected
        if(MODE == 1 || MODE == 3){
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


        TA0CTL &= ~TAIFG;
    }
