#ifndef DRV_TST_H
#define DRV_TST_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/export.h>

/* cross-driver API */
extern ssize_t egress_thread_add_work(const char *buf, size_t count);

#endif