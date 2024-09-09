#ifndef GPIO_DRV_H
#define GPIO_DRV_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/export.h>

void driver_toggle_gpio(void);

#endif