

#include "msp.h"
#include "msp432.h"

#define CS  ((CS_Type *) CS_BASE)

#define U10_1_5_MHZ 15
#define FREQ_1_5_MHZ 1500000

#define U10_3_MHZ 30               //30 cycles needed for a 10us delay
#define FREQ_3_MHZ 3000000

#define U10_6_MHZ 60               //60 cycles needed for a 10us delay
#define FREQ_6_MHZ 6000000

#define U10_12_MHZ 120             //120 cycles needed for a 10us delay
#define FREQ_12_MHZ 12000000

#define U10_24_MHZ 240             //240 cycles needed for a 10us delay
#define FREQ_24_MHZ 24000000

#define U10_48_MHZ 480             //480 cycles needed for a 10us delay
#define FREQ_48_MHZ 48000000

void delay_ms(int ms, int freq);
void delay_us(int us, int freq);    //Smallest accepted delay value is 10us
void set_DCO(int freq);

int main(void) {

   //uint32_t counter = 0;
   //uint8_t i = 0;
   P2->SEL1 &= ~0x02;      // set P2.1 as GPIO
   P2->SEL0 &= ~0x02;      // set P2.5 as GPIO
   P2->DIR  |= 0x02;       // set P2.1 as output

   while (1) {
      P2->OUT |= 1<<1;    // P2.1 on
      delay_ms(500, FREQ_3_MHZ);
      P2->OUT &= 1<<1;   // P2.1 off
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
// Switch-case statement selects which __delay_cycles() constant to use for each frequency.
//Each #define 10US in the header above is the number of clock cycles it takes for __delay_cycles() to generate a 10us delay
void delay_ms(int ms, int freq) {
    int i;
    switch(freq){
    case FREQ_1_5_MHZ:
        for(i=0; i<ms*100; i++){__delay_cycles(U10_1_5_MHZ);}
        break;

    case FREQ_3_MHZ:
        for(i=0; i<ms*10; i++){__delay_cycles(U10_3_MHZ);}
        break;

    case FREQ_6_MHZ:
        for(i=0; i<ms*100; i++){__delay_cycles(U10_6_MHZ);}
        break;

    case FREQ_12_MHZ:
        for(i=0; i<ms*100; i++){__delay_cycles(U10_12_MHZ);}
        break;

    case FREQ_24_MHZ:
        for(i=0; i<ms*100; i++){__delay_cycles(U10_24_MHZ);}
        break;

    case FREQ_48_MHZ:
        for(i=0; i<ms*100; i++){__delay_cycles(U10_48_MHZ);}
        break;
    }
}

// Delay microseconds function
void delay_us(int us, int freq) {
    int i;
    switch(freq){
    case FREQ_1_5_MHZ:
        for(i=0; i<us/10; i++){__delay_cycles(U10_1_5_MHZ);}
        break;

    case FREQ_3_MHZ:
        for(i=0; i<us/10; i++){__delay_cycles(U10_3_MHZ);}
        break;

    case FREQ_6_MHZ:
        for(i=0; i<us/10; i++){__delay_cycles(U10_6_MHZ);}
        break;

    case FREQ_12_MHZ:
        for(i=0; i<us/10; i++
        ){__delay_cycles(U10_12_MHZ);}
        break;

    case FREQ_24_MHZ:
        for(i=0; i<us/10; i++){__delay_cycles(U10_24_MHZ);}
        break;

    case FREQ_48_MHZ:
        for(i=0; i<us/10; i++){__delay_cycles(U10_48_MHZ);}
        break;
    }
}
