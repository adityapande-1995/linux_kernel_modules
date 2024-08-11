// simple_char_driver.c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_driver"
#define CLASS_NAME  "simple_char_class"

static int majorNumber;
static struct class *simpleCharClass = NULL;
static struct device *simpleCharDevice = NULL;

// Function prototypes
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init simple_char_driver_init(void) {
    printk(KERN_INFO "SimpleChar: Initializing the SimpleChar LKM\n");

    // Try to dynamically allocate a major number
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "SimpleChar failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "SimpleChar: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    simpleCharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(simpleCharClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(simpleCharClass);
    }
    printk(KERN_INFO "SimpleChar: device class registered correctly\n");

    // Register the device driver
    simpleCharDevice = device_create(simpleCharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(simpleCharDevice)) {
        class_destroy(simpleCharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(simpleCharDevice);
    }
    printk(KERN_INFO "SimpleChar: device class created correctly\n");
    return 0;
}

static void __exit simple_char_driver_exit(void) {
    device_destroy(simpleCharClass, MKDEV(majorNumber, 0));
    class_unregister(simpleCharClass);
    class_destroy(simpleCharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "SimpleChar: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "SimpleChar: Device has been opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "SimpleChar: Device successfully closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "SimpleChar: Reading from the device\n");
    return 0;  // Return 0 bytes read
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "SimpleChar: Writing to the device\n");
    return len;  // Return the number of bytes written
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Pande");
MODULE_DESCRIPTION("A simple Linux char driver");
MODULE_VERSION("0.1");

