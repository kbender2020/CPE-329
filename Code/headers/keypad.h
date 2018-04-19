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
