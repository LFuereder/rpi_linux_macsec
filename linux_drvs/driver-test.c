#include <linux/module.h>
#include <linux/pci.h>

#define PCI_TEST_VENDOR_ID 0x10ee
#define PCI_TEST_DEVICE_ID 0x7021
#define PCI_TEST_DEVICE_MEMOFFSET 0x0

void __iomem *ptr_mem0;

/* Meta Inforamtion */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lukas Fuereder");
MODULE_DESCRIPTION("Test application of the pci driver integration");


/* List of allowed devices */
static struct pci_device_id pci_test_driver_ids[] = {
    { PCI_DEVICE(PCI_TEST_VENDOR_ID, PCI_TEST_DEVICE_ID)},
    { }
};

MODULE_DEVICE_TABLE(pci, pci_test_driver_ids);


static void gather_device_info(struct pci_dev *dev)
{
    /* Gather information about memory space on device */
    int status = pci_resource_len(dev, 0);
    printk("---------------------------------------------------\n");
    printk("[ INFO ]   pci_test_driver BAR0 is %d bytes\n", status);
    printk("[ INFO ]   pci_test_driver BAR0 is mapped to 0x%llx\n", pci_resource_start(dev, 0));
    printk("---------------------------------------------------\n");
}

/* The following function is called, if a device is registered */
static int pci_test_device_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    ptr_mem0 = NULL;
    int status;

    printk("[ INFO ]   pci_test_driver currently in the probe step\n");
    gather_device_info(dev);

    /* Enable device */
    status = pcim_enable_device(dev);
    if(status < 0)
    {
        printk("[ ERROR ]   Could not enable pci_test_driver\n");
        return status;
    }

    status = pcim_iomap_regions(dev, BIT(0), KBUILD_MODNAME);
    if( status < 0)
    {
        printk("[ ERROR ]   Could not enable pci_test_driver\n");
        return status;
    }

    printk("[ INFO ]   Successfully enabled pci_test_driver\n");


    ptr_mem0 = pcim_iomap_table(dev)[0];
    if(ptr_mem0 == NULL)
    {
        printk("[ ERROR ]   Invalid pointer for BAR0\n");
        return -1;
    }

    printk("[ INFO ]   Value of ptr_mem0: 0x%p\n", ptr_mem0);

    printk("pci_test_driver mem0 state DWord: 0x%x\n", ioread32(ptr_mem0 + PCI_TEST_DEVICE_MEMOFFSET));
    iowrite8(0xb, ptr_mem0 + PCI_TEST_DEVICE_MEMOFFSET);
    
    return 0;
}


/* The following function is called, if a device is unregistered */
static void pci_test_device_remove(struct pci_dev *dev)
{
    printk("[ INFO ]   pci_test_driver currently in the remove step\n");

    printk("pci_test_driver mem0 state DWord: 0x%x\n", ioread32(ptr_mem0 + PCI_TEST_DEVICE_MEMOFFSET));
    iowrite8(0x0, ptr_mem0 + PCI_TEST_DEVICE_MEMOFFSET);
    printk("=======================================================\n");
    return;
}


/* PCI driver struct */
static struct pci_driver pci_test_driver = {
    .name = "pci_test_driver",
    .id_table = pci_test_driver_ids,
    .probe = pci_test_device_probe,
    .remove = pci_test_device_remove,
};

/* Init function of the driver */
static int __init my_init(void) {
    printk("[ INFO ]   pci_test_driver - Registering the PCI device\n");
    return pci_register_driver(&pci_test_driver);
}

/* Exit function of the driver */
static void __exit my_exit(void) {
    printk("[ INFO ]   pci_test_driver - Unregistering the PCI device\n");
    pci_unregister_driver(&pci_test_driver);
}

module_init(my_init);
module_exit(my_exit);
