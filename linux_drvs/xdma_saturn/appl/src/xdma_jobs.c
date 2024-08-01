/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

#define _BSD_SOURCE
#define _XOPEN_SOURCE 500
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../tools/dma_utils.c"

#define DEVICE_NAME_DEFAULT "/dev/xdma0_h2c_0"
#define DATA_PATH_DEFAULT "../data/datafile_4k.bin"
#define DATA_OUTPUT_PATH_DEFAULT = "../data/data_output.bin"

#define SIZE_DEFAULT (32)
#define COUNT_DEFAULT (1)


int dma_writejob(char *devname, uint64_t addr, uint64_t size, uint64_t offset, uint64_t count,
		    char *infname, char *ofname)
{
	uint64_t i;
	ssize_t rc;
	size_t bytes_done = 0;
	size_t out_offset = 0;
	char *buffer = NULL;
	char *allocated = NULL;
	struct timespec ts_start, ts_end;
	int infile_fd = -1;
	int outfile_fd = -1;
	int fpga_fd = open(devname, O_RDWR);
	long total_time = 0;
	float result;
	float avg_time = 0;
	int underflow = 0;

	if (fpga_fd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n",
			devname, fpga_fd);
		perror("open device");
		return -EINVAL;
	}

	if (infname) {
		infile_fd = open(infname, O_RDONLY);
		if (infile_fd < 0) {
			fprintf(stderr, "unable to open input file %s, %d.\n",
				infname, infile_fd);
			perror("open input file");
			rc = -EINVAL;
			goto out;
		}
	}

	if (ofname) {
		outfile_fd =
		    open(ofname, O_RDWR | O_CREAT | O_TRUNC | O_SYNC,
			 0666);
		if (outfile_fd < 0) {
			fprintf(stderr, "unable to open output file %s, %d.\n",
				ofname, outfile_fd);
			perror("open output file");
			rc = -EINVAL;
			goto out;
		}
	}

	posix_memalign((void **)&allocated, 4096 /*alignment */ , size + 4096);
	if (!allocated) {
		fprintf(stderr, "OOM %lu.\n", size + 4096);
		rc = -ENOMEM;
		goto out;
	}
	buffer = allocated + offset;
	if (verbose)
		fprintf(stdout, "host buffer 0x%lx = %p\n",
			size + 4096, buffer); 

	if (infile_fd >= 0) {
		rc = read_to_buffer(infname, infile_fd, buffer, size, 0);
		if ((rc < 0) || ((uint64_t) rc < size))
			goto out;
	}

	for (i = 0; i < count; i++) {
		/* write buffer to AXI MM address using SGDMA */
		rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);

		rc = write_from_buffer(devname, fpga_fd, buffer, size,
				      	 	addr);
		if (rc < 0)
			goto out;

		bytes_done = rc;
		if (!underflow && bytes_done < size)
			underflow = 1;

		rc = clock_gettime(CLOCK_MONOTONIC, &ts_end);

		/* subtract the start time from the end time */
		timespec_sub(&ts_end, &ts_start);
		total_time += ts_end.tv_nsec;
		
		/* a bit less accurate but side-effects are accounted for */
		if (verbose)
		fprintf(stdout,	"#%lu: CLOCK_MONOTONIC %ld.%09ld sec. write %ld bytes\n",
					 i, ts_end.tv_sec, ts_end.tv_nsec, size); 
			
		if (outfile_fd >= 0) {
			rc = write_from_buffer(ofname, outfile_fd, buffer,
						 bytes_done, out_offset);
			if ((rc < 0) || ((size_t) rc < bytes_done))
				goto out;
			out_offset += bytes_done;
		}
	}

	if (!underflow) 
	{
		avg_time = (float)total_time/(float)count;
		result = ((float)size)*1000/avg_time;
		if (verbose)
			printf("** Avg time device %s, total time %ld nsec, avg_time = %f, size = %lu, BW = %f \n",
			devname, total_time, avg_time, size, result);
		printf("%s ** Average BW = %lu, %f\n", devname, size, result);
	}

out:
	close(fpga_fd);
	if (infile_fd >= 0)
		close(infile_fd);
	if (outfile_fd >= 0)
		close(outfile_fd);
	free(allocated);

	if (rc < 0)	return rc;
	
	/* treat underflow as error */
	return underflow ? -EIO : 0;
}

