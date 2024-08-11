#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/net.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Pande");
MODULE_DESCRIPTION("A Simple Network Packet Sniffer with Process ID");
MODULE_VERSION("0.2");

static struct nf_hook_ops netfilter_ops_in;  // Hook for incoming packets
static struct nf_hook_ops netfilter_ops_out; // Hook for outgoing packets

// Helper function to get the PID from a socket
static pid_t get_socket_pid(struct sk_buff *skb) {
    struct sock *sk = skb_to_full_sk(skb);
    if (sk && sk->sk_socket && sk->sk_socket->file) {
        struct pid *pid_struct = sk->sk_socket->file->f_owner.pid;
        if (pid_struct) {
            return pid_vnr(pid_struct);
        }
    }
    return -1; // Return -1 if PID is not found
}

static unsigned int packet_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
    struct tcphdr *tcp_header;
    struct udphdr *udp_header;
    char source[16], dest[16];
    pid_t pid;

    snprintf(source, 16, "%pI4", &ip_header->saddr);
    snprintf(dest, 16, "%pI4", &ip_header->daddr);

    pid = get_socket_pid(skb);

    switch (ip_header->protocol) {
        case IPPROTO_TCP:
            tcp_header = (struct tcphdr *)(skb_transport_header(skb));
            printk(KERN_INFO "Sniffer: TCP packet: %s:%d -> %s:%d, PID: %d\n",
                   source, ntohs(tcp_header->source), dest, ntohs(tcp_header->dest), pid);
            break;
        case IPPROTO_UDP:
            udp_header = (struct udphdr *)(skb_transport_header(skb));
            printk(KERN_INFO "Sniffer: UDP packet: %s:%d -> %s:%d, PID: %d\n",
                   source, ntohs(udp_header->source), dest, ntohs(udp_header->dest), pid);
            break;
        default:
            printk(KERN_INFO "Sniffer: Other packet: %s -> %s, protocol: %d, PID: %d\n",
                   source, dest, ip_header->protocol, pid);
            break;
    }
    return NF_ACCEPT; // Allow packet to continue through the stack
}

static int __init packet_sniffer_init(void) {
    printk(KERN_INFO "Sniffer: Initializing packet sniffer module with PID tracking\n");

    // Hook for incoming packets
    netfilter_ops_in.hook = packet_hook;
    netfilter_ops_in.pf = PF_INET;
    netfilter_ops_in.hooknum = NF_INET_PRE_ROUTING;
    netfilter_ops_in.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &netfilter_ops_in);

    // Hook for outgoing packets
    netfilter_ops_out.hook = packet_hook;
    netfilter_ops_out.pf = PF_INET;
    netfilter_ops_out.hooknum = NF_INET_POST_ROUTING;
    netfilter_ops_out.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &netfilter_ops_out);

    return 0;
}

static void __exit packet_sniffer_exit(void) {
    printk(KERN_INFO "Sniffer: Exiting packet sniffer module\n");
    nf_unregister_net_hook(&init_net, &netfilter_ops_in);
    nf_unregister_net_hook(&init_net, &netfilter_ops_out);
}

module_init(packet_sniffer_init);
module_exit(packet_sniffer_exit);

