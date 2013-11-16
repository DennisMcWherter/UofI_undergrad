/**
 * scheduler.c
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

#include "mp2_given.h"

#define PROC_MP "mp2"
#define PROC_MP_STATUS "status"
#define MAX_BUFFER 1024
#define MAX_WAIT 5000
#define MAX_UTIL 693
#define THREAD_NAME "MP2_WORKER"

//Task states
typedef enum
{
    SLEEPING,
    RUNNING,
    READY
} mp2_task_state;

//Task structure
typedef struct
{
    struct task_struct* linux_task;
    struct timer_list wakeup_timer;
    int pid;
    int period;
    int computation;
    mp2_task_state state;
} mp2_task_struct;

//List structure
typedef struct
{
    struct list_head list;
    mp2_task_struct* task;
} task_list;

static mp2_task_struct* currentTask;
static struct task_struct* dispatcher_thread;
static struct proc_dir_entry* proc_mp;
static struct proc_dir_entry* proc_mp_status;
static char input[MAX_BUFFER];
static char output[MAX_BUFFER];
static spinlock_t list_lock;
static int utilization = 0;

LIST_HEAD(task_head);

// Forward decls
int registerProcess(const char* proc);
int yieldProcess(const char* proc);
int deregisterProcess(const char* proc);
mp2_task_struct* mp2_task_create(int pid, int period, int computation);
void mp2_task_destroy(mp2_task_struct* task);
void timer_callback(unsigned long data);
int dispatcher(void* data);
static int strToInt(const char** str, char delim);

/**
 * procfs write callback
 */
