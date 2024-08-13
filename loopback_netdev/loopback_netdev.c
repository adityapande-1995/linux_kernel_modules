#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *loopback_dev;

static int loopback_open(struct net_device *dev)
{
    netif_start_queue(dev);
    printk(KERN_INFO "loopback_netdev: Interface %s opened\n", dev->name);
    return 0;
}

static int loopback_stop(struct net_device *dev)
{
    netif_stop_queue(dev);
    printk(KERN_INFO "loopback_netdev: Interface %s closed\n", dev->name);
    return 0;
}

static netdev_tx_t loopback_xmit(struct sk_buff *skb, struct net_device *dev)
{
    printk(KERN_INFO "loopback_netdev: Packet received on %s\n", dev->name);
    
    skb_orphan(skb);  // Orphan the skb so we can reuse it
    skb->dev = dev;   // Set the device to itself for loopback
    skb->protocol = eth_type_trans(skb, dev);
    skb_reset_network_header(skb);
    skb->ip_summed = CHECKSUM_UNNECESSARY; // No need to verify checksum

    netif_rx(skb);  // Send packet back to the network stack

    printk(KERN_INFO "loopback_netdev: Packet transmitted back on %s\n", dev->name);
    return NETDEV_TX_OK;
}

static const struct net_device_ops loopback_netdev_ops = {
    .ndo_open = loopback_open,
    .ndo_stop = loopback_stop,
    .ndo_start_xmit = loopback_xmit,
};

static void loopback_setup(struct net_device *dev)
{
    dev->netdev_ops = &loopback_netdev_ops;
    dev->flags |= IFF_LOOPBACK;
    dev->hard_header_len = 0;
    dev->addr_len = 0;
    dev->mtu = 1500;
    dev->tx_queue_len = 1000;
    ether_setup(dev); // Basic setup for an Ethernet device
}

static int __init loopback_init(void)
{
    printk(KERN_INFO "loopback_netdev: Initializing loopback network device\n");

    loopback_dev = alloc_netdev(0, "loopback0", NET_NAME_UNKNOWN, loopback_setup);
    if (register_netdev(loopback_dev)) {
        printk(KERN_ALERT "loopback_netdev: Failed to register loopback device\n");
        free_netdev(loopback_dev);
        return -ENODEV;
    }

    printk(KERN_INFO "loopback_netdev: Loopback network device registered as %s\n", loopback_dev->name);
    return 0;
}

static void __exit loopback_exit(void)
{
    printk(KERN_INFO "loopback_netdev: Unregistering loopback network device %s\n", loopback_dev->name);
    unregister_netdev(loopback_dev);
    free_netdev(loopback_dev);
    printk(KERN_INFO "loopback_netdev: Loopback network device unregistered\n");
}

module_init(loopback_init);
module_exit(loopback_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Pande");
MODULE_DESCRIPTION("A simple loopback network driver");

