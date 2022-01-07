#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");

static int minor = 0; /* 次设备号 */
static dev_t test_dev;
static struct cdev test_cdev;
static struct class *test_class = NULL;
static struct device *test_device = NULL;

static int test_open(struct inode *inode, struct file *filp)
{
    /* TODO */
    printk("==>>test_open\n");
    return 0;
}

static int test_release(struct inode *inode, struct file *filp)
{
    /* TODO */
    return 0;
}

static ssize_t test_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    /* TODO */
    return 0;
}

static ssize_t test_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    /* TODO */
    return 0;
}

int test_vm_fault(struct vm_fault *vmf)
{
    printk("test_vm_fault\n");



    return VM_FAULT_SIGBUS;
}

void test_vm_open(struct vm_area_struct *vma)
{
    printk("test_vm_open\n");
}
void test_vm_close(struct vm_area_struct *vma)
{
    printk("test_vm_close\n");
}

int test_vm_access(struct vm_area_struct *vma, unsigned long addr,
		     void *buf, int len, int write)
{
    printk("test_vm_access\n");
    return 0;
}
static const struct vm_operations_struct test_vm_ops = {
	.fault = test_vm_fault,
	.open = test_vm_open,
	.close = test_vm_close,
	.access = test_vm_access
};

int test_mmap(struct file *filp, struct vm_area_struct *vma)
{
    printk("vma->vm_start:0x%lx vma->vm_end:0x%lx\n", vma->vm_start, vma->vm_end);
    printk("vma->vm_ops:0x%lx\n", (unsigned long )vma->vm_ops);
    printk("vma->vm_mm:0x%lx\n", (unsigned long )vma->vm_mm);
    printk("page_offset:0x%lx", vma->vm_pgoff);
    if (vma->vm_mm) {
        printk("vma->vm_mm->mmap_base:0x%lx\n", (unsigned long )vma->vm_mm->mmap_base);
    }

    vma->vm_ops = &test_vm_ops;
    return 0;
}

static struct file_operations test_fops = {
    .owner = THIS_MODULE,
    .open = test_open,
    .release = test_release,
    .read = test_read,
    .write = test_write,
    .mmap = test_mmap
};

/* 创建设备号 */
static int test_register_chrdev(void)
{
    int result;

    result = alloc_chrdev_region(&test_dev, minor, 1, "test_dev");
    if (result < 0) {
        pr_err("alloc_chrdev_region failed! result: %d\n", result);
        return result;
    }

    return 0;
}

/* 注册驱动 */
static int test_cdev_add(void)
{
    int result;

    cdev_init(&test_cdev, &test_fops);
    test_cdev.owner = THIS_MODULE;

    result = cdev_add(&test_cdev, test_dev, 1);
    if (result < 0) {
        pr_err("alloc_chrdev_region failed! result: %d\n", result);
        unregister_chrdev_region(test_dev, 1);
        return result;
    }

    return 0;
}

/* 创建设备节点 */
static int test_device_create(void)
{
    test_class = class_create(THIS_MODULE, "test_dev_class");
    if (IS_ERR(test_class)) {
        pr_err("class_create failed!\n");
        goto class_create_fail;
    }

    test_device = device_create(test_class, NULL, test_dev, NULL, "test_dev");
    if (IS_ERR(test_device)) {
        pr_err("device_create failed!\n");
        goto device_create_fail;
    }

    return 0;

device_create_fail:
    class_destroy(test_class);
class_create_fail:
    cdev_del(&test_cdev);
    unregister_chrdev_region(test_dev, 1);
    return -1;
}

static __init int test_init(void)
{
    int result;

    result = test_register_chrdev();
    if (result < 0) {
        return result;
    }

    result = test_cdev_add();
    if (result < 0) {
        return result;
    }

    result = test_device_create();
    if (result < 0) {
        return result;
    }

    return 0;
}

static __exit void test_exit(void)
{
    device_destroy(test_class, test_dev);
    class_destroy(test_class);
    cdev_del(&test_cdev);
    unregister_chrdev_region(test_dev, 1);
}

module_init(test_init);
module_exit(test_exit);
