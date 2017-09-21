/*
 * vfsys.h
 *
 *  Created on: Aug 21, 2017
 *      Author: bmelikant
 */

#ifndef MISSY_KERNEL_VFSYS_H_
#define MISSY_KERNEL_VFSYS_H_

#ifndef _cplusplus
#include <stdbool.h>
#endif

#include <stdint.h>
#include <stddef.h>

#define TYPE_FILE		0x0000
#define TYPE_DIRECTORY	0x0001
#define TYPE_BLOCK		0x0002
#define TYPE_CHARDEV	0x0004

// structure forward definitions
struct BLOCKDEV_OPS;
struct CHARDEV_OPS;
struct BLOCK_DEVICE;
struct CHAR_DEVICE;
struct VFSYS_INODE;

// operations on block devices
typedef struct BLOCKDEV_OPS {

} blkdev_ops;

// block device inode
typedef struct BLOCK_DEVICE {

} block_device;

// operations to be performed on character devices
typedef struct CHARDEV_OPS {

	int		(*chrdev_init)();
	void    (*open_chrdev)();
	void    (*close_chrdev)();
	ssize_t (*read_chrdev)(unsigned char *buf, size_t len);
	ssize_t (*write_chrdev)(unsigned char *buf, size_t len);

} chrdev_ops;

// character device inode
typedef struct CHAR_DEVICE {

	struct VFSYS_INODE inode;
	unsigned long device_id;
	struct CHARDEV_OPS ops;

} char_device;

// filesystem inode
typedef struct VFSYS_INODE {

	uint32_t inode_id;
	char *name;
	uint8_t filetype;
	uint16_t permissions;
	uint32_t uid;
	uint32_t gid;
	uint64_t filesize;

} vfsys_inode;

#endif /* MISSY_KERNEL_VFSYS_H_ */
