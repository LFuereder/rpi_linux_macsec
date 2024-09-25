#include "kritis3m_egress_kthread.h"


#define DEFAULT_BUFFER_SIZE 128
#define WRITE_DEVICE_DEFAULT "/dev/xdma0_h2c_0"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION("Linux Kernel Thread, which processes network packets and transmits them to via DMA.");

/* Global Variables */
static struct kritis3m_egress_queue* egress_driver;
static int thread_ID=1;

/* initialization macro for the work list of the kernel thread*/
LIST_HEAD(WORK_LIST);

ssize_t egress_thread_add_work(const char *buf, size_t count)
{
	struct kritis3m_queue_element* new_element;

	/* lock thread to add element to the work list */
	spin_lock(&egress_driver->lock);

	new_element = kmalloc(sizeof(struct kritis3m_queue_element), GFP_KERNEL);
	if(new_element == NULL) return -ENOMEM;

	new_element->data_buf = kmalloc((count*sizeof(char)), GFP_KERNEL);
	if(new_element->data_buf == NULL) goto DATA_ALLOC_ERROR;
	
	memcpy(new_element->data_buf, buf, count);
	
	new_element->data_len = count;
	
	list_add_tail(&new_element->list, &WORK_LIST);
	egress_driver->work_cnt++;

	/* unlock thread and proceed with main routine */
	spin_unlock(&egress_driver->lock);

	return 0;

DATA_ALLOC_ERROR:
	kfree(new_element);
	return -ENOMEM;
}
EXPORT_SYMBOL(egress_thread_add_work);



static inline int kritis3m_work_pending(void)
{
	struct list_head *work_item, *next;

	/* any work items assigned to this thread? */
	if (list_empty(&WORK_LIST))
	{
		printk("[ INFO ] currently 0 elements in work list\n");
		return 0;
	}
	
	int num_of_entries=0;
	/* any work item has pending work to do? */
	list_for_each_safe(work_item, next, &WORK_LIST) 
	{
		num_of_entries++;
	}

	printk("[ INFO ] currently %d elements in work list\n", num_of_entries);
	return (num_of_entries ? 1 : 0);
}

static size_t process_work_item(struct kritis3m_queue_element* work_item)
{
	struct xdma_cdev *xcdev = (struct xdma_cdev *) egress_driver->filp->private_data;
	loff_t pos = 0;

	printk("[ INFO ] processing work item with %ld byte\n", work_item->data_len);
	return drv_access_char_sgdma_write(xcdev, work_item->data_buf, work_item->data_len, &pos);
}

static int egress_thread_main(void * thread_nr) 
{
	struct list_head *iterator, *next;
	struct kritis3m_queue_element* work_item;

	while(!kthread_should_stop())
	{
		msleep(1000);

		/* lock thread to add element to the work list */
		spin_lock(&egress_driver->lock);
		
		if(kritis3m_work_pending())
		{
			list_for_each_safe(iterator, next, &WORK_LIST)
			{
				/*execute DMA transfer of data */
				work_item = list_entry(iterator, struct kritis3m_queue_element, list);
				
				spin_unlock(&egress_driver->lock);
				ssize_t rv = process_work_item(work_item);
				if(rv < 0) goto DMA_TRANSMIT_ERR;

				/*delete current element from work list */
				printk("[ INFO ] removing work element from list\n");
				spin_lock(&egress_driver->lock);

				list_del(&work_item->list);
				egress_driver->work_cnt--;
				kfree(work_item);
			}
		}

		/* unlock thread after processing work items */
		spin_unlock(&egress_driver->lock);
	}

	return 0;

DMA_TRANSMIT_ERR:
	printk(KERN_ERR "DMA transmission unsuccessful\n");
	return -1;
}


static ssize_t init_cdev_fileptr(void)
{
	egress_driver->filp = filp_open(WRITE_DEVICE_DEFAULT, O_RDWR, 0);
	if (IS_ERR(egress_driver->filp)) {
		printk(KERN_ERR "Failed to open device driver file (write job)\n");
		goto FILP_ERROR;
	}
	
	printk(KERN_INFO "File opened, associated f_op: %p\n", egress_driver->filp->f_op);

	return 0;

FILP_ERROR:
	return -EEXIST;
}

static int __init init_egress_driver(void)
{	
	printk("file_access - Loading kritis3m_egress_thread\n");

	/* allocate driver struct and populate the associated paramteters */
	egress_driver = kzalloc(sizeof(struct kritis3m_egress_queue), GFP_KERNEL);
	if (!egress_driver) return -ENOMEM;

	/* initialize file pointer to associated character device */
	if(init_cdev_fileptr() != 0) goto FILP_ERROR;

	/* pass name to kernel thread */
	egress_driver->ID = thread_ID;

	/* initialize thread lock */
	spin_lock_init(&egress_driver->lock);

	/* initialize working list for the egress driver */
	egress_driver->work_cnt=0;
	
	/* initial kernel thread to process egress queue */
	egress_driver->egress_kthread = kthread_create(egress_thread_main, &thread_ID, "egress_kthread");
	if(egress_driver->egress_kthread != NULL)
	{
		/* Let's start the thread */
		wake_up_process(egress_driver->egress_kthread);
		printk("egress_kthread -- Thread was created and is running now!\n");
	}
	else 
	{
		printk("egress_kthread -- Thread could not be created!\n");
		goto THP_ERROR;
	}

	return 0;

THP_ERROR:
	filp_close(egress_driver->filp, NULL);

FILP_ERROR:
	return -1;
}

static void __exit exit_egress_driver(void)
{
	printk("file_access - Removing kritis3m_egress_thread\n");
	
	kthread_stop(egress_driver->egress_kthread);

	if(!IS_ERR(egress_driver->filp))
	{
		filp_close(egress_driver->filp, NULL);
	}

	kfree(egress_driver);
}

module_init(init_egress_driver);
module_exit(exit_egress_driver);
