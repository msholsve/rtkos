/*  
 *  hello-1.c - The simplest kernel module.
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/proc_fs.h>

static struct proc_dir_entry *Our_Proc_File;

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	printk(KERN_INFO "procfile_read called\n");

	if (offset > 0)
	{
		return 0;
	}
	else
	{
		return sprintf(buffer, "NEIN NEIN NEIN NEIN\n");
	}
}

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");

	Our_Proc_File = create_proc_entry("nein", 0644, NULL);
	Our_Proc_File->read_proc = procfile_read;
	
	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}


void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
	remove_proc_entry("nein", NULL);
}
