/**
 * profiler.c
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
#include <linux/jiffies.h>

#include <asm/spinlock.h>
#include <asm/uaccess.h>

#include "chrdev.h"
#include "mp3_given.h"

#define PROC_MP "mp3"
#define PROC_MP_STATUS "status"
#define MAX_SAMPLES (NPAGES * PAGE_SIZE)
#define MAX_BUFFER 1024
#define MAX_WAIT 5000
#define MAX_UTIL 693
#define THREAD_NAME "MP3_WORKER"

#define DELAY(x) msecs_to_jiffies(x)

typedef struct 
{
    struct task_struct* task;
    int pid;
    int maj_flt;
    int min_flt;
    int util;
} tc_block;

//List structure
typedef struct
{
    struct list_head list;
    tc_block * tcb;
} task_list;

//work struct
typedef struct 
{
    struct delayed_work work;
} work_item;

typedef struct 
{
    unsigned long jiffies;
    unsigned long maj_flt;
    unsigned long min_flt;
    unsigned long util;
} sample_t;

static struct workqueue_struct * work_queue = NULL;
static struct delayed_work * mp3_work = NULL;

static struct proc_dir_entry* proc_mp;
static struct proc_dir_entry* proc_mp_status;

static char input[MAX_BUFFER];
static char output[MAX_BUFFER];

unsigned long * mp3_samples;
static unsigned long num_samples = 0;

static spinlock_t list_lock;

LIST_HEAD(task_head);

// Forward decls
int registerProcess(const char* proc);
int deregisterProcess(const char* proc);
int dispatcher(void* data);
tc_block * create_tcb(int pid);
static int strToInt(const char** str, char delim);
static void wq_handler(struct delayed_work * dwork);


/**
 * procfs write callback
 */
int write_procfs(struct file* file, const char* buffer, unsigned long count, void* data)
{
    unsigned long length = (count >= MAX_BUFFER) ? MAX_BUFFER - 1 : count;
    
    if(copy_from_user(input, buffer, length)) {
        printk(KERN_ALERT "Error: copy_from_user()\n");
        return -EFAULT;
    }

    input[length] = '\n'; 
    // Determine action type
    // Should be of the form:
    // A Message
    switch(input[0]) {
    case 'R':
        if(registerProcess(input+2))
            printk(KERN_INFO "Error during process registration!\n");
        break;
    case 'U':
        if(deregisterProcess(input+2))
            printk(KERN_INFO "Error during de-registration process!\n");
        break;
    default:
        break;
        // Do nothing
    }

    return length;
}

/**
 * procfs read callback
 */
int read_procfs(char * buffer, char ** buffer_location, off_t offset, int buffer_length, int * eof, void * data)  
{
    struct list_head* pos = NULL;
    task_list * entry = NULL;
    int ret = 0;
    int bufpos = 0;
    int lastsemi = 0;

    //No more to read
    if(offset > 0) {
        *eof = 1;
        return 0;
    }

    spin_lock(&list_lock);
    list_for_each(pos, &task_head) {
        if((bufpos + 1) < buffer_length) {
            lastsemi = bufpos;
            entry = list_entry(pos, task_list, list); 
            bufpos += sprintf(output + bufpos, "%d\n", entry->tcb->pid); 
        } else {
            break;
        }
    }
    spin_unlock(&list_lock);

    if(bufpos == 0) {
        return 0;
    }

    if((bufpos + 1) > buffer_length) {
        output[lastsemi] = '\0';
        ret = lastsemi + 1;
    } else {
        output[bufpos] = '\0';
        ret = bufpos + 1;
    }
    memcpy(buffer, output, ret);
    return (offset > 0) ? 0 : ret;
}

/**
 * module initialization
 */
int init_module(void)
{
    printk(KERN_INFO "---Initializing...\n");

    spin_lock_init(&list_lock);
    proc_mp = proc_mkdir(PROC_MP, NULL);

    if(proc_mp == NULL) {
        printk(KERN_ALERT "Could not create /proc/%s", PROC_MP);
        return -EFAULT;
    }

    proc_mp_status = create_proc_entry(PROC_MP_STATUS, 0666, proc_mp);

    if(proc_mp_status == NULL) {
        printk(KERN_ALERT "Could not create /proc/%s/%s\n", PROC_MP, PROC_MP_STATUS);
        return -EFAULT;
    }

    proc_mp_status->read_proc  = read_procfs;
    proc_mp_status->write_proc = write_procfs;

    mp3_samples = vmalloc(NPAGES * PAGE_SIZE);

    // Initialize driver
    init_driver();

    return 0;
}

/**
 * module clean up
 */
