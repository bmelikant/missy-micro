/*
 * device.c: Device driver interface for MISSY Microsystem
 *
 *  Created on: Oct 30, 2016
 *      Author: bmmelikant
 */

#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <errno.h>

#include "../include/chrdev.h"
#include "../include/device.h"

#define DEVICE_MAX 10

typedef struct CHARACTER_DEVICE {

	bool allocated;
	char device_name[DEVICE_MAXPATH];
	chrdev_ops *chrdev_fns;

} chrdev;

// character device inode list
chrdev char_devices[DEVICE_MAX];

// int device_list_init (): Initialize the device subsystem
int device_list_init () {

	for (size_t i = 0; i < DEVICE_MAX; i++)
		char_devices[i].allocated = false;

	return 0;
}

// int register_chrdev (): register a character device with the system
// inputs: major - major device number, name - name of device, dev_ops - operation functions
// returns: major number assigned on success, -1 on fail
int register_chrdev (int major, const char *name, chrdev_ops *device_ops) {

	// if the major # is zero, look for an unassigned node
	if (!major) {

		for ( ; major < DEVICE_MAX; major++) {

			if (char_devices[major].allocated == false) {

				char_devices[major].allocated = true;
				char_devices[major].chrdev_fns = device_ops;

				if (strlen (name) > DEVICE_MAXPATH)
					strncpy (char_devices[major].device_name, name, strlen(name));
				else
					strncpy (char_devices[major].device_name, name, DEVICE_MAXPATH);

				return major;
			}
		}

		// return an error code: too many open files
		errno = EMFILE;
		return -1;
	}

	// otherwise, attempt to allocate the given number
	else if (major > 0 && char_devices[major].allocated == false) {

		char_devices[major].allocated = true;
		char_devices[major].chrdev_fns = device_ops;

		if (strlen (name) < DEVICE_MAXPATH)
			strncpy (char_devices[major].device_name, name, strlen (name));
		else
			strncpy (char_devices[major].device_name, name, DEVICE_MAX-1);

		return major;
	}

	// otherwise return an EPERM error
	errno = ERANGE;
	return -1;
}

// ssize_t chrdev_read (): Read the requested character device
// inputs: major - device driver, buf - buffer to fill, length - size of buffer
// returns: actual number of bytes read (-1 on failure)
ssize_t chrdev_read (int major, char *buf, size_t length) {

	// call the device driver if it exists
	if (char_devices[major].allocated && char_devices[major].chrdev_fns->read_chrdev != NULL)
		return char_devices[major].chrdev_fns->read_chrdev(buf, length);

	// if the device is allocated but doesn't implement the function, send ENOSYS
	else if (char_devices[major].allocated) {

		errno = ENOSYS;
		return -1;
	}
	// if the device is unallocated, send ENODEV
	else {

		errno = ENODEV;
		return -1;
	}
}

// ssize_t chrdev_write (): Write to the requested character device
ssize_t chrdev_write (int major, char *buf, size_t length) {

	// call the device driver if it exists
	if (char_devices[major].allocated && char_devices[major].chrdev_fns->write_chrdev != NULL)
		return char_devices[major].chrdev_fns->write_chrdev(buf, length);

	// if the device is allocated but doesn't implement the function, send ENOSYS
	else if (char_devices[major].allocated) {

		errno = ENOSYS;
		return -1;
	}

	// if the device is unallocated, send ENODEV
	else {

		errno = ENODEV;
		return -1;
	}
}
