#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/ptrace.h>

#define WR_SIGNAL_STRUCT _IOW('a', 2, struct signal_struct_message*)
#define WR_SYSCALL_INFO _IOW('a', 3, struct syscall_info_message*)

struct signal_struct_info {
    bool valid;
    int nr_threads;
    int group_exit_code;
    int notify_count;
    int group_stop_count;
    unsigned int flags;
};

struct signal_struct_message {
    struct signal_struct_info *ssi;
    pid_t pid;
};

struct my_syscall_info {
    bool valid;
    unsigned long  sp;
    int nr;
    unsigned int arch;
    unsigned long instruction_pointer;
    unsigned long args[6];
};

struct syscall_info_message {
    struct my_syscall_info *msi;
    pid_t pid;
};


int main(int argc, char *argv[]) {
  
    int fd;

    printf("\n##################################\n");
    printf("\nOpening Driver...\n");

    fd = open("/dev/etx_device", O_RDWR);
    if (fd < 0) {
        printf("Cannot open device file...\n");
        return 0;
    }
    printf("Writing data...\n\n");
    
    
    /* syscall_info */
    if (argc < 2) {
        printf("\nNot enough arguments for syscall_info. Enter PID\n");
    } else {
   
    	struct syscall_info_message messg;
    	struct my_syscall_info msi;

    	messg.pid = atoi(argv[1]);
    	messg.msi = &msi;
    
   	ioctl(fd, WR_SYSCALL_INFO, (struct syscall_info_message *) &messg);
    	if (messg.msi->valid == true) {
	    printf("\nsyscall_info for PID %d: \n", messg.pid);
	    printf("\tStack pointer: %lu\n", messg.msi->sp);
	    printf("\tArchitecture: %u\n", messg.msi->arch);
            printf("\tInstruction pointer: %lu\n", messg.msi->instruction_pointer);
    	    printf("\tThe system call number: %d\n", messg.msi->nr);
    	    printf("\tSyscall arguments:\n\t\t1. %lu\n\t\t2. %lu\n\t\t3. %lu\n\t\t4. %lu\n\t\t5. %lu\n\t\t6. %lu\n",
           messg.msi->args[0],
           messg.msi->args[1],
           messg.msi->args[2],
           messg.msi->args[3],
           messg.msi->args[4],
           messg.msi->args[5]);
		
    	} else printf("\nsyscall_info for pid %d is NULL. Can not get any information\n", messg.pid);
    
    }
    
    
    
    /* signal_struct */
    
    if (argc < 2) {
        printf("\nNot enough arguments for signal_struct. Enter PID\n");
    } else {
    
    	struct signal_struct_message msg;
    	struct signal_struct_info ssi;

    	msg.pid = atoi(argv[1]);
    	msg.ssi = &ssi;
    
   	ioctl(fd, WR_SIGNAL_STRUCT, (struct signal_struct_message *) &msg);
    	if (msg.ssi->valid == true) {
        	printf("\nsignal_struct_info for PID %d: \n", msg.pid);
        	printf("\tNr threads = %d\n", msg.ssi->nr_threads);
       	 printf("\tGroup exit code = %d\n", msg.ssi->group_exit_code);
        	printf("\tNotify count = %d\n", msg.ssi->notify_count);
       	 printf("\tGroup stop count = %d\n", msg.ssi->group_stop_count);
        	printf("\tFlags = %d\n", msg.ssi->flags);
    	} else printf("\nsignal_struct for pid %d is NULL. Can not get any information\n", msg.pid);
    
    }

    printf("\nClosing Driver...\n");
    printf("\n###################################\n\n");
    close(fd);
}
