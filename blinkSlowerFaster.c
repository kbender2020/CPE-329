/* p2_1.c Toggling green LED in C using header file register definitions.
 * This program toggles green LED for 0.5 second ON and 0.5 second OFF.
 * The green LED is connected to P2.1.
 * The LEDs are high active (a '1' turns ON the LED).
 *
 * Tested with Keil 5.20 and MSP432 Device Family Pack V2.2.0
 * on XMS432P401R Rev C.
 */

#include "msp.h"

#define DELAY10MS 30000u // DELAY * 1 mS assuming 3 MHz clock

void delayMs(int n);


int main(void) {

    uint32_t counter = 1;
    uint8_t i = 0;
    P2->SEL1 &= ~2;         /* configure P2.1 as simple I/O */
    P2->SEL0 &= ~2;
    P2->DIR |= 2;           /* P2.1 set as output pin */


    while (1)
    {

            while (counter <= 32)
            {
            P2->OUT |= (1<<1);       /* turn on P2.1 green LED */
                for(i=0; i<counter; i++)
                __delay_cycles(DELAY10MS);


            P2->OUT &= ~(1<<1);      /* turn off P2.1 green LED */
                for(i=0; i<counter; i++)
                __delay_cycles(DELAY10MS);

            counter = counter << 1;
            }
            while (counter >= 1)
            {
            P2->OUT |= (1<<1);       /* turn on P2.1 green LED */
                for(i=0; i<counter; i++)
                __delay_cycles(DELAY10MS);


            P2->OUT &= ~(1<<1);      /* turn off P2.1 green LED */
                for(i=0; i<counter; i++)
                __delay_cycles(DELAY10MS);

            counter = counter >> 1;
            }
            counter = 1;
    } // end while(1)

} // end main

