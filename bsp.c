#include  "header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT

  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P1.4-P1.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7


  //Buzzer Config
  P2DIR |= BIT4;   // P2.4 output
  P2SEL |= BIT4;   // Select TA1.2 functionality
  _BIS_SR(GIE);                     // enable interrupts globally
}

void PB1_config(void){
  PBsArrPortSel &= ~PB1;          // Ensure P1.0 is GPIO
  PBsArrPortOut &= ~PB1;          // P1.0 output low (if output)
  PBsArrPortDir &= ~PB1;          // P1.0 as input
  PBsArrIntEdgeSel |= PB1;        // Pull-up mode on P1.0
 // PBsArrIntEn |= PB1;             //Enable PB1 Interrupts
  PBsArrIntPend &= ~PB1;          // Clear pending interrupt on P1.0

  _BIS_SR(GIE);                     // enable interrupts globally
}

//-------------------------------------------------------------------------------------
//            Stop All Timers
//-------------------------------------------------------------------------------------
void StopAllTimers(void){
    TACTL = MC_0; // halt timer A

}
//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADC_config(void){
      ADC10CTL0 = ADC10SHT_2 + ADC10ON+ SREF_0 + ADC10IE;  // 16*ADCLK+ Turn on, set ref to Vcc and Gnd, and Enable Interrupt
      ADC10CTL1 = INCH_3 + ADC10SSEL_3;     // Input A3 and SMCLK, was |
      ADC10AE0 |= BIT3;                         // P1.3 ADC option select
}

//-------------------------------------------------------------------------------------
//            Timer configuration
//-------------------------------------------------------------------------------------
void TIMER_A0_config(unsigned int counter){
    TACCR0 = counter; // (2^20/8)*345m = 45219 -> 0xB0A3
    TACCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1 + ID_3;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
}

void TIMER1_A_config(unsigned int period){
    TA1CCR0 = period;          // Set PWM period
    TA1CCR2 = period >> 1;     // 50% duty cycle
    TA1CCTL2 = OUTMOD_7;       // Reset/Set mode

    TA1CTL = TASSEL_2 | MC_1 | TACLR; // SMCLK, Up mode, clear
}

//-------------------------------------------------------------------------------------
//                              UART init
//-------------------------------------------------------------------------------------
void UART_init(void){
    if (CALBC1_1MHZ==0xFF)                  // Check if calibration constant for 1MHz not erased
      {
        while(1);                           // Trap CPU if missing
      }
    DCOCTL = 0;                             // Reset DCO control register; selects lowest DCOx and MODx settings initially
    BCSCTL1 = CALBC1_1MHZ;                  // Load calibrated DCO constant for 1MHz into Basic Clock System Control 1
    DCOCTL = CALDCO_1MHZ;                   // Load calibrated DCO constant for 1MHz into DCO control register (sets final DCO frequency)

    P2DIR = 0xFF;                           // P2 - Output
    P2OUT = 0;                              //

    P1SEL |= BIT1 + BIT2;                   // Set P1.1 (RXD) and P1.2 (TXD) as peripheral function pins
    P1SEL2 |= BIT1 + BIT2;                  // Set P1.1 (RXD) and P1.2 (TXD) to secondary peripheral function (USCI_A0)

    P1DIR |= RXLED + TXLED;                 // Set RXLED and TXLED pins as output
    P1OUT &= 0x00;                          // Clear RXLED and TXLED outputs

    //Bound Control Register Config
    UCA0BR0 = 104;                          // Baud Rate = 9600 (see datasheet table: 1MHz/9600 ~ 104.167)
    UCA0BR1 = 0;                            // Upper byte of baud rate (0 for 9600 at 1MHz)

    //Modulation Control Register Config
    UCA0MCTL = UCBRS0;                      // Modulation control: set UCBRSx = 1 for fine-tuning baud rate

    //Control Register 1 Config
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK as clock  for UART
    UCA0CTL1 &= ~UCSWRST;                   // Initialize USCI by releasing it from software reset


}


