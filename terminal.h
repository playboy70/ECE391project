#ifndef TERMINAL_H
#define TERMINAL_H

#define Terminal_total_num  3

/* Multiple terminals */
typedef struct terminal_t{
    int32_t    tid; 
    int32_t    cur_pid;
    uint8_t*   vid_buf;
}terminal_t;

extern terminal_t terminals[Terminal_total_num];
extern volatile int32_t cur_display_tid;

void init_terminal();

/* Terminal system calls */
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

#endif