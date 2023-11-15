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

#define WR_VALUE _IOW('a', 2, struct signal_struct_message*)

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
    
    
    /* signal_struct */
    
    if (argc < 2) {
        printf("\nNot enough arguments for signal_struct. Enter PID\n");
    } else {
    
    	struct signal_struct_message msg;
    	struct signal_struct_info ssi;

    	msg.pid = atoi(argv[1]);
    	msg.ssi = &ssi;
    
   	ioctl(fd, WR_VALUE, (struct signal_struct_message *) &msg);
    	if (msg.ssi->valid == true) {
        	printf("\nsignal_struct_info for PID %d: \n", msg.pid);
        	printf("\tnr_threads = %d\n", msg.ssi->nr_threads);
       	 printf("\tgroup_exit_code = %d\n", msg.ssi->group_exit_code);
        	printf("\tnotify_count = %d\n", msg.ssi->notify_count);
       	 printf("\tgroup_stop_count = %d\n", msg.ssi->group_stop_count);
        	printf("\tflags = %d\n", msg.ssi->flags);
    	} else printf("\ntask_struct for pid %d is NULL. Can not get any information\n", msg.pid);
    
    }

    printf("\nClosing Driver...\n");
    printf("\n###################################\n\n");
    close(fd);
}