void cleanup_module(void)
{
    struct list_head* pos = NULL, *q = NULL;
    task_list* entry = NULL;

    remove_proc_entry(PROC_MP_STATUS, proc_mp);
    remove_proc_entry(PROC_MP, NULL);

    // Cleanup driver BEFORE freeing memory
    close_driver();

    if(work_queue != NULL) {
        cancel_delayed_work_sync(mp3_work);
        destroy_workqueue(work_queue);
    }

    if(mp3_work != NULL)
        vfree(mp3_work);

    // Remove everything
    spin_lock(&list_lock);
    list_for_each_safe(pos, q, &task_head) {
        entry = list_entry(pos, task_list, list);
        list_del(pos);

        vfree(entry->tcb);
        vfree(entry);
    }
    spin_unlock(&list_lock);

    list_del(&task_head);
    vfree(mp3_samples);
    printk("Module exited\n");
}

//Convert string to int based on delim
static int strToInt(const char** str, char delim)
{
    int ret = 0;

    if(str == NULL)
        return -1;

    // Skip whitespace
    while(**str == ' ')
        (*str)++;
    
    for(; **str != delim ; (*str)++) {
        ret *= 10;
        ret += **str - '0';
    }
    (*str)++; // Skip found delimiter

    return ret;
}

/**
 * Register a process
 */
int registerProcess(const char* msg)
{
    int pid = 0;
    int schedule = 0;
    task_list* entry = NULL;

    if(msg == NULL) 
        return 0;

    entry = vmalloc(sizeof(task_list));
    pid = strToInt(&msg, '\n');
    entry->tcb = create_tcb(pid);

    if(entry->tcb == NULL) 
        return 1;
    spin_lock(&list_lock);
    if(list_empty(&task_head)) // make work queue and schedule
        schedule = 1;
    list_add(&entry->list, &task_head);
    spin_unlock(&list_lock);

    if(schedule) {
        work_queue = create_workqueue("mp3_queue");
        mp3_work = vmalloc(sizeof(struct delayed_work));
        INIT_DELAYED_WORK(mp3_work, wq_handler);
        queue_delayed_work(work_queue, mp3_work, DELAY(50));
    }
    
    return 0;
}

/*
 * Create a tc_block.
 */
tc_block * create_tcb(int pid) {
    tc_block * tcb = vmalloc(sizeof(tc_block));
    tcb->pid = pid;
    tcb->maj_flt = 0;
    tcb->min_flt = 0;
    tcb->util = 0;
    tcb->task = find_task_by_pid(pid);
    return tcb;
}

/**
 * Process de-registration
 */
int deregisterProcess(const char* msg)
{
    int pid = 0;
    int empty = 0;
    struct list_head * pos = NULL;
    struct list_head * q = NULL;
    task_list * entry = NULL;

    if(msg == NULL)
        return 0;
    pid = strToInt(&msg, '\n');

    spin_lock(&list_lock);
    list_for_each_safe(pos, q, &task_head) {    
        entry = list_entry(pos, task_list, list); 
        if(entry->tcb->pid == pid) {
            list_del(pos);
            vfree(entry->tcb);
            vfree(entry);
        }
    }
    empty = list_empty(&task_head);
    spin_unlock(&list_lock);

    if(empty) {
        cancel_delayed_work_sync(mp3_work);
        destroy_workqueue(work_queue);
        vfree(mp3_work);
        mp3_work = NULL;
        work_queue = NULL;
        num_samples = 0;
    }

    printk("De-registering: %d\n", pid);
    return 0;
}

/*
 * Function to be scheduled on work queue;
 * Goes through each registered proess and writes their information
 * to the sample buffer.
 */
static void wq_handler(struct delayed_work * dwork) {
    struct list_head* pos = NULL;
    task_list * entry = NULL;
    sample_t * sample_block = (sample_t *)(mp3_samples);
    sample_block = &sample_block[num_samples++];
    //if we've taken enough samples, don't schedule more work
    if(num_samples > 12000) {
        return;
    }
    spin_lock(&list_lock);
    list_for_each(pos, &task_head) {
        entry = list_entry(pos, task_list, list); 

        sample_block->jiffies = get_jiffies_64();
        sample_block->maj_flt = entry->tcb->task->maj_flt - entry->tcb->maj_flt;
        sample_block->min_flt = entry->tcb->task->min_flt - entry->tcb->min_flt;
        sample_block->util = (entry->tcb->task->utime) - entry->tcb->util;

        entry->tcb->maj_flt = entry->tcb->task->maj_flt;
        entry->tcb->min_flt = entry->tcb->task->min_flt;
        entry->tcb->util = entry->tcb->task->utime; 
    }
    spin_unlock(&list_lock);

    queue_delayed_work(work_queue, mp3_work, DELAY(50));
}

MODULE_LICENSE("GPL");

