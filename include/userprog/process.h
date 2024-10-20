#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
bool setup_stack (struct intr_frame *if_);
#if VM
struct lazy_load_argument{
	struct file *file;
	off_t ofs;
	uint8_t *upage;
	size_t page_read_bytes;
	size_t page_zero_bytes;
	bool writable;
};
#endif

tid_t process_create_initd (const char *file_name);
tid_t process_fork (const char *name, struct intr_frame *if_);
int process_exec (void *f_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (struct thread *next);

#endif /* userprog/process.h */
