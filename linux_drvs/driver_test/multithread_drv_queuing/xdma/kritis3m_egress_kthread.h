#ifndef EGRSS_THRT_H
#define EGRSS_THRT_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/string.h>

#include "xdma_mod.h"


/* Struct Definitions */
struct kritis3m_egress_queue 
{
    /* thread ID */
    int ID;

    /* thread lock*/
	spinlock_t lock;

    /* kernel task structure associated with thread */
    struct task_struct *egress_kthread;

    /* file pointer to access the xdma character driver */
    struct file *filp;

    /*  thread work list count */
	unsigned int work_cnt;
};

struct kritis3m_queue_element
{
    struct list_head list;

    char* data_buf;

    size_t data_len;
};


/* API to the egress queue */
ssize_t egress_thread_add_work(const char *buf, size_t count);

/* XDMA cross-driver API */
extern ssize_t drv_access_char_sgdma_write(struct xdma_cdev *xcdev, const char *buf, size_t count, loff_t *pos);

#endif