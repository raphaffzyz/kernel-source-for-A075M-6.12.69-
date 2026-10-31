#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/pid.h>
#include "nb9_abi.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Voce");
MODULE_DESCRIPTION("NB9 Kernel Driver");

// NOME MUDADO PARA O CHEAT ENCONTRAR
#define DEVICE_NAME "sa_log_wifi"
#define CLASS_NAME  "sa_log_class"

static int major_number;
static struct class* nb9_class = NULL;
static struct device* nb9_device = NULL;
static struct cdev nb9_cdev;

static int rw_process_memory(pid_t pid, unsigned long addr, void *buf, size_t size, bool write) {
    struct task_struct *task;
    struct mm_struct *mm;
    int ret;

    if (pid <= 0 || !addr || !buf || !size) return -EINVAL;

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        rcu_read_unlock();
        return -ESRCH;
    }
    mm = get_task_mm(task);
    rcu_read_unlock();

    if (!mm) return -EINVAL;

    ret = access_process_vm(task, addr, buf, size, write ? FOLL_WRITE : 0);
    mmput(mm);

    if (ret != size) return -EFAULT;
    return 0;
}

static long nb9_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    void __user *uarg = (void __user *)arg;
    COPY_MEMORY cm;
    HOOK_STATUS status;
    int ret = 0;

    switch (cmd) {
        case OP_CMD_HOOK_STATUS:
            memset(&status, 0, sizeof(status));
            status.active = 1;
            status.armed = 1;
            if (copy_to_user(uarg, &status, sizeof(status))) return -EFAULT;
            return 0;

        case OP_CMD_READ:
        case OP_CMD_READ_SAFE:
            if (copy_from_user(&cm, uarg, sizeof(cm))) return -EFAULT;
            ret = rw_process_memory(cm.pid, cm.addr, cm.buffer, cm.size, false);
            return ret;

        case OP_CMD_WRITE:
        case OP_CMD_WRITE_SAFE:
            if (copy_from_user(&cm, uarg, sizeof(cm))) return -EFAULT;
            ret = rw_process_memory(cm.pid, cm.addr, cm.buffer, cm.size, true);
            return ret;

        default:
            return -ENOTTY;
    }
}

static int nb9_open(struct inode *inodep, struct file *filep) { return 0; }
static int nb9_release(struct inode *inodep, struct file *filep) { return 0; }

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = nb9_open,
    .release = nb9_release,
    .unlocked_ioctl = nb9_ioctl,
};

static int __init nb9_init(void) {
    printk(KERN_INFO "NB9: Iniciando /dev/%s...\n", DEVICE_NAME);

    if (alloc_chrdev_region(&major_number, 0, 1, DEVICE_NAME) < 0) return -1;

    nb9_class = class_create(CLASS_NAME);
    if (IS_ERR(nb9_class)) {
        unregister_chrdev_region(major_number, 1);
        return PTR_ERR(nb9_class);
    }

    nb9_device = device_create(nb9_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(nb9_device)) {
        class_destroy(nb9_class);
        unregister_chrdev_region(major_number, 1);
        return PTR_ERR(nb9_device);
    }

    cdev_init(&nb9_cdev, &fops);
    if (cdev_add(&nb9_cdev, MKDEV(major_number, 0), 1) < 0) {
        device_destroy(nb9_class, MKDEV(major_number, 0));
        class_destroy(nb9_class);
        unregister_chrdev_region(major_number, 1);
        return -1;
    }

    printk(KERN_INFO "NB9: Sucesso! /dev/%s criado.\n", DEVICE_NAME);
    return 0;
}

static void __exit nb9_exit(void) {
    cdev_del(&nb9_cdev);
    device_destroy(nb9_class, MKDEV(major_number, 0));
    class_destroy(nb9_class);
    unregister_chrdev_region(major_number, 1);
    printk(KERN_INFO "NB9: Driver descarregado.\n");
}

module_init(nb9_init);
module_exit(nb9_exit);
