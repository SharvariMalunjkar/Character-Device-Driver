#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

//1. Creating structue for char driver
struct fake_driver{
    char data[100];
    struct semaphore sem;
}virtual_device;

//2. Declaring variables
struct cdev *mcdev;
int major_number;
int ret;

dev_t dev_num;


#define DEVICE_NAME "FirstCharDriver"


int device_open(struct inode *inode, struct file *flip){
     if(down_interruptible(&virtual_device.sem)!=0){
         printk(KERN_ALERT "FirstCharDriver: could not lock device during open.");
         return -1;
     }
     printk(KERN_INFO "FirstCharDriver: opened Device");
     return 0;
}


ssize_t device_read(struct file *flip,  char* bufStoreData, size_t bufCount, loff_t* curOffset){

     printk(KERN_INFO "FirstCharDriver: Reading from Device....");
     ret = copy_to_user(bufStoreData,virtual_device.data,bufCount);
     return ret;
}


ssize_t device_write(struct file*flip, const char*bufSourceData, size_t bufCount, loff_t* curOffset){
     printk(KERN_INFO "FirstCharDevice: Writing to Deive....");
     ret = copy_from_user(virtual_device.data,bufSourceData,bufCount);
     return ret;
}

int device_close(struct inode *inode, struct file* flip){
    up(&virtual_device.sem);
    printk(KERN_INFO "FirstCharDriver: closed device");
    return 0;

}



//6 File operations declaration
struct file_operations fops = {
	.owner = THIS_MODULE,
        .open = device_open,
        .release= device_close,
        .write = device_write,
        .read = device_read
};



//3. Register device
static int driver_entry(void){

    ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
    if(ret<0){
     printk(KERN_ALERT "FirstCharDrier: failed to allocate major number");
     return ret;
     }
    major_number = MAJOR(dev_num);
    printk(KERN_INFO "FirstCharDriver: major number is %d",major_number);
    printk(KERN_INFO "\tuse\"mknod/dev/%s C %d 0\" for device file", DEVICE_NAME,major_number);

    mcdev = cdev_alloc();
    mcdev->ops = &fops;
    mcdev->owner = THIS_MODULE;
    ret =cdev_add(mcdev,dev_num,1);
    if(ret<0){
        printk(KERN_ALERT "FirstCharDriver: unable to add cdrv to kernel...");
        return ret;
     }
     sema_init(&virtual_device.sem,1);
    return 0;
}



static void driver_exit(void){
   cdev_del(mcdev);
   unregister_chrdev_region(dev_num,1);
   printk(KERN_ALERT "FirstCharDriver: unloaded module.");
}



module_init(driver_entry);
module_exit(driver_exit);
