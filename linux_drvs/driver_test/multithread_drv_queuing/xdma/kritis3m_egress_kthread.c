#include "kritis3m_egress_kthread.h"


#define DEFAULT_BUFFER_SIZE 128
#define WRITE_DEVICE_DEFAULT "/dev/xdma0_h2c_0"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION("Linux Kernel Thread, which processes network packets and transmits them to via DMA.");

/* Global Variables */
static struct kritis3m_egress_queue* egress_queue;


static int egress_thread_main(void * thread_nr) 
{
	struct xdma_cdev *xcdev = (struct xdma_cdev *)egress_queue->filp->private_data;
	loff_t pos=0;

	while(!kthread_should_stop())
	{
		msleep(5000);

		//drv_access_char_sgdma_write(xcdev, src_buffer, DEFAULT_BUFFER_SIZE, &pos);
	}

	return 0;
}

static ssize_t init_cdev_fileptr(void)
{
	egress_queue->filp = filp_open(WRITE_DEVICE_DEFAULT, O_RDWR, 0);
	if (IS_ERR(egress_queue->filp)) {
		printk(KERN_ERR "Failed to open device driver file (write job)\n");
		goto FILP_ERROR;
	}
	
	printk(KERN_INFO "File opened, associated f_op: %p\n", egress_queue->filp->f_op);

	return 0;

FILP_ERROR:
	return -EEXIST;
}


static int __init my_init(void)
{	
	printk("file_access - Loading kritis3m_egress_thread\n");

	/* allocate driver struct and populate the associated paramteters */
	egress_queue = kzalloc(sizeof(struct kritis3m_egress_queue), GFP_KERNEL);
	if (!egress_queue) return -ENOMEM;

	/* initialize file pointer to associated character device */
	if(init_cdev_fileptr() != 0) goto FILP_ERROR;

	/* initial kernel thread to process egress queue */
	egress_queue->egress_kthread = kthread_create(egress_thread_main, NULL, "egress_kthread");
	if(egress_queue->egress_kthread != NULL)
	{
		/* Let's start the thread */
		wake_up_process(egress_queue->egress_kthread);
		printk("egress_kthread -- Thread was created and is running now!\n");
	}
	else 
	{
		printk("egress_kthread -- Thread could not be created!\n");
		goto THP_ERROR;
	}

	return 0;

THP_ERROR:
	filp_close(egress_queue->filp, NULL);

FILP_ERROR:
	return -1;
}

static void __exit my_exit(void)
{
	printk("file_access - Removing kritis3m_egress_thread\n");
	
	kthread_stop(egress_queue->egress_kthread);

	if(!IS_ERR(egress_queue->filp))
	{
		filp_close(egress_queue->filp, NULL);
	}

	kfree(egress_queue);
}

module_init(my_init);
module_exit(my_exit);
