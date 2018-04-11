

#include "msp.h"
#include "msp432.h"

#define FREQ_1_MHZ 1
#define FREQ_1_5_MHZ 1.5
#define FREQ_3_MHZ 3
#define FREQ_6_MHZ 6
#define FREQ_12_MHZ 12
#define FREQ_24_MHZ 24
#define FREQ_48_MHZ 48

void delay_ms(int ms, float freq);
void delay_us(int us, float freq);
void set_DCO(int freq);
int i;

void main(void)
{
     P2->SEL1 &= ~0x02;      // set P2.1 as GPIO
     P2->SEL0 &= ~0x02;      // set P2.5 as GPIO
     P2->DIR  |= 0x02;       // set P2.1 as output
     set_DCO(FREQ_3_MHZ);

     /*for(i=0;i<2;i++){
         P2->OUT |= (1<<1);    // P2.1 on
         delay_us(100, FREQ_3_MHZ);
         P2->OUT &= ~(1<<1);   // P2.1 off
         delay_us(100, FREQ_3_MHZ);
     }*/

     while(1) {
         P2->OUT |= (1<<1);    // P2.1 on
         delay_us(1, FREQ_3_MHZ);
         P2->OUT &= ~(1<<1);   // P2.1 off
         delay_us(1, FREQ_3_MHZ);
     }
}

//Delay milliseconds function
void delay_ms(int ms, float freq){
    if(ms>0){
    int num_cycles=(freq*ms*1000)/18;
    for(i=0; i<num_cycles;i++)
         {
            __delay_cycles(1);
         }
     }
}

//Delay microseconds function
void delay_us(int us, float freq){
    if(us>0){
    int num_cycles=(freq*us)/18;
    for(i=0; i<num_cycles;i++){
           __delay_cycles(1);
        }
    }
}


//Set Frequency
void set_DCO(int freq){

    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register CTL0
    // select clock sources
    switch(freq){
    case FREQ_1_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_0;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_3_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_1;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_6_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_2;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_12_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_3;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_24_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_4;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_48_MHZ:
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
        ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
        ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
        CS->CTL0 = CS_CTL0_DCORSEL_5;
        CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) | CS_CTL1_SELM_3;
        break;
    }
    CS->KEY = 0; // lock the CS registers
}
