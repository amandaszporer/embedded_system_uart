#include  "header/api.h"            // private library - API layer
#include  "header/app.h"            // private library - APP layer
#include  <stdio.h>


enum FSMstate state;
enum SYSmode lpm_mode;


void main(void){

  state = state7;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();     // Configure GPIO, Stop Timers, Init LCD


  while(1){
    switch(state){
    case state8: //idle
        IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
        break;
    case state1: // Count up onto LCD
        count_LCD();
        break;
    case state2: //Buzzer
        circular_tone_sequence();
        break;

    case state3: ; // Get delay time X
        change_delay_time();
        break;

    case state4: // Measure Potentiometer 3-digit value
        measure_pot();
        break;

    case state5:    //IlovemyNegev
        enable_pb1();
        break;

    case state6: // Clear Counts and LCD
        clear_counters();
        break;

    case state7: // Print Menu to PC
        state = state8;
        break;

    case state9:
        print_string();
        __bis_SR_register(LPM0_bits + GIE);
        break;
    }
  }
}
