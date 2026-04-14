#include <linux/sched.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/atomic.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/ipc_namespace.h>
#include <linux/task_work.h>

#include <linux/kprobes.h>
#include "deps.h"


extern kallsyms_lookup_name_t kallsyms_lookup_name_func;

typedef void (*zap_page_range_ptr_t)(struct vm_area_struct *, unsigned long, unsigned long);
static zap_page_range_ptr_t zap_page_range_ptr = NULL;
void zap_page_range(struct vm_area_struct *vma, unsigned long address, unsigned long size)
{
	if (!zap_page_range_ptr)
		zap_page_range_ptr = (zap_page_range_ptr_t) kallsyms_lookup_name_func("zap_page_range");
	zap_page_range_ptr(vma, address, size);
}

typedef int (*can_nice_ptr_t)(const struct task_struct *, const int);
static can_nice_ptr_t can_nice_ptr = NULL;
int can_nice(const struct task_struct *p, const int nice)
{
	if (!can_nice_ptr)
		can_nice_ptr = (can_nice_ptr_t) kallsyms_lookup_name_func("can_nice");
	return can_nice_ptr(p, nice);
}

typedef int (*security_binder_set_context_mgr_ptr_t)(struct task_struct *mgr);
security_binder_set_context_mgr_ptr_t security_binder_set_context_mgr_ptr = NULL;
int security_binder_set_context_mgr(struct task_struct *mgr)
{
	if (!security_binder_set_context_mgr_ptr)
		security_binder_set_context_mgr_ptr = (security_binder_set_context_mgr_ptr_t) kallsyms_lookup_name_func("security_binder_set_context_mgr");
	return security_binder_set_context_mgr_ptr(mgr);
}

typedef int (*security_binder_transaction_ptr_t)(struct task_struct *from, struct task_struct *to);
static security_binder_transaction_ptr_t security_binder_transaction_ptr = NULL;
int security_binder_transaction(struct task_struct *from, struct task_struct *to)
{
	if (!security_binder_transaction_ptr)
		security_binder_transaction_ptr = (security_binder_transaction_ptr_t) kallsyms_lookup_name_func("security_binder_transaction");
	return security_binder_transaction_ptr(from, to);
}

typedef int (*security_binder_transfer_binder_ptr_t)(struct task_struct *from, struct task_struct *to);
static security_binder_transfer_binder_ptr_t security_binder_transfer_binder_ptr = NULL;
int security_binder_transfer_binder(struct task_struct *from, struct task_struct *to)
{
	if (!security_binder_transfer_binder_ptr)
		security_binder_transfer_binder_ptr = (security_binder_transfer_binder_ptr_t) kallsyms_lookup_name_func("security_binder_transfer_binder");
	return security_binder_transfer_binder_ptr(from, to);
}

typedef int (*security_binder_transfer_file_ptr_t)(struct task_struct *from, struct task_struct *to, struct file *file);
static security_binder_transfer_file_ptr_t security_binder_transfer_file_ptr = NULL;
int security_binder_transfer_file(struct task_struct *from, struct task_struct *to, struct file *file)
{
	if (!security_binder_transfer_file_ptr)
		security_binder_transfer_file_ptr = (security_binder_transfer_file_ptr_t) kallsyms_lookup_name_func("security_binder_transfer_file");
	return security_binder_transfer_file_ptr(from, to, file);
}

typedef void (*put_ipc_ns_ptr_t)(struct ipc_namespace *ns);
static put_ipc_ns_ptr_t put_ipc_ns_ptr = NULL;
void put_ipc_ns(struct ipc_namespace *ns)
{
    if (!put_ipc_ns_ptr)
        put_ipc_ns_ptr = (put_ipc_ns_ptr_t) kallsyms_lookup_name_func("put_ipc_ns");
    put_ipc_ns_ptr(ns);
}

// struct ipc_namespace init_ipc_ns;
typedef struct ipc_namespace *init_ipc_ns_ptr_t;
static init_ipc_ns_ptr_t init_ipc_ns_ptr = NULL;
init_ipc_ns_ptr_t get_init_ipc_ns_ptr(void)
{
    if (!init_ipc_ns_ptr) init_ipc_ns_ptr = (init_ipc_ns_ptr_t) kallsyms_lookup_name_func("init_ipc_ns");
    return init_ipc_ns_ptr;
}

typedef int (*task_work_add_ptr_t)(struct task_struct *task, struct callback_head *twork, bool notify);
static task_work_add_ptr_t task_work_add_ptr = NULL;
int task_work_add(struct task_struct *task, struct callback_head *twork, enum task_work_notify_mode mode)
{
    if (!task_work_add_ptr)
        task_work_add_ptr = (task_work_add_ptr_t) kallsyms_lookup_name_func("task_work_add");
    return task_work_add_ptr(task, twork, mode);
}
//typedef 

