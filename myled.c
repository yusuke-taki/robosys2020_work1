// SPDX-License-Identifier: GPL-3.0

/*
 * Copyright(C) 2020 YUsuke Taki and Ryuichi Ueda.All right reserved.
*/

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/io.h>
#include<linux/ctype.h>
#include<linux/delay.h>

MODULE_AUTHER("Yusuke Taki and Ryuichi Ueda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENCE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static struct  u32 *gpio_base = NULL;


static ssize_t led write(struct file*flip, const char* buf, size_t count, loff_t* pos)
{
	int i, j, result_num;
	char c;
	typedef struct s_morse{
		char str_type;
		int size;
		int code[32]
	} MORSE;
	
	MORSE mo[]={
		{'0', 22, {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0}},
		{'1', 18, {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0}},
		{'2', 18, {0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0}},
		{'3', 16, {0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,0}},
		{'4', 14, {0,1,0,1,0,1,0,1,0,1,1,1,0,0}},
		{'5', 12, {0,1,0,1,0,1,0,1,0,1,0,0}},
		{'6', 14, {0,1,1,1,0,1,0,1,0,1,0,1,0,0}},
		{'7', 16, {0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0}},
		{'8', 18, {0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,0}},
		{'9', 20, {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,0}}
	};

	if(copy_from_user(&c,buf,sizeof(char))){
		return -EFAULT;
	}
	printk(KERN_INFO "receive %c\n", c);

	if(isdigit((unsigned char)c)){
		result_num = c - '0';
	}
	printk(KERN_INFO "result_num = %d\n", result_num);

	if(result_num % 2 == 0){
		for(i=0;i<mo[result_num].size;i++){
			if(mo[result_num].code[i]){
				gpio_base[7] = 1 << 25;
			}
			else{
				gpio_base[10] = 1 << 25;
			}
			for(j=0;j<100;j++){
				udelay(1000);
			}
		}
	}
	return 1;
}

static ssize_t sushi_read(struct file* flip, char*buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {'s', 'u', 's', 'h', 'i', 0x0A};
	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_INFO "sushi : copy_to_user failed\n");
	return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void)
{
	int retval;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chardev_region faild.\n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdev, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, major:%d, MAJOR(dev),MINOR(dev)");
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0x3f200000, 0xA0);

	const u32 led = {24,25};
	const u32 index = led/10;
	const u32 shift = (led%10)*3;
	const u32 mask = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask | (0x1 << shift));

	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
				
	



	
