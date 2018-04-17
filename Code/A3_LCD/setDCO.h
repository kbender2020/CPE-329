void set_DCO(int freq){

    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register CTL0
    // select clock sources
    switch(freq){
    case FREQ_1_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_0;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_3_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_1;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_6_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_2;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_12_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_3;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
        break;
    case FREQ_24_MHZ:
        CS->CTL0 = CS_CTL0_DCORSEL_4;
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
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
