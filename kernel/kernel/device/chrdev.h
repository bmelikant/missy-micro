#ifndef _MISSY_KERNEL_CHRDEV_H
#define _MISSY_KERNEL_CHRDEV_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

// TODO: Remove this temporary definition when the virtual filesystem is in place
#define DEVICE_MAXPATH 255


typedef struct CHRDEV_OPERATIONS {

	ssize_t (*read_chrdev)(char *buf, size_t len);
	ssize_t (*write_chrdev)(char *buf, size_t len);
	void (*open_chrdev)(void);
	void (*close_chrdev)(void);

} chrdev_ops;

#ifdef _cplusplus
}
#endif

#endif
