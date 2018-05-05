#include "msp.h"
#include "../headers/setDCO.h"

void Drive_DAC(unsigned level);
void config_SPI(void);
void config_TIMER_A(void);

int TempDAC_Value;

#define V0 0x000            // 0V voltage level
#define V1 0x4D9            // 1V voltage level
#define V2 0x9B2            // 2V voltage level
#define V3_3 0xFFF          // 3.3V voltage level
#define TIMER_VAL   100   // TIMER_A count time (60500 for 20ms square wave, 100 for 20ms Triangle wave)

signed int v_level;
int count;
signed int delta;
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

// Setup system clock speed, SPI interfacing and TIMER_A parameters
	set_DCO(FREQ_24_MHZ);
	config_SPI();
	config_TIMER_A();

    v_level = V0;                           // Initializing first voltage level to be 0V
    delta = 0x001;                          // Change in voltage level will be 0x001
    count = 0;
	while (1) {
	    Drive_DAC(v_level);                 // Write voltage levels to DAC
	    //TempDAC_Value += 100;             // Increment by 100 mV
	}

}
// end of main

//ISR toggles between 2V and 0V every 10ms designed to be a 2Vpp square wave with 1V DC offset
void TA0_0_IRQHandler(void){

    /*//Square Wave
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    count += 1;
    TIMER_A0->CCR[0] += TIMER_VAL;
    if ((count == 4) & (v_level == V0)){
        v_level = V2;
        count = 0;
    }else if((count == 4) & (v_level == V2)){
        v_level = V0;
        count = 0;
    }*/

    //Triangle Wave
    count += 1;
    if (count == 2488){                                        // After 2488 voltage level increases (0x9B2 == 2482) the DAC has made an even 2V sweep
        delta = ~delta + 1;                                    // Two's complement of delta (make it negative)
        count = 0;                                             // Reset count
    }
    v_level += delta;
    TIMER_A0->CCR[0] += TIMER_VAL;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

//Drive_DAC writes necessary voltage level value to DAC
void Drive_DAC(unsigned int level){
	unsigned int DAC_Word = 0;
	int i;

	DAC_Word = (0x7000) | (level & 0x0FFF);                    // 0x1000 sets DAC for Write
	                                                           // to DAC, Gain = 2, /SHDN = 1
	                                                           // and put 12-bit level value
	                                                           // in low 12 bits.

	P4->OUT &= ~BIT3;                                          // Clear P4.1 (drive /CS low on DAC)
	                                                           // Using a port output to do this for now

	EUSCI_B0->TXBUF = (unsigned char) (DAC_Word >> 8);         // Shift upper byte of DAC_Word
	                                                           // 8-bits to right

	while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));              // USCI_A0 TX buffer ready?

	EUSCI_B0->TXBUF = (unsigned char) (DAC_Word & 0x00FF);     // Transmit lower byte to DAC

	while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));              // Poll the TX flag to wait for completion

	for(i = 200; i > 0; i--);                                  // Delay 200 16 MHz SMCLK periods
	                                                           // to ensure TX is complete by SIMO

	P4->OUT |= BIT3;                                           // Set P4.1   (drive /CS high on DAC)

	return;
}

void config_SPI(void){
    // Configure port bits for SPI
    P4->DIR |= BIT3;
    P1->SEL0 |= BIT6 + BIT5;
    P1->SEL1 &= ~(BIT6 + BIT5);

    // SPI Setup
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;         // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST   |       // Remain eUSCI state machine in reset
                      EUSCI_B_CTLW0_MST     |       // Set as SPI master
                      EUSCI_B_CTLW0_SYNC    |       // Set as synchronus mode
                      EUSCI_B_CTLW0_CKPL    |       // Set clock polarity high
                      EUSCI_B_CTLW0_MSB;            // MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;   // SMCLK
    EUSCI_B0->BRW = 0x01;                           // divide by 16, clock = fBRCLK/(UCBRx)
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;        // Initialize USCI state machine, SPI
                                                    // now waiting for something to
                                                    // be placed in TXBUF
}

void config_TIMER_A(void){
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;       // enable the interrupt for TACCR0
    TIMER_A0->CCR[0] = TIMER_VAL;                 // one-third the required number of cycles for 10ms delay
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;


    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);     // enable NVIC and global interrupts
    __enable_irq();
}
