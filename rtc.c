#include "rtc.h"

/* Reference: https://wiki.osdev.org/RTC */

/* a volatile flat to enable synchronizations */
volatile int rtc_interrupt_occurred = 0;

/* rtc_init
 *   DESCRIPTION: Initialize the RTC and set the rate to default value 1024Hz
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Enable the RTC interrupt
 */
void rtc_init(void){
    outb((DIS_NMI|RTC_REGB), RTC_PORT_REG); // select register B, and disable NMI
    char prev = inb(RTC_PORT_CMOS);         // read the current value of register B
    outb((DIS_NMI|RTC_REGB), RTC_PORT_REG); // set the index again (a read will reset the index to register D)
    outb((prev|BIT_SIX), RTC_PORT_CMOS);    // turns on bit 6 of register B

    /* set freq to 2Hz and rate to 15 */
    rtc_change_irqrate(DEFAULT_FREQ);
    
    enable_irq(RTC_IRQ);
}

/* rtc_handler
 *   DESCRIPTION: RTC interrupt handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Read Status Register C to handle RTC interrupts
 */
void rtc_handler(){
    /* upon a IRQ 8, Status Register C will 
     * contain a bitmask telling which interrupt happened 
     */
    outb(RTC_REGC, RTC_PORT_REG);
    inb(RTC_PORT_CMOS);
    // uncomment the follow instruction to enable testing RTC
    // test_interrupts();

    /* set rtc flag */
    rtc_interrupt_occurred = 1;

    send_eoi(RTC_IRQ);
}

/* rtc_open
 *   DESCRIPTION: initilize RTC frequency to 2Hz
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: always 0 for success
 *   SIDE EFFECTS: RTC frequency change to 2Hz
 */
int32_t rtc_open (const uint8_t* filenme){
    
    /* initilize RTC frequency to 2Hz */
    rtc_change_irqrate(2);

    return 0; //always 0
}

/* rtc_open
 *   DESCRIPTION: do nothing
 *   INPUTS: fd - file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: always 0 for success
 *   SIDE EFFECTS: none
 */
int32_t rtc_close (int32_t fd){
    /* does nothing */
    return 0;
}

/* rtc_read
 *   DESCRIPTION: Wait for the next rtc interrupt
 *   INPUTS: fd - file descriptor num
 *           buf - data pointer
 *           nbytes - numer of bytes to be read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 until next interrupt raised
 *   SIDE EFFECTS: none
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes){

    rtc_interrupt_occurred = 0;
    
    /* block until the next interrupt */
    while(!rtc_interrupt_occurred);

    /* next interrupt raised */
    return 0;
}

/* rtc_write
 *   DESCRIPTION: Change rtc freq based on the input value that buf points to
 *   INPUTS: fd - file descriptor num
 *           buf - data pointer
 *           nbytes - numer of bytes to be written
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: Change rtc freq if success
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes){
    /* check valid inputs 
     * should always accepy only a 4-byte integer 
     */
    if ((buf == NULL) || (nbytes != 4)) { return -1; }
    int32_t freq = *(int32_t*)buf;

    /* change rtc rate based on input freq from buf pointer
     * valid freq check in rtc_change_irqrate function
     * 0 for success, -1 for failure
     */
    return rtc_change_irqrate(freq);
}

/* rtc_chnge_irqrate
 *   DESCRIPTION: Change rtc rate based on the input freq
 *   INPUTS: int32_t freq - input RTC freq
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for success, -1 for failure
 *   SIDE EFFECTS: Change RTC rate
 */
int rtc_change_irqrate(int32_t freq){
    int32_t rate = freq_to_rate(freq);

    if (rate == -1)
    {
        return -1;
    }
    outb((DIS_NMI|RTC_REGA), RTC_PORT_REG);
    char prev = inb(RTC_PORT_CMOS);
    outb((DIS_NMI|RTC_REGA), RTC_PORT_REG);
    outb(((prev&0xF0)|rate), RTC_PORT_CMOS);
    return 0;
}

/* freq_to_rate
 *   DESCRIPTION: Helper function for calculating rate from frequency
 *   INPUTS: int32_t freq - input RTC freq
 *   OUTPUTS: int32_t - calculated rate
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int32_t freq_to_rate(int32_t freq){
    /* frequency =  32768 (2^15) >> (rate-1) */
    /* rate must be above 2 and not over 15 */
    switch (freq)
    {
    case 2:
        return 15;
    case 4:
        return 14;
    case 8:
        return 13;
    case 16:
        return 12;
    case 32:
        return 11;
    case 64:
        return 10;
    case 128:
        return 9;
    case 256:
        return 8;
    case 512:
        return 7;
    case 1024:
        return 6;
    default:
        return -1;
    }
}