typedef void (*mmput_async_ptr_t)(struct mm_struct *);
static mmput_async_ptr_t mmput_async_ptr = NULL;
void mmput_async(struct mm_struct *mm)
{
    if (!mmput_async_ptr)
       mmput_async_ptr = (mmput_async_ptr_t) kallsyms_lookup_name_func("mmput_async");
    mmput_async_ptr(mm);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 20, 1)
static inline void __clear_open_fd(unsigned int fd, struct fdtable *fdt)
{
	__clear_bit(fd, fdt->open_fds);
	__clear_bit(fd / BITS_PER_LONG, fdt->full_fds_bits);
}

static void __put_unused_fd(struct files_struct *files, unsigned int fd)
{
	struct fdtable *fdt = files_fdtable(files);
	__clear_open_fd(fd, fdt);
	if (fd < files->next_fd)
		files->next_fd = fd;
}

int __close_fd_get_file(unsigned int fd, struct file **res)
{
	struct files_struct *files = current->files;
	struct file *file;
	struct fdtable *fdt;

	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	if (fd >= fdt->max_fds)
		goto out_unlock;
	file = fdt->fd[fd];
	if (!file)
		goto out_unlock;
	rcu_assign_pointer(fdt->fd[fd], NULL);
	__put_unused_fd(files, fd);
	spin_unlock(&files->file_lock);
	get_file(file);
	*res = file;
	return filp_close(file, files);

out_unlock:
	spin_unlock(&files->file_lock);
	*res = NULL;
	return -ENOENT;
}
#else
typedef void (*__wake_up_pollfree_ptr_t)(struct wait_queue_head *wq_head);
static __wake_up_pollfree_ptr_t __wake_up_pollfree_ptr = NULL;
void __wake_up_pollfree(struct wait_queue_head *wq_head){
    if (!__wake_up_pollfree_ptr)
        __wake_up_pollfree_ptr = (__wake_up_pollfree_ptr_t) kallsyms_lookup_name_func("__wake_up_pollfree");
    return __wake_up_pollfree_ptr(wq_head);
}

typedef struct file * (*close_fd_get_file_ptr_t)(unsigned int fd);
static close_fd_get_file_ptr_t close_fd_get_file_ptr = NULL;
struct file * close_fd_get_file(unsigned int fd)
{
    if (!close_fd_get_file_ptr)
        close_fd_get_file_ptr = (close_fd_get_file_ptr_t) kallsyms_lookup_name_func("close_fd_get_file");

    return close_fd_get_file_ptr(fd);

}

typedef int (*register_shrinker_ptr_t)(struct shrinker *shrinker, const char *fmt, ...);
static register_shrinker_ptr_t register_shrinker_ptr = NULL;

int register_shrinker(struct shrinker *shrinker, const char *fmt, ...)
{
    va_list args;

    if (!register_shrinker_ptr)
        register_shrinker_ptr = (register_shrinker_ptr_t) kallsyms_lookup_name_func("register_shrinker");

    va_start(args, fmt);
    int ret = register_shrinker_ptr(shrinker, fmt, args);
    va_end(args);
    return ret;
}

typedef void (*unregister_shrinker_ptr_t)(struct shrinker *shrinker);
static unregister_shrinker_ptr_t unregister_shrinker_ptr = NULL;

void unregister_shrinker(struct shrinker *shrinker)
{
    if (!unregister_shrinker_ptr)
        unregister_shrinker_ptr = (unregister_shrinker_ptr_t) kallsyms_lookup_name_func("unregister_shrinker");

    unregister_shrinker_ptr(shrinker);
}

typedef void (*zap_page_range_single_ptr_t)(struct vm_area_struct *vma,
                                           unsigned long address,
                                           unsigned long size,
                                           struct zap_details *details);

static zap_page_range_single_ptr_t zap_page_range_single_ptr = NULL;

// zap_page_range_single 函数实现
void zap_page_range_single(struct vm_area_struct *vma,
                                  unsigned long address,
                                  unsigned long size,
                                  struct zap_details *details)
{
    if (!zap_page_range_single_ptr)
        zap_page_range_single_ptr = (zap_page_range_single_ptr_t)kallsyms_lookup_name_func("zap_page_range_single");

    if (zap_page_range_single_ptr)
        zap_page_range_single_ptr(vma, address, size, details);
}

#endif // LINUX_VERSION_CODE < KERNEL_VERSION(4, 20, 1)

