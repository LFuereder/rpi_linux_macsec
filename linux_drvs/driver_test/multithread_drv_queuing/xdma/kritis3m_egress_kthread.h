#ifndef EGRSS_THRT_H
#define EGRSS_THRT_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include "xdma_mod.h"


struct kritis3m_egress_queue {
    
    /* kernel task structure associated with thread */
    struct task_struct *egress_kthread;

    /* file pointer to access the xdma character driver */
    struct file *filp;

    /*  thread work list count */
	unsigned int work_cnt;
	
    /* thread work list count */
	struct list_head work_list;
};

/* cross-driver API */
extern ssize_t drv_access_char_sgdma_write(struct xdma_cdev *xcdev, const char *buf, size_t count, loff_t *pos);

#endif