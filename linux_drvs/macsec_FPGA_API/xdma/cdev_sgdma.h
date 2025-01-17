/*
 * This file is part of the Xilinx DMA IP Core driver for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */

#ifndef _XDMA_IOCALLS_POSIX_H_
#define _XDMA_IOCALLS_POSIX_H_

#include <linux/ioctl.h>


#define IOCTL_XDMA_PERF_V1 (1)
#define XDMA_ADDRMODE_MEMORY (0)
#define XDMA_ADDRMODE_FIXED (1)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 *
 * _IO(type,nr)		    no arguments
 * _IOR(type,nr,datatype)   read data from driver
 * _IOW(type,nr.datatype)   write data to driver
 * _IORW(type,nr,datatype)  read/write data
 *
 * _IOC_DIR(nr)		    returns direction
 * _IOC_TYPE(nr)	    returns magic
 * _IOC_NR(nr)		    returns number
 * _IOC_SIZE(nr)	    returns size
 */

struct xdma_performance_ioctl {
	/* IOCTL_XDMA_IOCTL_Vx */
	uint32_t version;
	uint32_t transfer_size;
	/* measurement */
	uint32_t stopped;
	uint32_t iterations;
	uint64_t clock_cycle_count;
	uint64_t data_cycle_count;
	uint64_t pending_count;
};



/* IOCTL codes */

#define IOCTL_XDMA_PERF_START   _IOW('q', 1, struct xdma_performance_ioctl *)
#define IOCTL_XDMA_PERF_STOP    _IOW('q', 2, struct xdma_performance_ioctl *)
#define IOCTL_XDMA_PERF_GET     _IOR('q', 3, struct xdma_performance_ioctl *)
#define IOCTL_XDMA_ADDRMODE_SET _IOW('q', 4, int)
#define IOCTL_XDMA_ADDRMODE_GET _IOR('q', 5, int)
#define IOCTL_XDMA_ALIGN_GET    _IOR('q', 6, int)

/* cross-driver API */

/// @brief drv_access_write represents an interface for access to the xdma pci driver within the linux kernel space.
///		   This function does not contain any implementation for copy from/to userspace and can therefore not be used 
///		   in user space applications. For kernel space applications it works identical to the .write callback function 
///		   of the cdev_ctrl LKM.

/// @param xcdev reference to the xdma character device, which shall be used for transmission 
/// @param buf pointer to the buffer containing the message, which shall be transmitted (only valid for kernel memory) 
/// @param count number of bytes, which shall be transmitted
/// @param pos optional position offset, which can be added on top of the current flash-memory (on the FPGA) position.

/// @return returns the number of transmitted bytes or -EEXIST in case of an error while trying to access the file pointer.
ssize_t drv_access_char_sgdma_write(struct xdma_cdev *xcdev, const char *buf, size_t count, loff_t *pos);

/// @brief drv_access_read represents an interface for access to the xdma pci driver within the linux kernel space.
///		   This function does not contain any implementation for copy from/to userspace and can therefore not be used 
///		   in user space applications. For kernel space applications it works identical to the .write callback function 
///		   of the cdev_ctrl LKM.

/// @param buf pointer to the buffer, which shall be used for the received data (only valid for kernel memory) 
/// @param count number of bytes, which are expected
/// @param pos optional position offset, which can be added on top of the current flash-memory (on the FPGA) position.

/// @return returns the number of transmitted bytes or -EEXIST in case of an error while trying to access the file pointer.
ssize_t drv_access_char_sgdma_read(const char *buf, size_t count, loff_t *pos);

#endif /* _XDMA_IOCALLS_POSIX_H_ */
