#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      10000


//#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms


// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL


//   Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT


// PushButtons abstraction
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PBsArrPortOut      P1OUT
//#define PB0                0x02   // P1.0
#define PB1                0x01  // P1.0
//#define PB2                0x04  // P1.2
//#define PB3                0x08  // P2.0

#define TXLED BIT0
#define RXLED BIT6
#define TXD BIT2
#define RXD BIT1


extern void GPIOconfig(void);
extern void PB1_config(void);
extern void ADC_config(void);
extern void TIMER_A0_config(unsigned int counter);
extern void TIMER1_A_config(unsigned int period);
extern void StopAllTimers(void);
extern void UART_init(void);



#endif
