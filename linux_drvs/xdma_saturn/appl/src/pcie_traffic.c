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

#define DATA_FILE_PATH "data/datafile_4k.bin"
#define DEVICE_NAME_DEFAULT "/dev/xdma0_h2c_0"
#define SIZE_DEFAULT (32)
#define COUNT_DEFAULT (1)

void print_transmission_info(int underflow, long total_time, uint64_t count,
			     uint64_t size, char *devname)
{
	if (!underflow) {
		float avg_time = (float)total_time / (float)count;
		float result = ((float)size) * 1000 / avg_time;
		if (verbose)
			printf("** Avg time device %s, total time %ld nsec, avg_time = %f, size = %lu, BW = %f \n",
			       devname, total_time, avg_time, size, result);
		printf("%s ** Average BW = %lu, %f\n", devname, size, result);
	}
}

// size_t initialize_memBuffer(uint64_t size, uint64_t offset, ssize_t rc,
// 			    int infile_fd, char *infname, char *buffer,
// 			    char *allocated)
// {
// 	posix_memalign((void **)&allocated, 4096 /*alignment */, size + 4096);
// 	if (!allocated) {
// 		fprintf(stderr, "OOM %lu.\n", size + 4096);
// 		rc = -ENOMEM;
// 		return -1;
// 	}

// 	buffer = allocated + offset;
// 	if (verbose)
// 		fprintf(stdout, "host buffer 0x%lx = %p\n", size + 4096,
// 			buffer);

// 	if (infile_fd >= 0) {
// 		rc = read_to_buffer(infname, infile_fd, buffer, size, 0);
// 		if ((rc < 0) || ((uint32_t) rc < size))
// 			return -2;
// 	}

// 	return 0;
// }

size_t verify_prameters(char *devname, int fpga_fd, char *infname,
			int infile_fd, char *ofname, int outfile_fd)
{
	if (fpga_fd < 0) {
		fprintf(stderr, "unable to open device %s, %d.\n", devname,
			fpga_fd);
		perror("open device");
		return -1;
	}

	if (infname) {
		infile_fd = open(infname, O_RDONLY);

		if (infile_fd < 0) {
			fprintf(stderr, "unable to open input file %s, %d.\n",
				infname, infile_fd);
			perror("open input file");
			return -2;
		}
	}

	if (ofname) {
		outfile_fd =
			open(ofname, O_RDWR | O_CREAT | O_TRUNC | O_SYNC, 0666);

		if (outfile_fd < 0) {
			fprintf(stderr, "unable to open output file %s, %d.\n",
				ofname, outfile_fd);
			perror("open output file");
			return -3;
		}
	}

	return 0;
}

int main()
{
	/*Pervious transfer parameter*/
	char *devname = DEVICE_NAME_DEFAULT;
	uint64_t addr = 0;
	uint64_t size = SIZE_DEFAULT;
	uint64_t offset = 0;
	uint64_t count = COUNT_DEFAULT;
	char *infname = DATA_FILE_PATH;
	char *ofname = NULL;

	/* Original parameter */
	uint64_t i;
	ssize_t rc = 0;
	size_t bytes_done = 0;
	size_t out_offset = 0;

	char *buffer = NULL;
	char *allocated = NULL;
	struct timespec ts_start, ts_end;
	int infile_fd = -1;
	int outfile_fd = -1;
	int fpga_fd = open(devname, O_RDWR);
	long total_time = 0;
	int underflow = 0;

	if (verify_prameters(devname, fpga_fd, infname, infile_fd, ofname,
			     outfile_fd) != 0)
		return -1;

	// if (initialize_memBuffer(size, offset, rc, infile_fd, infname, buffer,
	// 			 allocated) != 0)
	// 	return -2;

	posix_memalign((void **)&allocated, 4096 /*alignment */, size + 4096);
	if (!allocated) {
		fprintf(stderr, "OOM %lu.\n", size + 4096);
		rc = -ENOMEM;
		return -1;
	}

	buffer = allocated + offset;
	if (verbose)
		fprintf(stdout, "host buffer 0x%lx = %p\n", size + 4096,
			buffer);

	if (infile_fd >= 0) {
		rc = read_to_buffer(infname, infile_fd, buffer, size, 0);
		if ((rc < 0) || ((uint32_t) rc < size))
			return -2;
	}

	for (i = 0; i < count; i++) {
		/* write buffer to AXI MM address using SGDMA */
		rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);

		rc = write_from_buffer(devname, fpga_fd, buffer, size, addr);
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
			fprintf(stdout,
				"#%lu: CLOCK_MONOTONIC %ld.%09ld sec. write %ld bytes\n",
				i, ts_end.tv_sec, ts_end.tv_nsec, size);

		if (outfile_fd >= 0) {
			rc = write_from_buffer(ofname, outfile_fd, buffer,
					       bytes_done, out_offset);

			if (rc < 0 || ((size_t) rc < bytes_done))
				goto out;
			out_offset += bytes_done;
		}
	}

	print_transmission_info(underflow, total_time, count, size, devname);

out:
	close(fpga_fd);
	if (infile_fd >= 0)
		close(infile_fd);
	if (outfile_fd >= 0)
		close(outfile_fd);
	free(allocated);

	if (rc < 0)
		return rc;
	/* treat underflow as error */
	return underflow ? -EIO : 0;
}
