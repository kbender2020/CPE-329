//nibble() writes four bits to the LCD by first enabling E, sending the data, then disabling E
void nibble(){
    P4 -> OUT |= BIT2;
    delay_us(37, FREQ_3_MHZ);
    P4 -> OUT &= ~BIT2;
}

//LCD Initialization Process
void init_LCD(){
    //Set Function
    delay_us(30000,FREQ_3_MHZ);
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
    delay_us(1520, FREQ_3_MHZ);
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

//Writes entire 32 character string to the LCD
void write_string_LCD(char string[]){       //Maximum string length is 32 characters for LCD (2 lines, 16 characters each)
    clear_LCD();
    int len = strlen(string);
    int j;
    j = 0;
    if(len > 16){
        for (j=0; j<16; j++){
            write_char_LCD(string[j]);
            delay_us(100000, FREQ_3_MHZ);
        }
        P4 -> OUT = 0xC0;                       //Set DDRAM address to 0x40 (start of second line of LCD)
        nibble();
        P4 -> OUT = 0x00;
        nibble();
        for (j=16; j<len; j++){
            write_char_LCD(string[j]);
            delay_us(100000, FREQ_3_MHZ);
        }
    }
    else{
        for (j=0; j<len+1; j++){
            write_char_LCD(string[j]);
            delay_us(100000, FREQ_3_MHZ);
        }
    }
}
