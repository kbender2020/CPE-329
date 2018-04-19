#include "msp.h"
#include <string.h>
#include "../headers/delay_us.h"
#include "../headers/frequency.h"
#include "../headers/lcdInit.h"
#include "../headers/setDCO.h"
#include "../headers/keypad.h"

#define correct_pass "1234"
#define meme_pass "6969"

void init ();
int compare_string(char str1[], char str2[]);

void main(void){
    P4 -> DIR |= 0xFF;                          //Set all of P4 as outputs
    P4 -> OUT &= 0x00;                          //Set all outputs to 0
    init();
    int num_times = 0;
    while (num_times < 5){
        clear_LCD();
        home_LCD();
        write_string_LCD(" Enter Passcode: ");
        P4 -> OUT = 0xC0;
        nibble();
        P4 -> OUT = 0x00;
        nibble();
        char passcode[] = "????";
        int i = 0;
        char key;
        while(1){
            key = keypad_getkey();
            delay_us(200000, FREQ_3_MHZ);
            if (key != 0x00){
                write_char_LCD(key);
                passcode[i] = key;
                i = i+1;
            }
            if (passcode[3] != '?'){break;}
        }
        if (compare_string(correct_pass, passcode)){
            clear_LCD();
            write_string_LCD(" Congratulations ");
            delay_us(1000000, FREQ_3_MHZ);
        }
        else if (compare_string(meme_pass, passcode)){
            clear_LCD();
            write_string_LCD(" MEMELORD DETECTED");
            delay_us(1000000, FREQ_3_MHZ);
        }
        else {
            clear_LCD();
            write_string_LCD(" Try again :(   ");
            delay_us(1000000, FREQ_3_MHZ);
            num_times = num_times + 1;
        }
    }
    write_string_LCD(" LOCKED OUT     ");
    while(1){}
}

int compare_string(char str1[], char str2[]){
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 != len2){return 0;}
    for (i=0; i<len1; i++){
        if (str1[i]!=str2[i]){return 0;}
    }
    return 1;
}

void init(){
    init_LCD();
    init_keypad();
    clear_LCD();
    home_LCD();
}
