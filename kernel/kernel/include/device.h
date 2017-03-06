#ifndef _MISSY_KERNEL_DEVICE_H
#define _MISSY_KERNEL_DEVICE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

#include <include/chrdev.h>

typedef struct CHARACTER_DEVICE {

	bool allocated;
	unsigned long device_id;
	char device_name[DEVICE_MAXPATH];
	chrdev_ops *chrdev_fns;

} chrdev;

//
// device interface functions
void device_list_init 		();
void device_list_shutdown	();

// character device interaction
int register_chrdev (int major, const char *name, chrdev_ops *device_ops);

// used for system calls / kernel access to devices
void chrdev_open 	();
void chrdev_close 	();

ssize_t chrdev_read 	(int major, char *buf, size_t len);
ssize_t chrdev_write 	(int major, char *buf, size_t len);

#ifdef _cplusplus
}
#endif

#endif
