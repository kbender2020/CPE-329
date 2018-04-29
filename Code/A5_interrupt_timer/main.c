#include "msp.h"
#include "../headers/setDCO.h"
#include "../headers/delay_us.h"

int count;
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //stop watchdog timer

    P1->DIR |= BIT0;                                //RED LED1
    P2->DIR |= BIT2;                                //ISR Bit BLUE LED2

    P4->DIR |= BIT3;                                //Setting up MCLK to output on P4.3
    P4->SEL1 &= ~BIT3;
    P4->SEL0 |= BIT3;

    P1->OUT |= BIT0;
    P2->OUT |= BIT2;

    set_DCO(FREQ_1_MHZ);

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;       //enable the interrupt for TACCR0
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;       //enable the interrupt for TACCR1
    TIMER_A0->CCR[0] = 0x0FFF;
    TIMER_A1->CCR[1] = 1500;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;


    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);    //enable NVIC and global interrupts
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
    __enable_irq();

    while(1){}
}

void TA0_0_IRQHandler(void){
    //P2->OUT |= BIT2;                               //Turn on BLUE LED (Start of ISR)
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;       //Clear interrupt flag

    //Two bit Counter Code
    delay_us(195, FREQ_1_5_MHZ);
    P2->OUT ^= BIT2;
    TIMER_A0->CCR[0] += 750;

    //20 second clock
    count += 1;
    if(count == 229){
        P1->OUT ^= BIT0;
        count = 0;
    }

    //Duty Cycle code
    if(P1->OUT &= BIT0){
        P1->OUT &= ~BIT0;
        TIMER_A0->CCR[0] += 85;
    }else{
        P1->OUT |= BIT0;
        TIMER_A0->CCR[0] += 85;
    }
    //P2->OUT &= ~BIT2;                               //Turn off BLUE LED (end of ISR)
}

void TA0_N_IRQHandler(void){
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
    P1->OUT ^= BIT0;
    TIMER_A0->CCR[1] += 1500;
}

