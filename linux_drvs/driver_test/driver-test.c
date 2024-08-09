#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/fs.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION(
	"Generic Test Driver, if probed accesses the gpio_driver and modifies its current entry.");

/** Variable Declarations */
static struct timer_list my_timer;
char my_gpio_state;

/**
 * @brief Callback function of the Timer.
 */
void timer_callback(struct timer_list *data)
{
	struct file *file;
	ssize_t len;

	/* Open the file */
	file = filp_open("/dev/my_gpio_driver", O_RDWR, 0);
	if (!file) {
		printk("file_access - Error opening file\n");
		return;
	}

	/* Write to the file */
	len = kernel_write(file, &my_gpio_state, sizeof(my_gpio_state), &file->f_pos);
	if (len < 0) {
		printk("file_access - Error writing to file: %ld\n", len);
        goto FileError;
	}

	switch (my_gpio_state) {
	case 0:
		my_gpio_state = 1;
		break;

	case 1:
		my_gpio_state = 0;
		break;

	default:
		my_gpio_state = 0;
		break;
	}

FileError:
   	filp_close(file, NULL);
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void)
{
    /* Initialize timer */
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void)
{
	printk("file_access - Unloading driver\n");
}

module_init(my_init);
module_exit(my_exit);
