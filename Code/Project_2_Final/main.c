#include "msp.h"
#include "../headers/cpe_329_funcs.h"
#include <string.h>
#include <math.h>
/* Pinouts:
 * P4 Struct is assigned to output to LCD Display (exception being P4.3, which is the SPI for the DAC)
 *
 */

#define V0 0x000            // 0V voltage level
#define V1 0x4D9            // 1V voltage level
#define V2 0x9B2            // 2V voltage level
#define V3_3 0xFFF          // 3.3V voltage level

void get_waveform_parameters(int stuff[]);
int string_to_int(int array[]);

static int data[3];
static int period;
static int on_time;
static int normalized_period;
static int normalized_on_time;
static int count;
static int v_level;
static int saw_timer;
static int sin_timer;
static int sin_array[100] = {2048,2176,2304,2431,2557,2680,2801,2919,
                             3034,3145,3251,3353,3449,3540,3625,3704,
                             3776,3842,3900,3951,3995,4031,4059,4079,
                             4091,4095,4091,4079,4059,4031,3995,3951,
                             3900,3842,3776,3704,3625,3540,3449,3353,
                             3251,3145,3034,2919,2801,2680,2557,2431,
                             2304,2176,2048,1919,1791,1664,1538,1415,
                             1294,1176,1061,950,844,742,646,555,
                             470,391,319,253,195,144,100,64,
                             36,16,4,0,4,16,36,64,
                             100,144,195,253,319,391,470,555,
                             646,742,844,950,1061,1176,1294,1415,
                             1538,1664,1791,1919};

static int saw_array[101] = {0,41,82,123,164,205,246,287,328,369,410,
                            450,491,532,573,614,655,696,737,778,819,
                            860,901,942,983,1024,1065,1106,1147,1188,
                            1229,1270,1310,1351,1392,1433,1474,1515,
                            1556,1597,1638,1679,1720,1761,1802,1843,
                            1884,1925,1966,2007,2048,2089,2129,2170,
                            2211,2252,2293,2334,2375,2416,2457,2498,
                            2539,2580,2621,2662,2703,2744,2785,2826,
                            2867,2908,2948,2989,3030,3071,3112,3153,
                            3195,3235,3276,3317,3358,3399,3434,3481,
                            3522,3563,3604,3645,3686,3723,3737,3808,
                            3849,3890,3931,3972,4013,4054,4095};

void main(void)
 {  //Initialization Sequence
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;             //stop watchdog timer
    set_DCO(FREQ_24_MHZ);                                   // Set everything to run off of 24MHZ clock
    P4 -> DIR |= 0xFF;                                      // Set all of P4 as outputs to LCD (and 4.3 as SPI)
    P4 -> OUT &= 0x00;                                      // Set all outputs to 0
    config_SPI();
    init_LCD();
    init_keypad();
    v_level = V0;
    data[0] = '2';
    normalized_on_time = 50;
    normalized_period = 100;
    period = 240000;
    config_TIMER_A();
    int poll = 1;

    while(1){
        while(poll){
            //Drive_DAC(v_level);
            poll = poll_poundkey();
        }

        __disable_irq();
        get_waveform_parameters(data);
        period = 24000000/data[2];                          // Number of clock cycles per period at 24MHz data[2] = frequency
        count = 0;

        if (data[0] == '1'){
            on_time = (period*data[1])/100;                 // Number of clock cycles voltage is high
            normalized_period = period/1000;                // Normalization number is chosen to be 1000
            normalized_on_time = on_time/1000;
            v_level = V0;
        }
        if (data[0] == '2'){sin_timer = period/100;}
        if (data[0] == '3'){saw_timer = period/101;}

        poll = 1;
        __enable_irq();
    }
}
// Square Wave ISR
void TA0_0_IRQHandler(void){
    if (data[0] == '1'){
        TIMER_A0->CCR[0] += 1000;
        count++;
        if  ((v_level == V3_3) & (count == normalized_on_time)){
            v_level = V0;
        }else if ((v_level == V0) & (count == normalized_period)){
            v_level = V3_3;
            count = 0;
        }
        Drive_DAC(v_level);
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;              // Clear Interrupt Flag
    }
    else if (data[0] == '2'){
        TIMER_A0->CCR[0] += sin_timer;
        count++;
        if (count == 100){count = 0;}
        v_level = sin_array[count];
        Drive_DAC(v_level);
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
    else if (data[0] == '3'){
        count++;
        TIMER_A0->CCR[0] += saw_timer;
        if (count == 101){count = 0;}
        v_level = saw_array[count];
        Drive_DAC(v_level);
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    }
}


void get_waveform_parameters(int stuff[]){
    int waveform;
    int duty_cycle;
    int frequency;
    int thing = 1;
    write_string_LCD("Choose Waveform: ");
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
        int d_cycle[] = {'?', '?', '?'};
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
            if (d_cycle[2] != '?'){
                duty_cycle = string_to_int(d_cycle);
                break;}
            }
    }
    //Get Frequency
    delay_us(200000, FREQ_24_MHZ);
    clear_LCD();
    home_LCD();
    write_string_LCD(" Frequency (Hz): ");
    second_line_LCD();
    i = 0;
    int freq_in[] = {'?', '?', '?'};
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

    delay_us(300000, FREQ_24_MHZ);
    clear_LCD();
    stuff[0] = waveform;
    stuff[1] = duty_cycle;
    stuff[2] = frequency;
}

int string_to_int(int array[]){
    int i;
    int x = 0;
    int y = 0;
    for (i=0; i<3; i++){
        x = array[i] - 48;
        y += x*pow(10, 2-i);
    }
    return y;
}
