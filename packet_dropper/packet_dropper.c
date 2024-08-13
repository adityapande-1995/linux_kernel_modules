#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/random.h>

static struct nf_hook_ops nfho;

static unsigned int drop_percentage = 20;  // Default drop percentage
module_param(drop_percentage, uint, 0644);
MODULE_PARM_DESC(drop_percentage, "Percentage of packets to drop");

unsigned int drop_packet(void *priv, struct sk_buff *skb,
                         const struct nf_hook_state *state) {
    unsigned int rand;

    get_random_bytes(&rand, sizeof(rand));

    // Drop packets based on the configured percentage
    if (rand % 100 < drop_percentage) {
        printk(KERN_INFO "Dropping packet\n");
        return NF_DROP;
    }

    return NF_ACCEPT;
}

static int __init init_module_fn(void) {
    nfho.hook = drop_packet;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET;
    nfho.priority = NF_IP_PRI_FIRST;

    if (nf_register_net_hook(&init_net, &nfho)) {
        printk(KERN_INFO "Failed to register net hook\n");
        return -1;
    }

    printk(KERN_INFO "Packet dropper loaded with %u%% drop\n", drop_percentage);
    return 0;
}

static void __exit cleanup_module_fn(void) {
    nf_unregister_net_hook(&init_net, &nfho);
    printk(KERN_INFO "Packet dropper unloaded\n");
}

module_init(init_module_fn);
module_exit(cleanup_module_fn);

MODULE_LICENSE("GPL");

