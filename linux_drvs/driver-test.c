#include <linux/module.h>
#include <linux/pci.h>

#define PCI_TEST_VENDOR_ID 0x10ec
#define PCI_TEST_DEVICE_ID 0x818b

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


/* The following function is called, if a device is registered */
static int pci_test_device_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    printk("pci_test_driver currently in the probe step\n");
    return 0;
}


/* The following function is called, if a device is unregistered */
static void pci_test_device_remove(struct pci_dev *dev)
{
    printk("pci_test_driver currently in the remove step\n");
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
    printk("pci_test_driver - Registering the PCI device\n");
    return pci_register_driver(&pci_test_driver);
}

/* Exit function of the driver */
static void __exit my_exit(void) {
    printk("pci_test_driver - Unregistering the PCI device\n");
    pci_unregister_driver(&pci_test_driver);
}

module_init(my_init);
module_exit(my_exit);
