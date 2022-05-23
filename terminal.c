#include "lib.h"
#include "terminal.h"
#include "keyboard_handler.h"

/* void init_terminal
 * Inputs:  None
 * Return Value: None
 * Function: Initialize terminal arrays*/
void init_terminal(){
    int i;
    for(i=0; i < Terminal_total_num; i++){
        terminals[i].tid = i;
        terminals[i].cur_pid = -1;
    }
};


/* int32_t terminal_close(int32_t fd)
 * Inputs:  filename
 * Return Value: 0 on success -1 on failure
 * Function: end terminal */
int32_t terminal_close(int32_t fd) {
    return -1;
}


/* int32_t terminal_open(const uint8_t* filename)
 * Inputs:  filename
 * Return Value: 0 on success -1 on failure
 * Function: build terminal */
int32_t terminal_open(const uint8_t* filename) {
	return 0;
}


/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function: put content from saved_keyboard_buffer to buf */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    if (buf == NULL)
        return -1;
    uint8_t* buffer = (uint8_t*) buf;
    int char_number=0;
    int i;
    while(1){
        if(1==return_enter_flag()){
            saved_keyboard_buffer[saved_buffer_position]='\n'; // \n will occur in the end of keyboard buffer
            saved_buffer_position++;
            break;
        }
    }
    for(i=0;i<=buffer_size;i++){
        buffer[i]=saved_keyboard_buffer[i]; //put content from saved_keyboard_buffer to buf
        if('\n'==saved_keyboard_buffer[i] || i == nbytes-1){//detect whether saved_keyboard_buffer ends or nbytes is satisfied
            char_number = i+1;
            break;
        }
    }
    reset_enter_flag();//after all ends clear flag
    buffer_clear();
    return char_number;
}


/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs:  file directory, buffer, filename
 * Return Value: number of data from buffer
 * Function: output content from buf */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    if (buf == NULL)
        return -1;
    uint8_t* buffer = (uint8_t*)buf;    
    int32_t i=0;

    cli();//block interrupt

    for(i=0;i<nbytes;i++){
        if(i==0){
            special_putc('\0',buffer[i]);//check if it is the first element in buffer
        }
        else{
            special_putc(buffer[i-1],buffer[i]);
        }
        
    }

    sti();
    return i;
}
