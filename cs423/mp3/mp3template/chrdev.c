/**
 * chrdev.c
 *
 * Character device driver to use mmap() from user-space
 *
 * Authors: Max Docauer (docauer2), Jiageng Li (jli65),
 *          Dennis J. McWherter, Jr. (dmcwhe2)
 */
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>

#include "chrdev.h"

// Forward declarations - internal methods
static int mmap_mem(struct file*, struct vm_area_struct*);
static int dev_open(struct inode*, struct file*);
static int dev_close(struct inode*, struct file*);

// Variable definitions
extern unsigned long* mp3_samples;

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .mmap    = mmap_mem,
    .open    = dev_open,
    .release = dev_close
};
static dev_t mmap_dev;
static struct cdev mmap_cdev;

/**
 * mmap_mem
 *
 * mmap() callback from device driver
 */
static int mmap_mem(struct file* ptr, struct vm_area_struct* vm)
{
    int ret = 0;
    unsigned long length = vm->vm_end - vm->vm_start;
    unsigned long start = vm->vm_start;
    unsigned long pfn = 0;
    char* smPtr = (char*)mp3_samples;
    struct page* p = NULL;
    // Below is for debugging purposes - it is unnecessary
    // in a real environment:
    //memset(mp3_samples, 2, PAGE_SIZE+2);
    while(length > 0) {
        pfn = vmalloc_to_pfn(smPtr);
        p = pfn_to_page(pfn);
        if(p == NULL)
            return 0;
        if((ret = remap_pfn_range(vm, start, pfn, PAGE_SIZE, PAGE_SHARED)) < 0)
            return ret;
        start  += PAGE_SIZE;
        smPtr  += PAGE_SIZE;
        length -= PAGE_SIZE;
    }
    printk("Virtual page: 0x%lx\n", pfn);
    return 0;
}

/**
 * dev_open
 *
 * Device driver open() callback
 */
static int dev_open(struct inode* inode, struct file* filep)
{
    printk("Open\n");
    return 0;
}

/**
 * dev_close
 *
 * Device driver close() callback
 */
static int dev_close(struct inode* inode, struct file* filep)
{
    printk("Close\n");
    return 0;
}

/**
 * Initialize the driver
 */
int init_driver(void)
{
    int i = 0;
    int size = NPAGES * PAGE_SIZE;
    struct page* p = NULL;

    memset(mp3_samples, -1, size);

    // Register the pages so they are safe to map
    for(i = 0 ; i < size ; i += PAGE_SIZE) {
        p = vmalloc_to_page(mp3_samples + i);
        if(p != NULL)
            SetPageReserved(p);
    }

    if(alloc_chrdev_region(&mmap_dev, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ERR "Error: Could not allocate char dev region\n");
        vfree(mp3_samples);
        return -ENODEV;
    }

    cdev_init(&mmap_cdev, &fops);
    if(cdev_add(&mmap_cdev, mmap_dev, 1) < 0) {
        printk(KERN_ERR "Error: Could not add char dev\n");
        vfree(mp3_samples);
        return -ENOMEM;
    }

    printk(KERN_INFO "Driver successfully initialized!\n");

    return 0;
}

/**
 * Close the driver
 */
void close_driver(void)
{
    int i = 0;
    struct page* p = NULL;

    cdev_del(&mmap_cdev);
    unregister_chrdev_region(mmap_dev, 1);

    // Unreserve pages so we can free
    for(i = 0 ; i < (NPAGES * PAGE_SIZE) ; i += PAGE_SIZE) {
        p = vmalloc_to_page(mp3_samples + i);
        if(p != NULL)
            ClearPageReserved(p);
    }

    printk(KERN_INFO "Driver closing...\n");
}


