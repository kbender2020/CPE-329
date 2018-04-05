

#include "msp.h"
void delay_ms(int ms);
   
int main(void) {

   P2->SEL1 &= ~BIT1;      // set P2.1 as GPIO
   P2->SEL0 &= ~BIT1;      // set P2.5 as GPIO
   P2->DIR  |= BIT1;       // set P2.1 as output

   while (1) {
      P2->OUT |= BIT1;    // P2.1 on
      delay_ms(500);
      P2->OUT &= ~BIT1;   // P2.1 off
      delay_ms(500);
    }
}

// Delay milliseconds function
void delay_ms(int ms) {
    int i, j;
    for (j = 0; j < ms; j++)
        for (i = 750; i > 0; i--);   // delay 1 ms (approx)
} 