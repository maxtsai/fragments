#include <linux/module.h>	
#include <linux/init.h>
#include <linux/kernel.h>	
#include <linux/syscalls.h>
#include <linux/miscdevice.h>
//#include <asm/memory.h>
#include <linux/vmalloc.h>  
#include <linux/slab.h> /*kmalloc*/

#include <linux/input.h> /*for sysfs register*/


static struct kobject *test_obj;
static char *test_buf = NULL;
//static char test_buf[1024];

static int result_set(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	printk("### [%s:%d] %s\n", __func__, __LINE__, buf);
	return count;
}
static int result_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int i;
	for (i=0 ; i<64; i++)
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

void simple_vma_open(struct vm_area_struct *vma)
{
	printk("### [%s:%d]\n", __func__, __LINE__);
}
void simple_vma_close(struct vm_area_struct *vma)
{
	int i;
	printk("\n### [%s:%d]\n", __func__, __LINE__);
	for (i=0 ; i<32; i++)
		printk("%02x ", test_buf[i]);
	printk("\n");
}
static struct vm_operations_struct simple_remap_vm_ops = {
	 .open = simple_vma_open,
	 .close = simple_vma_close,
};

static int test_mmap(struct file *filp, struct vm_area_struct *vma)
{
#if 0
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	//unsigned long phyaddr = ((unsigned long) test_buf) - PAGE_OFFSET;
	unsigned long phyaddr = ((unsigned long) test_buf) + offset - 0x10000000;
	unsigned long mypfn = phyaddr >> PAGE_SHIFT;
	//unsigned long mypfn = virt_to_page(test_buf);
	unsigned long vmsize = vma->vm_end - vma->vm_start;

	if (vmsize > (8192-offset)) {
		printk("%ld > %ld\n", vmsize, 8192-offset);
		return -ENXIO;
	}

	//vma->vm_flags |= VM_IO | VM_SHARED;
	//vma->vm_flags |= VM_LOCKED;

	//vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	if (remap_pfn_range(vma, vma->vm_start, mypfn,
				vmsize, vma->vm_page_prot)) {
		printk("mmap remap_pfn_range failed\n");
		return -ENOBUFS;
	}
	vma->vm_ops = &simple_remap_vm_ops;
	simple_vma_open(vma);
#else
            int ret;  
            long length = vma->vm_end - vma->vm_start;  
            unsigned long start = vma->vm_start;  
            char *vmalloc_area_ptr = (char *)test_buf;  
            unsigned long pfn;  
            if (length > 1 * PAGE_SIZE)  
                    return -EIO;  
	    pfn = vmalloc_to_pfn(vmalloc_area_ptr);  
	    if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,  
				       PAGE_SHARED)) < 0) {  
		    return ret;  
	    }  
#endif

	return 0;
}

static const struct file_operations test_fops = {
	.owner                  = THIS_MODULE,
	.mmap                   = test_mmap,
	.llseek                 = noop_llseek,
};

static struct miscdevice test_dev = {
	0,
	"test",
	&test_fops
};

int init_module(void)
{
	int ret = 0, i = 0;
	char *argv[3], *envp[4];

	//test_buf = kmalloc(8192, GFP_KERNEL);
	//test_buf = (void *)__get_free_pages(GFP_KERNEL, 0);
	test_buf = (char *)vmalloc(1 * PAGE_SIZE);
	if (!test_buf)
		return -ENOMEM;

	SetPageReserved(vmalloc_to_page((void *)(((unsigned long)test_buf))));  

	//memset(test_buf, 0xff, 512);

	ret = misc_register(&test_dev);
	if (ret) {
		printk("can't register test_dev\n");
		return ret;
	}

	test_obj = kobject_create_and_add("test", NULL);
        ret = sysfs_create_group(test_obj, &sysfs_attribute_group);

	argv[i++] = "/aa.out";
	argv[i] = 0;

	i = 0;
	envp[i++] = "HOME=/";
	envp[i++] = "PATH=/usr/bin";
	envp[i] = 0;
	
	ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
	return 0;
}
 
void cleanup_module(void)
{
	if (!test_buf)
		return;

	printk("### [%s:%d]\n", __func__, __LINE__);
	sysfs_remove_group(test_obj, &sysfs_attribute_group);
	kobject_del(test_obj);

	misc_deregister(&test_dev);

	vfree(test_buf);
	//free_pages((unsigned long)test_buf, 0);
	ClearPageReserved(test_buf);
}

MODULE_LICENSE("GPL");