static int dma_readjob(char *devname, uint64_t addr,
			uint64_t size, uint64_t offset, uint64_t count,
			char *ofname)
{
	ssize_t rc = 0;
	size_t out_offset = 0;
	size_t bytes_done = 0;
	uint64_t i;
	char *buffer = NULL;
	char *allocated = NULL;
	struct timespec ts_start, ts_end;
	int out_fd = -1;
	int fpga_fd = open(devname, O_RDWR);
	long total_time = 0;
	float result;
	float avg_time = 0;
	int underflow = 0;

	if (fpga_fd < 0) 
	{
        fprintf(stderr, "unable to open device %s, %d.\n", devname, fpga_fd);
		perror("open device");
        return -EINVAL;
    }

	/* create file to write data to */
	if (ofname) 
	{
		out_fd = open(ofname, O_RDWR | O_CREAT | O_TRUNC | O_SYNC, 0666);
		
		if (out_fd < 0) 
		{
            fprintf(stderr, "unable to open output file %s, %d.\n", ofname, out_fd);
			perror("open output file");
            rc = -EINVAL;
            goto out;
        }
	}

	posix_memalign((void **)&allocated, 4096 /*alignment */ , size + 4096);
	if (!allocated) {
		fprintf(stderr, "OOM %lu.\n", size + 4096);
		rc = -ENOMEM;
		goto out;
	}

	buffer = allocated + offset;

	if (verbose) fprintf(stdout, "host buffer 0x%lx, %p.\n", size + 4096, buffer);

	for (i = 0; i < count; i++) 
	{
		rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);
		rc = read_to_buffer(devname, fpga_fd, buffer, size, addr);
		if (rc < 0)
			goto out;
		
		bytes_done = rc;

		if (!underflow && bytes_done < size)
			underflow = 1;
		
		clock_gettime(CLOCK_MONOTONIC, &ts_end);

		/* subtract the start time from the end time */
		timespec_sub(&ts_end, &ts_start);
		total_time += ts_end.tv_nsec;
		/* a bit less accurate but side-effects are accounted for */
		if (verbose) fprintf(stdout, "#%lu: CLOCK_MONOTONIC %ld.%09ld sec. read %ld/%ld bytes\n",
								  i, ts_end.tv_sec, ts_end.tv_nsec, bytes_done, size);

		/* file argument given? */
		if (out_fd >= 0) 
		{
			rc = write_from_buffer(ofname, out_fd, buffer, bytes_done, out_offset);

			if ((rc < 0) || ((size_t) rc < bytes_done))
				goto out;
			
			out_offset += bytes_done;
		}
	}

	if (!underflow) 
	{
		avg_time = (float)total_time/(float)count;
		result = ((float)size)*1000/avg_time;
		if (verbose) printf("** Avg time device %s, total time %ld nsec, avg_time = %f, size = %lu, BW = %f \n",
								devname, total_time, avg_time, size, result);
		
		printf("%s ** Average BW = %lu, %f\n", devname, size, result);
		rc = 0;
	} else 
		rc = -EIO;

out:
	close(fpga_fd);
	if (out_fd >= 0)
		close(out_fd);
	free(allocated);

	return rc;
}

void write_to_binary_data(char* infname)
{

	unsigned char buffer[128] = 
	{
		0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x11, 0x22, 0x33, 
		0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x44, 0x55, 0x66,
		0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x77, 0x88, 0x99,
		0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xAA, 0xBB, 0xCC,

		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12,
		0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x23,
		0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x34,

		0xBA, 0xD0, 0xC0, 0xDE, 0x00, 0x11, 0x22, 0x33, 
		0xBA, 0xD0, 0xC0, 0xDE, 0x00, 0x44, 0x55, 0x66,
		0xBA, 0xD0, 0xC0, 0xDE, 0x00, 0x77, 0x88, 0x99,
		0xBA, 0xD0, 0xC0, 0xDE, 0x00, 0xAA, 0xBB, 0xCC,
	};

	FILE* write_ptr = fopen(infname,"wb");  // w for write, b for binary

	fwrite(buffer,sizeof(buffer),1,write_ptr); // write 128 bytes from our buffer

	fclose(write_ptr);
}


int main()
{
	char *device = DEVICE_NAME_DEFAULT;
	uint64_t address = 0;
	uint64_t size = SIZE_DEFAULT;
	uint64_t offset = 0;
	uint64_t count = COUNT_DEFAULT;
	char *infname = DATA_PATH_DEFAULT;
	char *ofname = NULL;
	int rc = 0;

	write_to_binary_data(infname);


	fprintf(stdout, "dev %s, addr 0x%lx, size 0x%lx, offset 0x%lx, "
					"count %lu\n", device, address, size, offset, count);

	//rc = dma_writejob(device, address, size, offset, count, infname, ofname);

	rc = dma_readjob(device, address, size, offset, count, ofname);

	return rc;
}
