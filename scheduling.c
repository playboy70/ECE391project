#include "scheduling.h"
#include "i8259.h"
#include "terminal.h"
#include "paging.h"

volatile int32_t active_tid = 0;
volatile int32_t next_sched_tid;

/*
 * init_PIT: initialize PIT, set up the I/O port
 * and enable the interrupt
 * Input: none
 * Output: none
 * Return value: none
 * Side_effect: enable the IRQ_0
 * 
*/
void init_PIT(){
    /* Tell PIT that we're setting CHANNEL0 */
    outb(PIT_MODE_REG, PIT_COMMAND_PORT);
    /* Send low byte */
    outb(PIT_FREQ_DIVISOR & HIGH_BYTE_MASK, PIT_CHANNEL0);
    /* Send high byte */
    outb(PIT_FREQ_DIVISOR >> HIGH_BITS_SHIFT, PIT_CHANNEL0);
    enable_irq(PIT_IRQ);
}

/*
 * pit_handler: when the interrupt comes, switch the process
 * Input: none
 * Output: switch the process
 * Return value: none
 * Side_effect: check the PIT interrupt and call process switch
 * 
*/
void pit_handler(){
    send_eoi(PIT_IRQ);
    int32_t next_process = reschedule();
    process_switch(next_process);
}

/*
 * process_switch: switch from current process to the nect process,
 * store the previous information to PCB and get the new PCB based on 
 * the process id
 * Input: none
 * Output: switch the process, change the PCB
 * return value: 0 means success, -1 means fail
 * Side_effect: switch the process
 * 
*/
int32_t process_switch(uint32_t process){
    terminal_t cur_terminal = terminals[active_tid];
    terminal_t next_terminal = terminals[next_sched_tid];

    int32_t cur_pid = cur_terminal.cur_pid;
    pcb_t* cur_pcb = EIGHT_MB - EIGHT_KB * (1 + cur_pid);
    pcb_t* next_pcb = EIGHT_MB - EIGHT_KB * (1 + process);

    /* Video memory mapping */
    /* If the next scheduled terminal is currently displayed */
    if(cur_display_tid == next_sched_tid){
        vid_remapping(VIDEO);
    }else{
        vid_mapping((uint32_t)next_terminal.vid_buf);
    }

    /* Set up paging for the next process */
    map_process(PROGRAM_IMAGE, EIGHT_MB + EIGHT_KB * process);

    /* context switch */
    /* First store contents to enable restoration of the cur process state for the next time slice */
    asm volatile("			                \n\
			            movl %%ebp, %%eax 	\n\
			            movl %%esp, %%ebx 	\n\
			    "
			    :"=a"(cur_pcb->saved_ebp), "=b"(cur_pcb->saved_esp));   /* outputs */

    /* Save SS0 and ESP0 in TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - EIGHT_KB * (process) - 4;

    /* Update current running terminal and stack info */
    active_tid = next_sched_tid;
    asm volatile("			                \n\
			            movl %%eax, %%ebp 	\n\
			            movl %%ebx, %%esp 	\n\
			    "
                :
			    :"=a"(next_pcb->saved_ebp), "=b"(next_pcb->saved_esp));   /* inputs */

    return 0;
    
}

/*
 * reschedule: decide which process should be executed next and  
 * rearrange the schedule list
 * Input: none
 * Output: none
 * Return value: process id of next process
 * Side_effect: rearrange the next process
 * 
*/
int32_t reschdule(){
    next_sched_tid = (active_tid + 1) % terminal_total_num;
    terminal_t* next_terminal = terminals[next_sched_tid];
    uint32_t next_sched_pid = next_terminal.cur_pid;
    return next_sched_pid;
}















