

#include "msp.h"
#include "msp432.h"

#define CS  ((CS_Type *) CS_BASE)


#define FREQ_1_5_MHZ 1.5
#define FREQ_3_MHZ 3
#define FREQ_6_MHZ 6
#define FREQ_12_MHZ 12
#define FREQ_24_MHZ 24
#define FREQ_48_MHZ 48

void delay_ms(int ms, int freq);
void delay_us(int us, int freq);
void set_DCO(int freq);
int i;

void main(void)
{
     P2->SEL1 &= ~0x02;      // set P2.1 as GPIO
     P2->SEL0 &= ~0x02;      // set P2.5 as GPIO
     P2->DIR  |= 0x02;       // set P2.1 as output

     while(1) {
         P2->OUT |= (1<<1);    // P2.1 on
         delay_ms(1000, FREQ_3_MHZ);
         P2->OUT &= ~(1<<1);   // P2.1 off
         delay_ms(1000, FREQ_3_MHZ);
     }
}

void delay_ms(int ms, int freq){
    int num_cycles=(freq*ms*1000)/18;
    for(i=0; i<num_cycles;i++)
         {
            __delay_cycles(1);
         }
     }

void delay_us(int us, int freq){
    int num_cycles=(freq*us)/18;
    for(i=0; i<num_cycles;i++)
	     {
            __delay_cycles(1);
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


