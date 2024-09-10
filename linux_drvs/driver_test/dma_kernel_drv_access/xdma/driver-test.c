#include "driver-test.h"
#include "cdev_sgdma.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION("Generic Test Driver, if probed accesses the gpio_driver and modifies its current entry.");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void)
{
	printk("file_access - Loading driver\n");
	char_sgdma_drv_access_write();
	
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
