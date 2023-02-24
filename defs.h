
#ifndef DEFS_H_
#define DEFS_H_

#define TIMER_CLEAR TA0CTL |= TACLR;
#define CLEAR_TIME TIME[0] = 0; TIME[1] = 0; TIME[2] = 0;
#define READ_TIME rtc.readTime(TIME);

#define MODE_CLOCK 0
#define MODE_EDIT_CLOCK 1
#define MODE_TIMER 2

#define LED_ALARM_ON P1OUT |= BIT7;
#define LED_ALARM_OFF P1OUT &= ~BIT7;

#define BUZZER_ON P1OUT |= BIT1;
#define BUZZER_OFF P1OUT &= ~BIT1;

#define RTC_ALARM_ENABLE rtc.writeData(REG_WRITE_CTRL, 0x04 | 0x01);
#define RTC_ALARM_DISABLE rtc.writeData(REG_WRITE_CTRL, 0x04);

#endif /* DEFS_H_ */
