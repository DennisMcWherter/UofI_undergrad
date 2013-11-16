/**
 * timer_module.c
 *
 * Authors: Max Docauer (docauer2), Jiageng Li (jli65),
 *          Dennis J. McWherter, Jr. (dmcwhe2)
 */

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/timer.h>

#include <asm/spinlock.h>
#include <asm/uaccess.h>

#include "mp1_given.h"

#define PROC_MP1 "mp1"
#define PROC_MP1_STATUS "status"
#define MAX_BUFFER 1024
#define MAX_WAIT 5000
#define THREAD_NAME "MP1_WORKER"

typedef struct
{
    int pid;
    unsigned long cpu_time;
    struct list_head list;
} pid_list_t;

static struct task_struct* thread;
static struct timer_list timer;
static struct proc_dir_entry* proc_mp1;
static struct proc_dir_entry* proc_mp1_status;
static char input[MAX_BUFFER];
static spinlock_t list_lock;

// Debug information
static unsigned allocs = 0;
static unsigned frees = 0;

LIST_HEAD(pid_list_head);

/**
 * worker thread
 */
int thread_routine(void* data)
{
    pid_list_t* entry;
    printk(KERN_INFO "Thread initialized.\n");

    while(!kthread_should_stop()) {
        spin_lock(&list_lock);
        list_for_each_entry(entry, &pid_list_head, list) {
            if(get_cpu_use(entry->pid, &entry->cpu_time) == -1)
                printk(KERN_ALERT "Error with get_cpu_use() (is pid %d dead?)\n", entry->pid);
        }
        spin_unlock(&list_lock);
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
        printk(KERN_INFO "Thread processing...\n");
    }

    printk(KERN_INFO "Exiting thread...\n");

    return 0;
}

/**
 * procfs timer callback
 */
void timer_callback(unsigned long data)
{
    wake_up_process(thread);
    // Restart time
    mod_timer(&timer, jiffies + msecs_to_jiffies(MAX_WAIT));
}

/**
 * procfs write callback
 */
int write_procfs(struct file* file, const char* buffer, unsigned long count, void* data)
{
    int i = 0;
    int pid = 0;
    int num = 0;
    unsigned long length = (count > MAX_BUFFER) ? MAX_BUFFER : count;
    pid_list_t* entry = NULL;

    printk(KERN_INFO "Writing...\n");
    
    if(copy_from_user(input, buffer, length)) {
        printk(KERN_ALERT "Error: copy_from_user()\n");
        return -EFAULT;
    }

    for( i = 0 ; i < length ; ++i ) {
        num = buffer[i] - '0';
        pid *= 10;
        pid += num;
    }

    entry = vmalloc(sizeof(pid_list_t));
    entry->pid = pid;
    entry->cpu_time = 0;

    spin_lock(&list_lock);
    list_add(&entry->list, &pid_list_head);
    spin_unlock(&list_lock);

    allocs++; // Debug

    return length;
}

/**
 * procfs read callback
 */
int read_procfs(char* buffer, char** buffer_loc, off_t offset, int buffer_len, int* eof, void* data)
{
    struct list_head* pos = NULL;
    pid_list_t* entry = NULL;
    int ret = 0;

    printk(KERN_INFO "Reading...\n");

    spin_lock(&list_lock);
    list_for_each(pos, &pid_list_head) {
        entry = list_entry(pos, pid_list_t, list);
        ret += sprintf(buffer+ret, "(%d): %lu\n", entry->pid, entry->cpu_time);
    }
    spin_unlock(&list_lock);
    
    ret = (offset > 0) ? 0 : ret;
    printk(KERN_INFO "Ret is %d", ret);

    return (offset > 0) ? 0 : ret;
}

/**
 * module initialization
 */
int init_module(void)
{
    printk(KERN_INFO "Initializing...\n");

    spin_lock_init(&list_lock);
    setup_timer(&timer, timer_callback, 0);
    proc_mp1 = proc_mkdir(PROC_MP1, NULL);

    if(proc_mp1 == NULL) {
        printk(KERN_ALERT "Could not create /proc/%s", PROC_MP1);
        return -EFAULT;
    }

    proc_mp1_status = create_proc_entry(PROC_MP1_STATUS, 0666, proc_mp1);

    if(proc_mp1_status == NULL) {
        printk(KERN_ALERT "Could not create /proc/%s/%s\n", PROC_MP1, PROC_MP1_STATUS);
        return -EFAULT;
    }

    proc_mp1_status->read_proc  = read_procfs;
    proc_mp1_status->write_proc = write_procfs;

    thread = kthread_run(thread_routine, NULL, THREAD_NAME);

    if(mod_timer(&timer, jiffies + msecs_to_jiffies(MAX_WAIT)))
        printk(KERN_ALERT "Problem with mod_timer() (Continung...)\n");

    return 0;
}

/**
 * module clean up
 */
void cleanup_module(void)
{
    struct list_head* pos, *q = NULL;
    pid_list_t* entry = NULL;

    del_timer(&timer);
    kthread_stop(thread);

    remove_proc_entry(PROC_MP1_STATUS, proc_mp1);
    remove_proc_entry(PROC_MP1, NULL);

    list_for_each_safe(pos, q, &pid_list_head) {
        entry = list_entry(pos, pid_list_t, list);
        list_del(pos);
        vfree(entry);
        frees++; // Debug
    }

    printk(KERN_INFO "Allocs: %d\nFrees: %d\nExiting...\n", allocs, frees);
}

MODULE_LICENSE("GPL");

