#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include"platform_header.h"//privare data structure
#include<linux/platform_device.h>// definations are here
#include<linux/slab.h>



//device private data structure,will allocatde memory when one device is detevted in probe
struct pdrv_dev_private_data
{
	struct pdev_private_data pdev_prv_data;
	char *buffer;
	dev_t dev_num;
	struct cdev cdev;
};

//driver private data structure.create one gloable var to use
struct pdrv_private_data
{
	int total_devices;
	dev_t device_num_base;
	struct class *class_p;
	struct device *device_p;

};

struct pdrv_private_data pdrv_private_data;



//this is called when matched device is found
int pdrv_probe(struct platform_device *pdev)
{
	struct pdrv_dev_private_data *pdrv_d_prv_data;
	struct pdev_private_data *pdev_data;//struct from header use this to take data and again assing to local pdev_data to use


	//get the platform data
	pdev_data = pdev->dev.platform_data;
	if(pdev_data == NULL)
	{	
		printk("pdrv_probe: pdev_data failed");
		return -1;
	}

	//allocate memory for local var to filldata and use further
	pdrv_d_prv_data = kzalloc(sizeof(pdrv_d_prv_data),GFP_KERNEL);
	if(pdrv_d_prv_data == NULL)
	{

		printk("pdrv_probe: kzalloc failed");
		return -1;
	}

	//filling data here
	pdrv_d_prv_data->pdev_prv_data.size = pdev_data->size;
	pdrv_d_prv_data->pdev_prv_data.perm = pdev_data->perm;
	pdrv_d_prv_data->pdev_prv_data.serial_number = pdev_data->serial_number;

	pdev->dev.driver_data = pdrv_d_prv_data;//store address here  to free memory in remove further

	printk("pdrv_probe: invoked device is detected");
	printk("device size: %d\n device perm: %d\n device serial_number: %s\n",pdrv_d_prv_data->pdev_prv_data.size,
										pdrv_d_prv_data->pdev_prv_data.perm,
										pdrv_d_prv_data->pdev_prv_data.serial_number);



	//here do cdev inint, cdev add, and device create if you want to create read,wriet open..
	//		
	/*why  cdev here??
	Connects file operations to one device
	Each device needs,Its own cdev,Its own minor number


	why device create?
	created even if hardware doesn’t exist
	/dev node should exist only if device exists

*/
	return 0;
}

//this is called whrn device is removed from system
int pdrv_release(struct platform_device *pdev)
{

	printk("pdrv_release: device is removed");
	//free the mmemory
	kfree(pdev->dev.driver_data);

	printk("pdrv_release: memory is freed");
	return 0;
}


struct platform_driver pdrv_data = {

	.probe  = pdrv_probe,
	.remove = pdrv_release,
	.driver ={
		.name = "platform_device"
}
};

#define MAX_DEVICES 10
static int __init pdrv_init(void)
{int ret;

	printk("pdrv_int: invoked");

	//1. dynamically allocate a device numberbfor MAXDEVICES
	ret = alloc_chrdev_region(&pdrv_private_data.device_num_base,0,MAX_DEVICES,"platfrom_char_driver");
	if(ret < 0)
	{
		printk("pdrv_init: alloc_chardev_Region failed");
		return ret =-1;
	}


	//2. create device class under /sys/class
	pdrv_private_data.class_p = class_create(THIS_MODULE,"platform_class");
	if(pdrv_private_data.class_p == NULL)
	{
	
		printk("pdrv_init: class_create failed");
		return ret =-1;
	}

	//3. register a platform driver
	platform_driver_register(&pdrv_data);
	printk("pdrv_int: drievr is registered");
        return 0;
}



static void __exit pdrv_exit(void)
{

	printk("pdrv_exit: invoked");
	platform_driver_unregister(&pdrv_data);

        class_destroy(pdrv_private_data.class_p);

	unregister_chrdev_region(pdrv_private_data.device_num_base,MAX_DEVICES);
	printk("pdrv_exit: drievr is unregistered");


}
module_init(pdrv_init);
module_exit(pdrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ABHI");
MODULE_DESCRIPTION("platform driver1");


/*
this is the final flow of this driver

km@KernelMasters:~/Desktop$ sudo dmesg
[11609.075734] pdrv_exit: drievr is unregistered
[11620.412819] pdev_int: invoked
[11620.413659] pdev_int: device registration is done
[11627.179968] pdrv_int: invoked
[11627.180547] pdrv_probe: invoked device is detected
[11627.180550] device size: 512
                device perm: 1
                device serial_number: PDEV_12345
[11627.183148] pdrv_probe: invoked device is detected
[11627.183152] device size: 512
                device perm: 1
                device serial_number: PDEV_678910
[11627.183196] pdrv_int: drievr is registered
[11676.863471] pdev_exit: invoked
[11676.863515] pdrv_release: device is removed
[11676.864426] pdev_release: invoked
[11676.864456] pdrv_release: device is removed
[11676.864474] pdev_release: invoked
[11676.864475] pdev_exit: devices are unregistered
[11680.712320] pdrv_exit: invoked


LINKS::

https://embeddedprep.com/platform-devices-in-linux/

https://kernel.org/doc/html/latest/driver-api/driver-model/platform.html

*/
