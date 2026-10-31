#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Voce");
MODULE_DESCRIPTION("NB9 Kernel Driver");

static int __init nb9_init(void) {
    printk(KERN_INFO "NB9 Driver: Carregado com sucesso!\n");
    return 0;
}

static void __exit nb9_exit(void) {
    printk(KERN_INFO "NB9 Driver: Descarregado.\n");
}

module_init(nb9_init);
module_exit(nb9_exit);
