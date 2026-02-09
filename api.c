#include  "header/api.h"            // private library - API layer
#include  "header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"


unsigned int count = 0;
char count_str[5];

const unsigned int resolution = 1024;
const float v_ref = 3.41;
float pot_meas;
char afterDigit_str[4];
char beforeDigit_str[1];

const unsigned int freq_seq[] = {1000, 1250, 1500, 1750, 2000, 2250, 2500};
const unsigned int num_freqs = sizeof(freq_seq)/sizeof(freq_seq[0]);

//-------------------------------------------------------------
//                1. Count up
//------------------------------------------------------------
void count_LCD(){
    lcd_clear();
    lcd_home();
    lcd_puts("Count Up: ");
    lcd_new_line;
    while(state==state1){
        int2str(count_str, count);
        lcd_puts(count_str);
        timer_call_counter();
        lcd_home();
        lcd_new_line;
        count++;
    }
}

//-------------------------------------------------------------
//                2. Buzzer Tone Series
//------------------------------------------------------------
void circular_tone_sequence(void){
    unsigned int i = 0;

    while(state == state2){
        unsigned int period = freq_to_period(freq_seq[i]);
        TIMER1_A_config(period);   // Configure PWM for current frequency

        timer_call_counter();      // Wait X ms using your existing timer-based delay

        i = (i + 1) % num_freqs;   // Circular increment
    }

    // Stop timer when done to turn off buzzer
    TA1CTL = MC_0; // Stop timer
    P2OUT &= ~BIT4; // Ensure buzzer off
}


//-------------------------------------------------------------
//                3. Change Delay Time [ms]
//------------------------------------------------------------
void change_delay_time(){
    delay_time = atoi(string1);  // Get delay time from user
    state = state8;
}


//-------------------------------------------------------------
//                4. Measure Pot voltage [v]
//------------------------------------------------------------
void measure_pot(){
    lcd_clear();
    lcd_home();
    lcd_puts("Pot Measurement:");

    ADC_config();
    while (state == state4){
        ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
        ADC10CTL0 &= ~ADC10ON; // Don't get into interrupt

        pot_meas = ( v_ref * ADC10MEM ) / resolution;

        int ipart = (int)pot_meas;

        float fpart = pot_meas - (float)ipart;

        int afterdigit = (int) (fpart * 1000);

        int2str(beforeDigit_str, ipart);

        int2str(afterDigit_str, afterdigit);
        //write to LCD
        lcd_home();
        lcd_new_line;
        if (ipart == 0) lcd_puts("0");
        else lcd_puts(beforeDigit_str);
        lcd_puts(".");
        lcd_puts(afterDigit_str);
        lcd_puts(" [V]");
        timer_call_counter();
        }
}
//-------------------------------------------------------------
//                5. PB1 I love my Negev
//------------------------------------------------------------
void enable_pb1(){
    PBsArrIntEn |= PB1;             //Enable PB1 Interrupts

}
//-------------------------------------------------------------
//                6. Clear LCD and init counters
//------------------------------------------------------------
void clear_counters(){
    disable_interrupts();
    lcd_clear();
    lcd_home();
    count = 0;
    enable_interrupts();
    state = state8;
}

//-------------------------------------------------------------
//                6. Clear LCD and init counters
//------------------------------------------------------------

void print_string(){
    lcd_clear();
    lcd_home();
    lcd_puts(string1);
}
