#include<linux/kernel.h>
#include<linux/device.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include"platform_header.h"//privare data structure
#include<linux/platform_device.h>// definations are here

/*
callback to free the devices adter all the reference have gone away
this should be set ny the allocater of the device
if some data is allocated dynamically after device is unregisred.. this api will free that memory
*/
void pdev_release(struct device *dev)
{


	printk("pdev_release: invoked");

}



//private data to send to driver to use this data in driver further
struct pdev_private_data pdev_prv_data[2] ={

[0] = {.size = 512, .perm = RDWR, .serial_number="PDEV_12345"},
[1] = {.size = 512, .perm = RDWR, .serial_number="PDEV_678910"}

};


//we are creating to devices to match with driver with compatable string meathod.. 
//match function will check for the same string in drievr if matched then devices can use our driver
struct platform_device pdev_1 ={

	.name = "platform_device",
	.id   = 0,
	.dev  ={
		.platform_data = &pdev_prv_data[0],
		.release = pdev_release
}
};

struct platform_device pdev_2 ={

	.name = "platform_device",
	.id   = 1,
	.dev  ={
		.platform_data = &pdev_prv_data[1],
		.release = pdev_release
}
};


static int __init pdev_init(void)
{
	printk("pdev_int: invoked");
	platform_device_register(&pdev_1);
	platform_device_register(&pdev_2);
	printk("pdev_int: device registration is done\n");
        return 0;
}



static void __exit pdev_exit(void)
{

	printk("pdev_exit: invoked");
	platform_device_unregister(&pdev_1);
	platform_device_unregister(&pdev_2);
	printk("pdev_exit: devices are unregistered\n");


}
module_init(pdev_init);
module_exit(pdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ABHI");
MODULE_DESCRIPTION("platform driver");

/*
/sys/devices/platform:

is the path to see if the devices are register or not


-> this is the logs flow:
km@KernelMasters:~/Desktop$ sudo dmesg
[ 5911.275753] pdev_int: invoked
[ 5911.275815] pdev_int: device registration is done
[ 5921.147421] pdev_exit: invoked
[ 5921.147464] pdev_release: invoked
[ 5921.147473] pdev_release: invoked
[ 5921.147473] pdev_exit: devices are unregistered



-> while interacting with driver this is the flow
m@KernelMasters:~/Desktop$ sudo dmesg
[ 7336.690754] pdev_int: invoked
[ 7336.691168] pdev_int: device registration is done
[ 7351.280177] pdrv_int: invoked
[ 7351.280199] pdrv_probe: invoked device is detected
[ 7351.280225] pdrv_probe: invoked device is detected
[ 7351.280235] pdrv_int: drievr is registered
[ 7370.274286] pdev_exit: invoked
[ 7370.274306] pdrv_release: device is removed
[ 7370.274329] pdev_release: invoked
[ 7370.274337] pdrv_release: device is removed
[ 7370.274341] pdev_release: invoked
[ 7370.274342] pdev_exit: devices are unregistered



*/
