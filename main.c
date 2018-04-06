

#include "msp.h"
//Clock frequencies for MSP 432
#define FREQ_1_5_MHZ 1500000;
#define FREQ_3_MHZ 3000000;
#define FREQ_6_MHZ 6000000;
#define FREQ_12_MHZ 12000000;
#define FREQ_24_MHZ 24000000;
#define FREQ_48_MHZ 48000000;

void delay_ms(int ms, int freq);       // Delay function for specified number of milliseconds
void delay_us(int us, int freq);       // Delay function for specified number of microseconds
void set_DCO(int freq);                // Setting the internal DCO clock frequency

int main(void) {

   //uint32_t counter = 0;
   //uint8_t i = 0;
   P2->SEL1 &= ~0x02;      // set P2.1 as GPIO
   P2->SEL0 &= ~0x02;      // set P2.5 as GPIO
   P2->DIR  |= 0x02;       // set P2.1 as output

   while (1) {                         // Run infinitely
      P2->OUT |= (1<<1);               // P2.1 on ofr X milliseconds
      delay_ms(500, FREQ_3_MHZ);
      P2->OUT &= (1<<1);               // P2.1 off for X milliseconds
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
        DCORSEL = 4
        break;
    case FREQ_48_MHZ:
        DCORSEL = 5
        break;
    }
}*/

// Delay milliseconds function
void delay_ms(int ms, int freq) {
    int num_cycles = (freq * ms)/1000;    // Calculate number of cycles needed to generate specified delay
    __delay_cycles(num_cycles);           // Delay the device for that number of cycles
}

// Delay microseconds function
void delay_us(int us, int freq){
    int num_cycles = (freq * us)/1000000; // Calculate number of cycles needed to generate specified delay 
    __delay_cycles(num_cycles);           // Delay the device for that number of cycles
}
