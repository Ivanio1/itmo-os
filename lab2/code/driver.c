#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/netdevice.h>
#include <linux/vmalloc.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/ptrace.h>

#define WR_SIGNAL_STRUCT _IOW('a', 2, struct signal_struct_message*)
#define WR_SYSCALL_INFO _IOW('a', 3, struct syscall_info_message*)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivanio1");
MODULE_DESCRIPTION("Linux kernel driver (IOCTL)");
MODULE_VERSION("1.0");


void fill_signal_struct(void);
void fill_syscall_info(void);

int __init etx_driver_init(void);
void __exit etx_driver_exit(void);
long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int etx_open(struct inode *inode, struct file *file);
int etx_release(struct inode *inode, struct file *file);
ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t *off);

struct file_operations fops = {
        .owner = THIS_MODULE,
        .read = etx_read,
        .write = etx_write,
        .open = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release = etx_release,
};

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

struct my_seccomp_data {
	int nr;
	__u32 arch;
	__u64 instruction_pointer;
	__u64 args[6];
};

struct my_syscall_info {
	__u64			sp;
	struct my_seccomp_data	data;
};

struct syscall_info_message {
    struct my_syscall_info *msi;
    pid_t pid;
};

dev_t dev = 0;

struct class *dev_class;
struct cdev etx_cdev;
struct task_struct *ts;
struct signal_struct_message msg;
struct syscall_info_message msg2;


long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    if (cmd == WR_SYSCALL_INFO) {
    	if (copy_from_user(&msg2, (struct syscall_info_message *) arg, sizeof(msg2))) {
            pr_err("Data Write : Err!\n");
        }
        pr_info("Pid = %d\n", msg2.pid);
        fill_syscall_info();
    }
    if (cmd == WR_SIGNAL_STRUCT) {
        if (copy_from_user(&msg, (struct signal_struct_message *) arg, sizeof(msg))) {
            pr_err("Data Write : Err!\n");
        }
        pr_info("Pid = %d\n", msg.pid);
        fill_signal_struct();
    } else {
        pr_info("Default\n");
    }
    return 1;
}

int __init etx_driver_init(void) {
    /*Allocating Major number*/
    if ((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0) {
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&etx_cdev, &fops);

    /*Adding character device to the system*/
    if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
        pr_err("Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if ((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if ((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
        pr_err("Cannot create the Device 1\n");
        goto r_device;
    }
    pr_info("Device Driver Insert...Done\n");
    return 0;

    r_device:
        class_destroy(dev_class);
    r_class:
        unregister_chrdev_region(dev, 1);
        return -1;
}

void fill_syscall_info(){

}

void fill_signal_struct() {
    ts = get_pid_task(find_get_pid(msg.pid), PIDTYPE_PID);
    if (ts == NULL) {
        msg.ssi->valid = false;
    } else {
        msg.ssi->valid = true;
        msg.ssi->nr_threads = ts->signal->nr_threads;
        msg.ssi->group_exit_code = ts->signal->group_exit_code;
        msg.ssi->notify_count = ts->signal->notify_count;
        msg.ssi->group_stop_count = ts->signal->group_stop_count;
        msg.ssi->flags = ts->signal->flags;
    }
}

void __exit etx_driver_exit(void) {
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...!\n");
    pr_info("Done\n");
}

/* This function will be called when we open the Device file */
int etx_open(struct inode * inode, struct file * file) {
    pr_info("Device File Opened...\n");
    return 0;
}

/* This function will be called when we close the Device file */
int etx_release(struct inode * inode, struct file * file) {
    pr_info("Device File Closed...\n");
    return 0;
}

/* This function will be called when we read the Device file */
ssize_t etx_read(struct file * filp, char __user * buf, size_t len, loff_t * off) {
    pr_info("Read Function\n");
    return 0;
}

/* This function will be called when we write the Device file */
ssize_t etx_write(struct file * filp, const char __user * buf, size_t len, loff_t * off) {
    pr_info("Write function\n");
    return len;
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

