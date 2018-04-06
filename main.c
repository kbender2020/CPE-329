

#include "msp.h"

#define FREQ_1_5_MHZ 1500000;
#define FREQ_3_MHZ 3000000;
#define FREQ_6_MHZ 6000000;
#define FREQ_12_MHZ 12000000;
#define FREQ_24_MHZ 24000000;
#define FREQ_48_MHZ 48000000;

void delay_ms(int ms, int freq);
void delay_us(int us, int freq);
void set_DCO(int freq);

int main(void) {

   //uint32_t counter = 0;
   //uint8_t i = 0;
   P2->SEL1 &= ~0x02;      // set P2.1 as GPIO
   P2->SEL0 &= ~0x02;      // set P2.5 as GPIO
   P2->DIR  |= 0x02;       // set P2.1 as output

   while (1) {
      P2->OUT |= (1<<1);    // P2.1 on
      delay_ms(500, FREQ_3_MHZ);
      P2->OUT &= (1<<1);   // P2.1 off
      delay_ms(500, FREQ_3_MHZ);
    }
}

/* Set Frequency
void set_DCO(int freq){
    switch(freq){
    case FREQ_1.5_MHZ:
        DCORSEL = 0
        break;
    case FREQ_3_MHZ:
        DCORSEL = 1
        break;
    case FREQ_6_MHZ:
        DCORSEL = 2
        break;
    case FREQ_12_MHZ:
        DCORSEL = 3
        break;
    case FREQ_24_MHZ:
        DCOSEL = 4
        break;
    case FREQ_48_MHZ:
        DCOSEL = 5
        break;
    }
}*/

// Delay milliseconds function
void delay_ms(int ms, int freq) {
    int num_cycles = (freq * ms)/1000;
    __delay_cycles(num_cycles);
}

// Delay microseconds function
void delay_us(int us, int freq){
    int num_cycles = (freq * us)/1000000;
    __delay_cycles(num_cycles);
}
