#ifndef DRV_TST_H
#define DRV_TST_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/export.h>

extern ssize_t char_sgdma_drv_access_write(void);

#endif