int write_procfs(struct file* file, const char* buffer, unsigned long count, void* data)
{
    int i = 0;
    int pid = 0;
    int num = 0;
    unsigned long length = (count >= MAX_BUFFER) ? MAX_BUFFER - 1 : count;
    
    if(copy_from_user(input, buffer, length)) {
        printk(KERN_ALERT "Error: copy_from_user()\n");
        return -EFAULT;
    }

    input[length] = '\0'; 
    // Determine action type
    // Should be of the form:
    // A, Message
    switch(input[0]) {
    case 'R':
        if(registerProcess(input+3))
            printk(KERN_INFO "Error during process registration!\n");
        break;
    case 'Y':
        if(yieldProcess(input+3))
            printk(KERN_INFO "Error during yield process!\n");
        break;
    case 'D':
        if(deregisterProcess(input+3))
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
            bufpos += sprintf(output + bufpos, "%d,%d,%d;", entry->task->pid, 
                    entry->task->period, entry->task->computation); 
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

    dispatcher_thread = kthread_run(dispatcher, NULL, THREAD_NAME);

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

    kthread_stop(dispatcher_thread);

    // Remove everything
    spin_lock(&list_lock);
    list_for_each_safe(pos, q, &task_head) {
        entry = list_entry(pos, task_list, list);
        list_del(pos);
        mp2_task_destroy(entry->task);
        vfree(entry->task);
        vfree(entry);
    }
    spin_unlock(&list_lock);

    list_del(&task_head);
    printk("Module exited\n");
}

/**
 * create task structure
 */
mp2_task_struct* mp2_task_create(int pid, int period, int computation)
{
    mp2_task_struct* task = vmalloc(sizeof(mp2_task_struct));
    
    task->pid = pid;
    task->period = period;
    task->computation = computation;
    task->linux_task = find_task_by_pid(pid); 
    task->state = SLEEPING;
    setup_timer(&task->wakeup_timer, timer_callback, (unsigned long)task);   

    return task;
}

//clear task contents
void mp2_task_destroy(mp2_task_struct* task)
{
    if(task == NULL)
        return;
    del_timer(&task->wakeup_timer);
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
    int period = 0;
    int computation = 0;
    int task_util = 0;
    int time = 0;
    mp2_task_struct* task = NULL;
    task_list * entry = vmalloc(sizeof(task_list)); 

    if(msg == NULL)
        return 0;

    pid = strToInt(&msg, ',');
    period = strToInt(&msg, ',');
    computation = strToInt(&msg, '\0');

    if(pid < 0 || period < 1 || computation < 1) {
        printk(KERN_ALERT "Problem registering pid: %d since there is some invalid value", pid);
        return -1;
    }

    task_util = (computation * 1000) / period;
    if(task_util + utilization > MAX_UTIL) {
        printk(KERN_ALERT "Could not register process - avoiding CPU overload");
        return -1;
    }
    utilization += task_util;

    task = mp2_task_create(pid, period, computation);
    entry->task = task;

    if(task == NULL)
        return 1;

    spin_lock(&list_lock);
    list_add(&entry->list, &task_head);
    time = task->period - task->computation;
    mod_timer(&task->wakeup_timer, jiffies + msecs_to_jiffies(time));
    spin_unlock(&list_lock);

    printk("Registering: %d\n", pid);
    return 0;
}

/**
 * Yield process
 */
int yieldProcess(const char* msg)
{
    int pid = 0;
    int time = 0;
    task_list* entry = NULL;
    
    if(msg == NULL)
        return 0;

    pid = strToInt(&msg, '\0');

    spin_lock(&list_lock);
    list_for_each_entry(entry, &task_head, list) {
        if(entry->task->pid == pid)
            break;
    }

    // If time <= 0, then it is during the next period
    time = entry->task->period - entry->task->computation;
    if(entry != NULL && time > 0) {
        entry->task->state = SLEEPING;
        mod_timer(&entry->task->wakeup_timer, jiffies + msecs_to_jiffies(time));
        set_task_state(entry->task->linux_task, TASK_UNINTERRUPTIBLE);
    }
    spin_unlock(&list_lock);
    //We wake up the dispatcher here;
    //Although this is not as per the spec,
    //We believe it to be a good idea.
    wake_up_process(dispatcher_thread);
    printk("))((- Yielding: %d\n", pid);
    return 0;
}

/**
 * Process de-registration
 */
int deregisterProcess(const char* msg)
{
    int pid = 0;
    int i = 0;
    struct list_head * pos = NULL;
    struct list_head * q = NULL;
    task_list * entry = NULL;

    if(msg == NULL)
        return 0;
    pid = strToInt(&msg, '\0');

    spin_lock(&list_lock);
    list_for_each_safe(pos, q, &task_head) {    
        entry = list_entry(pos, task_list, list); 
        if(entry->task->pid == pid) {
            utilization -= (entry->task->computation * 1000) / entry->task->period;
            list_del(pos);
            if(currentTask == entry->task)
                currentTask = NULL; // Nothing running - paranoia
            mp2_task_destroy(entry->task);
            vfree(entry->task);
            vfree(entry);
        }
    }
    spin_unlock(&list_lock);

    printk("De-registering: %d\n", pid);
    return 0;
}

/**
 * Dispatcher thread
 */
int dispatcher(void* data)
{
    task_list* entry = NULL;
    mp2_task_struct* highest = NULL;
    struct sched_param sparam;

    while(!kthread_should_stop()) {
        highest = NULL;
        entry = NULL;

        printk(KERN_INFO "----Dispatcher Executing----\n");
        spin_lock(&list_lock);
        list_for_each_entry(entry, &task_head, list) {
            if(entry->task->state == READY) {
                if(highest == NULL || entry->task->period <= highest->period)
                    highest = entry->task;
            }
        }

        //if currentTask is running (a timer expired, no yield)
        if(currentTask != NULL && currentTask->state == RUNNING) {
            printk(KERN_INFO "Preemp-loop");
            //if a task is READY & has higher priority
            if((highest != NULL) && (highest->period <= currentTask->period)) {
                //Preempt current task
                printk(KERN_INFO "--Preempting-- %d\n", currentTask->pid);
                currentTask->state = READY;
                sparam.sched_priority = 0;
                sched_setscheduler(currentTask->linux_task, SCHED_NORMAL, &sparam);
                printk(KERN_INFO "Setting to NORMAL: %d\n", currentTask->pid);
                //Schedule new task
                wake_up_process(highest->linux_task);
                sparam.sched_priority = MAX_USER_RT_PRIO - 1;
                sched_setscheduler(highest->linux_task, SCHED_FIFO, &sparam);
                highest->state = RUNNING;
                printk(KERN_INFO "-Setting to FIFO:  %d\n", highest->pid);
                currentTask = highest;
                //else if no READY task or no high enough priority task
            } else {
                printk(KERN_INFO "NO ACTION PREEMP");
                //do nothing
            }
        } else {
            printk(KERN_INFO "--Not Preempting--\n");
            //if task is SLEEPING
            if(currentTask != NULL && currentTask->state == SLEEPING) {
                sparam.sched_priority = 0;
                sched_setscheduler(currentTask->linux_task, SCHED_NORMAL, &sparam);
                printk(KERN_INFO "-| Setting to NORMAL: %d\n", currentTask->pid);
                currentTask = NULL;
            }
            //Schedule Highest
            if(highest != NULL && highest->state == READY) {
                wake_up_process(highest->linux_task);
                sparam.sched_priority = MAX_USER_RT_PRIO - 1;
                sched_setscheduler(highest->linux_task, SCHED_FIFO, &sparam);
                highest->state = RUNNING;
                printk(KERN_INFO "-| Setting to FIFO:  %d\n", highest->pid);
                currentTask = highest;
            }
        }

        spin_unlock(&list_lock);
        printk(KERN_INFO "---Finished Dispatch Run---\n");
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
    }

    printk(KERN_INFO "Dispatch Thread exiting...");

    return 0;
}

/**
 * Timer callback
 */
void timer_callback(unsigned long data)
{
    mp2_task_struct* task = (mp2_task_struct*)data;
    //Should never set running to ready; but hopefully this never happens
    task->state = READY;
    wake_up_process(dispatcher_thread);
    printk(KERN_INFO "(())-Waking up %d; set to READY\n", task->pid);
}

MODULE_LICENSE("GPL");

