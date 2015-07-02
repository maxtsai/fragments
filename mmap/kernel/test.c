#include <linux/module.h>	
#include <linux/init.h>
#include <linux/kernel.h>	
#include <linux/pci.h>
#include <linux/syscalls.h>
#include <linux/miscdevice.h>
#include <linux/vmalloc.h>  
#include <linux/slab.h> /*kmalloc*/
#include <linux/input.h>


static struct kobject *test_obj;
static char *test_buf = NULL;
static dma_addr_t dma_phys;

#define PAGENUM 1024

static int result_set(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	memset(test_buf, buf[0], PAGENUM*PAGE_SIZE);	
	return count;
}
static int result_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int i;
	for (i=0 ; i<32; i++)
		printk("%02x ", test_buf[i]);
	printk("\n");
	return 0;
}

static DEVICE_ATTR(result, S_IRUGO | S_IWUGO, result_show, result_set);

static struct attribute *sysfs_attributes[] = {
        &dev_attr_result.attr,
        NULL
};

static struct attribute_group sysfs_attribute_group = {
        .attrs = sysfs_attributes
};

static int test_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;  
	long length = vma->vm_end - vma->vm_start;  
	unsigned long start = vma->vm_start;  
	char *vmalloc_area_ptr = (char *)test_buf;  
	unsigned long pfn;  
	if (length > PAGENUM * PAGE_SIZE)  
		return -EIO;  

	pfn = dma_phys >> PAGE_SHIFT;

	if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,  
			       vma->vm_page_prot)) < 0) {  
		return ret;  
	}  
	return 0;
}

static const struct file_operations test_fops = {
	.owner                  = THIS_MODULE,
	.mmap                   = test_mmap,
	.llseek                 = noop_llseek,
};

static struct miscdevice test_dev = {
	MISC_DYNAMIC_MINOR,
	"test",
	&test_fops
};

int init_module(void)
{
	int ret = 0;

	//test_buf = (char *)vmalloc(PAGENUM * PAGE_SIZE);
	test_buf = (char *)pci_alloc_consistent(NULL, PAGENUM * PAGE_SIZE, &dma_phys);
	if (!test_buf) {
		printk("### [%s:%d] alloc mem fail.\n", __func__, __LINE__);
		return -ENOMEM;
	}

	//SetPageReserved(vmalloc_to_page((void *)(((unsigned long)test_buf))));  

	ret = misc_register(&test_dev);
	if (ret) {
		printk("can't register test_dev\n");
		return ret;
	}

	test_obj = kobject_create_and_add("test", NULL);
        ret = sysfs_create_group(test_obj, &sysfs_attribute_group);

	return 0;
}
 
void cleanup_module(void)
{
	if (!test_buf)
		return;

	sysfs_remove_group(test_obj, &sysfs_attribute_group);
	kobject_del(test_obj);

	misc_deregister(&test_dev);

	//ClearPageReserved(test_buf);
	//vfree(test_buf);
	pci_free_consistent(NULL, PAGENUM * PAGE_SIZE, (void*)test_buf, dma_phys);
	printk("[%s:%d] cleanup module\n", __func__, __LINE__);
}

MODULE_LICENSE("GPL");
