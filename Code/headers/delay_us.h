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
