#ifndef DRV_TST_H
#define DRV_TST_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/export.h>

extern void driver_toggle_gpio(char state);

#endif