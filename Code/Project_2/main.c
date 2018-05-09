#include "msp.h"
#include "../headers/cpe_329_funcs.h"
#include <string.h>
/* Pinouts:
 * P4 Struct is assigned to output to LCD Display (exception being P4.3, which is the SPI for the DAC)
 *
 */

#define NUM_SAMPLES 1000
#define V0 0x000            // 0V voltage level
#define V1 0x4D9            // 1V voltage level
#define V2 0x9B2            // 2V voltage level
#define V3_3 0xFFF          // 3.3V voltage level

int * get_waveform_parameters(void);
int string_to_int(char string[]);
int gen_sin(double t, int freq);

static int *data;
static int period;
static int on_time;
static int off_time;
static int normalized_period;
static int normalized_on_time;
static int count;
static int poll = 1;
static int v_level;
static double sin_array[NUM_SAMPLES];
static int saw_timer;
static int stuff[3];

void main(void)
{   //Initialization Sequence
    set_DCO(FREQ_24_MHZ);                                     // Set everything to run off of 24MHZ clock
    P4 -> DIR |= 0xFF;                                      // Set all of P4 as outputs to LCD (and 4.3 as SPI)
    P4 -> OUT &= 0x00;                                      // Set all outputs to 0
    config_SPI();
    init_LCD();
    init_keypad();
    data = get_waveform_parameters();
    config_TIMER_A();

    while(1){
        while(poll){
            Drive_DAC(v_level);
            poll = poll_poundkey();
        }

        __disable_irq();
        data = get_waveform_parameters();
        period = 24000000/data[2];                          // Number of clock cycles per period at 24MHz data[2] = frequency
        count = 0;
        if (data[0] == '1'){
            on_time = (period*data[1])/100;                 // Number of clock cycles voltage is high
            off_time = period - on_time;
            normalized_period = period/1000;                // Normalization number is chosen to be 1000
            normalized_on_time = on_time/1000;
            TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;         // enable the interrupt for TACCR0
            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;       // disable the interrupt for TACCR1
            TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIE;       // disable the interrupt for TACCR2
        }
        if (data[0] == '2'){
            double t = 1/NUM_SAMPLES;
            int i;
            for (i=0; i<NUM_SAMPLES; i++){
                sin_array[i] = gen_sin(t,data[2]);
            }
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;       // disable the interrupt for TACCR0
            TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;         // enable the interrupt for TACCR1
            TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIE;        // disable the interrupt for TACCR2
        }
        if (data[0] == '3'){
            saw_timer = period/4096;
            TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIE;       // disable the interrupt for TACCR0
            TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIE;       // disable the interrupt for TACCR1
            TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CCIE;          // enable the interrupt for TACCR2
        }
        poll = 1;
        __enable_irq();
    }
}
// Square Wave ISR
void TA0_0_IRQHandler(void){
    if (period > 0xFFFF){
        TIMER_A0->CCR[0] += 1000;
        count++;
        if  ((v_level == V3_3) & (count == normalized_on_time)){
            v_level = V0;
        }else if ((v_level == V0) & (count == normalized_period)){
            v_level = V3_3;
            count = 0;
        }
    }else{
        if (v_level == V0){                                 // Turn on statement
            v_level = V3_3;
            TIMER_A0->CCR[0] += on_time;
        }else{                                              // Turn off statement
            v_level = V0;
            TIMER_A0->CCR[0] += off_time;
        }
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;              //  Clear Interrupt Flag
}
// Sine Wave ISR
void TA1_0_IRQHandler(void){
    count++;
    if (count > NUM_SAMPLES){count = 0;}
    v_level = sin_array[count];
    TIMER_A0->CCR[1] += period/NUM_SAMPLES;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;
}
// Sawtooth Wave ISR
void TA2_0_IRQHandler(void){
    v_level += 0x001;
    TIMER_A0->CCR[2] += saw_timer;
    if (v_level == 0xFFF){v_level = V0;}
    TIMER_A0->CCR[2] += saw_timer;
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
}


int * get_waveform_parameters(void){
    int waveform;
    int duty_cycle;
    int frequency;
    int thing = 1;
    write_string_LCD(" Choose Waveform: ");
    second_line_LCD();
    write_string_LCD("1:SQ 2:SIN 3:SAW ");
    while(thing){
        waveform = keypad_getkey();
        if(waveform == 0x00){
            thing= 1;
        }else{thing = 0;}
    }
//Get Frequency and Duty Cycle
    char key;
    int i = 0;
    // Get duty cycle if Square wave
    if (waveform == '1'){
        clear_LCD();
        home_LCD();
        write_string_LCD(" Duty Cycle (%): ");
        second_line_LCD();
        char d_cycle[] = "??";
        char key;
        int i = 0;
        while(1){
            key = keypad_getkey();
            delay_us(200000, FREQ_24_MHZ);
            if (key!= 0x00){
                write_char_LCD(key);
                d_cycle[i] = key;
                i += 1;
            }
            if (d_cycle[1] != '?'){
                duty_cycle = string_to_int(d_cycle);
                break;}
            }
    }
    //Get Frequency
    delay_us(200000, FREQ_24_MHZ);
    clear_LCD();
    home_LCD();
    write_string_LCD(" Frequency:      ");
    second_line_LCD();
    i = 0;
    char freq_in[] = "???";
    while(1){
        key = keypad_getkey();
        delay_us(200000, FREQ_24_MHZ);
        if(key != 0x00){
            write_char_LCD(key);
            freq_in[i] = key;
            i += 1;
        }
        if (freq_in[2] != '?'){
            frequency = string_to_int(freq_in);
            break;
        }
    }

    delay_us(200000, FREQ_24_MHZ);
    clear_LCD();
    home_LCD();
    write_string_LCD(" 1:Hz 2:kHZ 3:MHz");
    while(1){
        key = keypad_getkey();
        delay_us(200000, FREQ_24_MHZ);
        if(key == '1'){
           frequency = frequency * 1;
           break;
        }
        if(key == '2'){
           frequency = frequency * 1000;
           break;
        }
        if(key == '3'){
           frequency = frequency * 1000000;
           break;
        }
    }
    clear_LCD();
    stuff[0] = waveform;
    stuff[1] = duty_cycle;
    stuff[2] = frequency;
    return stuff;
}

int string_to_int(char string[]){
    int j, x, y;
    int len = strlen(string) - 1;
    for (j=0; j<=len; j++){
        x = string[j] - 48;                     // converts ascii number to actual number
        y += x*10^(len-j);                      // for decimal place of the number
    }
    return y;
}

int gen_sin(double t, int freq){
    double sin_val;
    double voltage;
    sin_val = t*6.28319*freq - (248.05*t*freq*t*freq*t*freq)/6 + (9792.63*t*freq*t*freq*t*freq*t*freq*t*freq)/120;
    voltage = 0xFFF/(sin_val+1);
    return voltage;
}
