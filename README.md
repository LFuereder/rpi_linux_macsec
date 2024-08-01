Raspberry Pi Linux MACsec
=========================

This fork of the Raspberry Pi Linux Kernel 6.6.25 aims to implement a hardware accelerated MACsec kernel module for use in a Raspberry Pi 5 and a Zynq UltraScale+ MPSoC ZCU106. The current state of implementation contains a patched version of the Xilinx direct memory access driver (XDMA) for arm32 and arm64 implemented by [Laurence Barker](https://github.com/laurencebarker/Saturn/tree/main). In addition to this we integrated the MACsec kernel module implemented by [Sabrina Dubroca](https://man7.org/linux/man-pages/man8/ip-macsec.8.html). 


Initial Setup:
To setup the MACsec kernel module in Raspbian, a couple of dependencies need to be configured. The SD-card used to boot the Raspberry Pi needs to contain a version of the OS, which already contains the correct formatting. This can be done using the Raspberry Pi Imager Tool. 

Following this, the development environment needs to be established and a number of development tools need to be installed. This is achieved by executing the following commands:

```console
$ sudo apt install git bc bison flex libssl-dev make
```

```console
$ sudo apt install crossbuild-essential-arm64
```

After configuring the SD-card and the installation of the tools, we can now clone the current version of Linux from the Raspberry Pi Repository. As we fully replace the kernel in the future steps of this guide, it is not relevant to comply to the same version of the kernel as written on the SD-card. To clone the repository, we execute the following command:

```console
$ git clone --depth=1 https://github.com/raspberrypi/linux
```


## Kernel Preparation:
This guide proceeds with the configuration of a 64-bit Linux build. If a different architecture is needed we reference the official  [Linux Kernel Guide](https://www.raspberrypi.com/documentation/computers/linux_kernel.html). In the next step we adapt the predefined configuration for the associated build configuration. Depending on the hardware used for the MACsec communication this command varies in the following way:

If the Raspberry Pi 5 is used the used configuration is named ```bcm2712_defconfig```. For Raspberry Pi models 3, 3+, 4, 400 and Zero 2 W this configuration is named ```bcm2711_defconfig```. In case a different hardware is used or the 32-bit configuration is desired, we reference the original [Linux Kernel Guide](https://www.raspberrypi.com/documentation/computers/linux_kernel.html). 

Per default after cloning the repository, the build configurations are stored in the following path:

```console
$ cd ~/{WORKSPACE}/linux/arch/arm64/configs
```

As we want to adapt the build configuration to build the MACsec kernel module, we need to insert the following line into the associated build configuration:

```console
CONFIG_MACSEC=m
```

After inserting the line above into the file we safe and close it to execute the following commands (Keep in mind, this guide portrays the module configuration for the Raspberry Pi 5 with a 64-bit architecture. These commands deviates for different hardware):

```console
$ cd linux
$ KERNEL=kernel_2712
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2712_defconfig
```

This execute builds the sources and Device Tree files, which enable us to fully build the Linux image with the associated modules and Device Tree Blob Sources (dtbs). This can be done with the following command:

```console
$ make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```


## Write to SD-card:
To write to the SD-card, we need to mount the partitions to two different folders, which are later used to install the operating system. Therefore we create the folder ```mnt/```, which contains the two folders ```fat32/```(for the bootfs partition) and ```ext4/ ```(for the rootfs partition) and mount the associated SD-card. 

```console
$ mkdir mnt
$ mkdir mnt/fat32
$ mkdir mnt/ext4
$ sudo mount /dev/sda1 mnt/fat32
$ sudo mount /dev/sda2 mnt/ext4
```

If the name of the SD-card is not known at this point, we use the following command to display the connected devices:

```console
$ lsblk
```

In the next step we install the modules onto the mounted SD-card partition. This is achieved by executing the following command:

```console
$ sudo env PATH=$PATH make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=mnt/ext4 modules_install
```

Following this, we copy the kernel and the device tree blobs onto the SD-card and disconnect the two partitions:

```console
$ sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
$ sudo cp arch/arm64/boot/Image mnt/fat32/$KERNEL.img
$ sudo cp arch/arm64/boot/dts/broadcom/*.dtb mnt/fat32/
$ sudo cp arch/arm64/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
$ sudo cp arch/arm64/boot/dts/overlays/README mnt/fat32/overlays/
$ sudo umount mnt/fat32
$ sudo umount mnt/ext4
```

## Activation of the MACsec kernel module:
Lastly, we boot the Raspberry Pi with the operating system written onto the SD-card and execute the following command:

```console
$ sudo modprobe macsec
```

If done correctly, this should activate the MACsec kernel module and make encrypted layer 2 communication possible. The article [[MACsec module usage]] explains the usage of the MACsec kernel module in more detail.
