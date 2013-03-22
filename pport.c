#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/ioport.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#include<asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

#define device_name "pport"

static int device_open=0;
int PP_MAJOR=80;
int port;

int pport_init(void);
void pport_exit(void);

static int pp_open(struct inode *,struct file * );
static int pp_release(struct inode *,struct file *);
static ssize_t pp_read(struct file *,char __user *,size_t ,loff_t *);
static ssize_t pp_write(struct file*,const char __user *,size_t,loff_t *);

struct file_operations fops=
  {
    .read=pp_read,
    .write=pp_write,
    .open=pp_open,
    .release=pp_release
  };

int pport_init(void)
{
  int ret=register_chrdev(PP_MAJOR,device_name,&fops);
  if(ret<0)
    {
      printk(KERN_ALERT "Error in register_chrdev with %d\n",ret);
      return ret;
}
  port=check_region(0x378,1);
  if(port)
    {
      printk(KERN_ALERT "Error in pp_port check_region\n");
      ret=port;      
      goto fail;
}
  request_region(0x378,1,"pp_port");
  printk(KERN_INFO "pp_port insert\n");
  return 0;
 fail:
  pport_exit();
  return ret;
}

void pport_exit(void)
{
  unregister_chrdev(PP_MAJOR,device_name);
  if(!port)
      release_region(0x378,1);
      printk(KERN_INFO "remove PP_port");
}

static int pp_open(struct inode *inode,struct file *filp)
{
  if(device_open)
  return -EBUSY;
  device_open++;
  try_module_get(THIS_MODULE);
  return 0;
}

static int pp_release(struct inode *inode,struct file *filp)
{
  device_open--;
  module_put(THIS_MODULE);
  return 0;
}

static ssize_t pp_read(struct file *filp,char __user *buffer,size_t length,loff_t *offset)
{
  char pp_buffer;
  pp_buffer=inb(0x378);
  copy_to_user(buffer,&pp_buffer,1);
  if(*offset==0)
    {
     *offset+=1;
     return 1;    
}else
    return 0;
}

static ssize_t pp_write(struct file *filp,const char __user *buffer,size_t length,loff_t *offset_t)
{
  char pp_buffer,*tmp;
  tmp=length+buffer-1;
  copy_from_user(&pp_buffer,tmp,1);
  outb(pp_buffer,0x378);
  return 0;
}

module_init(pport_init);
module_exit(pport_exit);
