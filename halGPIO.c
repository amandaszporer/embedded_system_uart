#include  "header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"


// Global Variables
char string1[32];
const char string2[] = {"I love my Negev\n"};
int STRING2_LENGTH = 17;
unsigned int i;
int m = 0;
int j=0;
int delay_ifg = 0;
int state3_flag = 0;
int state9_flag = 0;
int delay9 =0;
unsigned int delay_time = 500;
const unsigned int timer_half_sec = 65535;




//--------------------------------------------------------------------
//             System Configuration
//--------------------------------------------------------------------
void sysConfig(void){
    GPIOconfig();
    StopAllTimers();
    lcd_init();
    lcd_clear();
    UART_init();
    _BIS_SR(GIE);                     // enable interrupts globally
}

//---------------------------------------------------------------------
//            General Functions
//---------------------------------------------------------------------

//----------------------Int to String---------------------------------//
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}
//----------------------Count Timer Calls---------------------------------
void timer_call_counter(){

    unsigned int num_of_halfSec;

    num_of_halfSec = (int) delay_time / half_sec;
    unsigned int res;
    res = delay_time % half_sec;
    res = res * clk_tmp;

    for (i=0; i < num_of_halfSec; i++){
        TIMER_A0_config(timer_half_sec);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }

    if (res > 1000){
        TIMER_A0_config(res);
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ int until Byte RXed
    }
}

unsigned int freq_to_period(unsigned int freq){
    return (1000000 / freq); // SMCLK = 1 MHz
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
    if (LPM_level == 0x00)
      _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01)
      _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02)
      _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
    else if(LPM_level == 0x03)
      _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04)
      _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
 // asm("NOP");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//                     Polling delays
//---------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}
//---------------**************************----------------------------
//               Interrupt Services Routines
//---------------**************************----------------------------
//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR  // For PB1
  __interrupt void PBs_handler(void){
    delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
    if(PBsArrIntPend & PB1){
        PBsArrIntPend &= ~PB1;  // Clear pending interrupt
        int i = 0;
        while(i < STRING2_LENGTH){ // Use your defined constant
            while (!(IFG2 & UCA0TXIFG)); // Wait for TXBUF ready
            UCA0TXBUF = string2[i++];
        }
    }

    }
//*********************************************************************
//                        TIMER A0 ISR
//*********************************************************************
#pragma vector = TIMER0_A0_VECTOR // For delay
__interrupt void TimerA_ISR (void)
{
    StopAllTimers();
    LPM0_EXIT;
}

//*********************************************************************
//                         ADC10 ISR
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}

//*********************************************************************
//                           TX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(state == state7) UCA0TXBUF = '7'; //print menu in PC
    else if(state3_flag == 1 && delay_ifg == 0){
        UCA0TXBUF = '3'; // request delay prompt
        delay_ifg = 1;   // set flag indicating we are now awaiting delay input
    }
    else if(state9_flag == 1){
            UCA0TXBUF = '9'; // request delay prompt
            delay9 = 1;   // set flag indicating we are now awaiting delay input
        }
    else UCA0TXBUF = 'F';   // Finish
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
}


//*********************************************************************
//                         RX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(UCA0RXBUF == '1' && delay_ifg == 0){     //count up
        state = state1;
        delay9 = 0;
        IE2 |= UCA0TXIE;
    }
    else if(UCA0RXBUF == '2' && delay_ifg == 0){    //buzzer
        state = state2;
        delay9 = 0;
        IE2 |= UCA0TXIE;
    }

    else if(UCA0RXBUF == '3' || delay_ifg == 1){ // if awaiting delay input
        delay9 = 0;
        if (delay_ifg == 0){
            state3_flag = 1;
            IE2 |= UCA0TXIE;
        }
        else {
            string1[j] = UCA0RXBUF;
            j++;
            if (UCA0RXBUF == '\n'){
                string1[j-1] = '\0'; // null terminate string safely
                j = 0;
                delay_ifg = 0;       // finished receiving delay input
                state3_flag = 0;
                state = state3;      // back to state3 for processing
                IE2 |= UCA0TXIE;     // enable TX interrupt to send 'F' or next prompt
            }
        }
    }

    else if(UCA0RXBUF == '4' && delay_ifg == 0){    //pot
        state = state4;
        delay9 = 0;
        IE2 |= UCA0TXIE;
    }

    else if(UCA0RXBUF == '5' && delay_ifg == 0){ //PB1 -> i love my negev
           state = state5;
           delay9 = 0;
           PB1_config();
       }

    else if(UCA0RXBUF == '6' && delay_ifg == 0){    //clearLCD
        state = state6;
        delay9 = 0;
        IE2 |= UCA0TXIE;
    }
    else if(UCA0RXBUF == '7' && delay_ifg == 0){    //Show Menu
        state = state7;
        delay9 = 0;
        IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
    }
    else if(UCA0RXBUF == '8' && delay_ifg == 0){    //sleep
        state = state8;
        delay9 = 0;
        IE2 |= UCA0TXIE;
    }
    else if(UCA0RXBUF == '9' || delay9 == 1){ // if awaiting delay input
        if (delay9 == 0){
            state9_flag = 1;
            IE2 |= UCA0TXIE;
        }
        else {
            string1[j] = UCA0RXBUF;
            j++;
            if (UCA0RXBUF == '\n'){
                string1[j-1] = '\0'; // null terminate string safely
                j = 0;
                //delay9 = 0;
                state9_flag = 0;
                state = state9;      // back to state3 for processing
                IE2 |= UCA0TXIE;     // enable TX interrupt to send 'F' or next prompt
            }
        }
    }

    switch(lpm_mode){
    case mode0:
        LPM0_EXIT; // must be called from ISR only
        break;
    case mode1:
        LPM1_EXIT; // must be called from ISR only
        break;
    case mode2:
        LPM2_EXIT; // must be called from ISR only
        break;
    case mode3:
        LPM3_EXIT; // must be called from ISR only
        break;
    case mode4:
        LPM4_EXIT; // must be called from ISR only
        break;
    }
}
