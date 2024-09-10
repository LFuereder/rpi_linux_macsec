#include "gpio_driver.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION("A simple gpio driver for setting a LED and reading its current state");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"

#define GPIO_PIN 592

/**
 * @brief Read data out of the buffer
 */
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta;
	char tmp[3] = " \n";

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(tmp));

	/* Read value of button */
	printk("Value of button: %d\n", gpio_get_value(GPIO_PIN));
	tmp[0] = gpio_get_value(GPIO_PIN) + '0';

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, &tmp, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Writing the value parsed from user space memory onto the gpio register
 * 		  (as long as it is within the valid states)
 * 
 * @param state This value originates in the driver_write callback_function below and is referenced in the function. 
 * 
 * @return ssize_t If executed correctly, this function returns 0. In case an invalid value is passed to it, this
 * 				   function returns the error code: -EIO.  
 */
void driver_toggle_gpio(char state)
{
	switch(state) {
		case '0':
			gpio_set_value(GPIO_PIN, 0);
			break;
		
		case '1':
			gpio_set_value(GPIO_PIN, 1);
			break;
		
		default:
			printk("Invalid State!\n");
			break;
	}
}

EXPORT_SYMBOL(driver_toggle_gpio);

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) 
{
	int to_copy, not_copied, delta;
	char value;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(value));

	/* Copy data to user */
	not_copied = copy_from_user(&value, user_buffer, to_copy);

	/* Write copied value to the associated register */
	driver_toggle_gpio(value);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev_nr - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev_nr - close was called!\n");
	return 0;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");

	/* Allocate a device nr */
	if( alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}
	printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

	/* Create device class */
	if((my_class = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device class can not be created!\n");
		goto ClassError;
	}

	/* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	/* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

	/* GPIO Pin init */
	if(gpio_request(GPIO_PIN, "rpi-gpio-14")) {
		printk("Can not allocate GPIO %d\n", GPIO_PIN);
		goto AddError;
	}

	/* Set GPIO Pin direction */
	if(gpio_direction_output(GPIO_PIN, 0)) {
		printk("Can not set GPIO %d to output!\n", GPIO_PIN);
		goto GpioError;
	}

	return 0;

GpioError:
	gpio_free(GPIO_PIN);

AddError:
	device_destroy(my_class, my_device_nr);

FileError:
	class_destroy(my_class);

ClassError:
	unregister_chrdev_region(my_device_nr, 1);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	gpio_set_value(GPIO_PIN, 0);
	gpio_free(GPIO_PIN);

	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev_region(my_device_nr, 1);
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

