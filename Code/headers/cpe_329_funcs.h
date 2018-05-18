//P1 Functions (software delays, setting DCO, LCD functions, Keypad functions)
#include <string.h>

#define FREQ_1_MHZ 1
#define FREQ_1_5_MHZ 1.5
#define FREQ_3_MHZ 3
#define FREQ_6_MHZ 6
#define FREQ_12_MHZ 12
#define FREQ_24_MHZ 24
#define FREQ_48_MHZ 48

void set_DCO(int freq){

    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register CTL0
    // select clock sources
    switch(freq){
    case FREQ_1_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_0;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
        break;
    case FREQ_3_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_1;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
        break;
    case FREQ_6_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_2;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
        break;
    case FREQ_12_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_3;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
        break;
    case FREQ_24_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_4;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;
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

//Microsecond delay
int i;

void delay_us(int us, float freq){
    if(us>0){
    int num_cycles=(freq*us)/18;
    for(i=0; i<num_cycles;i++){
           __delay_cycles(1);
        }
    }
}

//nibble() writes four bits to the LCD by first enabling E, sending the data, then disabling E
void nibble(){
    P4 -> OUT |= BIT2;
    delay_us(37, FREQ_24_MHZ);
    P4 -> OUT &= ~BIT2;
}

//LCD Initialization Process
void init_LCD(){
    //Set Function
    delay_us(30000, FREQ_24_MHZ);
    P4 -> OUT |= 0x20;                    //Function Set (upper nibble): This sets LCD in 4 bit mode
    nibble();                             //Write
    P4 -> OUT &= 0x20;                    //Send data again bc LCD is stupid
    nibble();
    P4 -> OUT |= 0x80;                    //Function set (lower nibble): Sets number of lines and font size (2 lines 5x8)
    nibble();

    P4 -> OUT &= 0x00;                    //Display ON/OFF Control (upper nibble)
    nibble();
    P4 -> OUT |= 0xF0;                    //Display ON/OFF Control (lower nibble): Display ON, Cursor ON, Blink ON
    nibble();

    P4 -> OUT &= 0x00;                    //Display Clear
    nibble();
    P4 -> OUT |= 0x10;
    P4 -> OUT |=  BIT2;                   //Longer nibble (>1.52ms) needed for clearing display
    delay_us(1520, FREQ_24_MHZ);
    P4 -> OUT &= ~BIT2;

    P4 -> OUT &= 0x00;                    //Entry Mode Set (upper nibble)
    nibble();
    P4 -> OUT |= 0x70;                    //Entry Mode Set (lower nibble)
    nibble();
    }

//Writes individual characters to the LCD
void write_char_LCD(char character){
    char char_upper;
    char char_lower;
    char_upper = character;
    char_lower = character << 4;               //Shift the lower nibble in character to the upper nibble
    P4 -> OUT = char_upper & 0xF0;             //Write upper nibble of character to the LCD
    P4 -> OUT |= BIT0;                         //RS = 1
    nibble();
    P4 -> OUT = char_lower & 0xF0;             //Write the lower nibble of character to the LCD
    P4 -> OUT |= BIT0;                         //RS = 1
    nibble();
}

//Clears the LCD Display of all characters
void clear_LCD(){
    P4 -> OUT &= 0x00;
    nibble();
    P4 -> OUT |= 0x10;
    nibble();
}


void home_LCD(){
    P4 -> OUT = 0x00;
    nibble();
    P4 -> OUT = 0x10;
    nibble();
}

void second_line_LCD(){
    P4->OUT = 0xC0;
    nibble();
    P4->OUT = 0x00;
    nibble();
}

void write_string_LCD(char string[]){       //Maximum string length is 32 characters for LCD (2 lines, 16 characters each)
    int j;
    int len = strlen(string);
    /*if(len > 16){
        for (j=0; j<16; j++){
            write_char_LCD(string[j]);
            delay_us(10000, FREQ_24_MHZ);
        }
        P4 -> OUT = 0xC0;                       //Set DDRAM address to 0x40 (start of second line of LCD)
        nibble();
        P4 -> OUT = 0x00;
        nibble();
        for (j=16; j<len; j++){
            write_char_LCD(string[j]);
            delay_us(10000, FREQ_24_MHZ);
        }
    }
    else{*/
        for (j=0; j<len+1; j++){
            write_char_LCD(string[j]);
            delay_us(10000, FREQ_24_MHZ);
        }
    //}
}

void init_keypad(){
    P5 -> DIR = 0xF0;   //Only make the rows outputs, column pins will be inputs
    P5 -> REN = 0x0F;   //Enable pull-up resistors for columns pins (lower nibble 0,1,2 of P5 are columns)
    P5 -> OUT = 0x0F;   //Make column pins pull-ups
}

char keypad_getkey(){
    int row;
    int col;
    int key;
    const char row_select[] = {0x10, 0x20, 0x40, 0x80};

    P5 -> DIR |= 0xF0;               //Row pins are outputs
    P5 -> OUT &= ~0xF0;              //Drive all row pins low
    col = P5 -> IN & 0x0F;           //Read all column pins
    P5 -> OUT |= 0xF0;               //Set all rows high before disabling them
    P5 -> DIR &= ~0xF0;              //Disable all row pins
    if (col == 0x0F)                 //if all columns are high
        return 0x00;                 //no key has been pressed

    for(row = 0; row < 4; row++){
        P5 -> DIR &= 0xF0;              //disable all rows
        P5 -> DIR |= row_select[row];   //enable one row at a time
        P5 -> OUT &= ~row_select[row];  //drive the active row low
        col = P5 -> IN & 0x07;          //read all columns
        P5 -> OUT |= row_select[row];   //drive active row high
        if (col != 0x07) break;         //if any column input is low, a key has been pressed
        }

    P5 -> OUT |= 0xF0;
    P5 -> DIR &= 0xF0;
    if (row == 4) return 0x00;

    //generates unique numbers associated to each row/column combination {1,2,3,5,6,7,9,10,11,13,14,15}
    if (col == 0x05) key = row*4+1;
    if (col == 0x06) key = row*4+2;    //col1 pin3
    if (col == 0x03) key = row*4+3;    //col3 pin5

    const char actual_nums[] = {'1','2','3','!','*','0','#','!','7','8','9','!','4','5','6'}; //actual ASCII characters we want to write to the LCD
    return actual_nums[key-1];
}

int poll_poundkey(void){
    int col;
    P5->DIR &= 0xF0;
    P5->DIR |= BIT5;
    P5->OUT &= ~BIT5;
    col = P5->IN & BIT2;  //read only last column
    if (col == 0){return 0;}
    else{return 1;}
}

/***************************************************************************************************************************************************************/
//P2 Functions (SPI Initialization, DAC initialization, TimerA Initialization)
void config_SPI(void){
    // Configure port bits for SPI
    P4->DIR |= BIT3;
    P1->SEL0 |= BIT6 + BIT5;
    P1->SEL1 &= ~(BIT6 + BIT5);

    // SPI Setup
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;                     // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST   |                   // Remain eUSCI state machine in reset
                      EUSCI_B_CTLW0_MST     |                   // Set as SPI master
                      EUSCI_B_CTLW0_SYNC    |                   // Set as synchronus mode
                      EUSCI_B_CTLW0_CKPL    |                   // Set clock polarity high
                      EUSCI_B_CTLW0_MSB;                        // MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;               // SMCLK
    EUSCI_B0->BRW = 0x01;                                       // divide by 16, clock = fBRCLK/(UCBRx)
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;                    // Initialize USCI state machine, SPI
                                                                // now waiting for something to
                                                                // be placed in TXBUF
}

//Drive_DAC writes necessary voltage level value to DAC
void Drive_DAC(unsigned int level){
    unsigned int DAC_Word = 0;

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

    P4->OUT |= BIT3;                                           // Set P4.1   (drive /CS high on DAC)

    return;
}

//Timer A Configuration Function
void config_TIMER_A(void){
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;                    // enable the interrupt for TACCR0 (square wave)
    // Initializing timer count
    TIMER_A0->CCR[0] = 10;
    //TIMER_A0->CCR[1] = 10;
    //TIMER_A0->CCR[2] = 10;

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    //TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    //TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;

    // enable NVIC and global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    //NVIC->ISER[1] = 1 << ((TA1_0_IRQn) & 31);
    //NVIC->ISER[2] = 1 << ((TA2_0_IRQn) & 31);
    __enable_irq();
}
