#ifndef GPIO_DRV_H
#define GPIO_DRV_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/export.h>

/* API to access driver operations within the kernel space */
void driver_toggle_gpio(char state);

#endif