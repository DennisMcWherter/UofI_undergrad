/**
 * chrdev.h
 *
 * Character device driver interface
 *
 * Authors: Max Docauer (docauer2), Jiageng Li (jli65),
 *          Dennis J. McWherter, Jr. (dmcwhe2)
 */

#ifndef CHRDEV_H__
#define CHRDEV_H__

#define DEVICE_NAME "mp3_dev"
#define NPAGES (128)

// Driver methods
int init_driver(void);
void close_driver(void);

#endif /** CHRDEV_H__ */

