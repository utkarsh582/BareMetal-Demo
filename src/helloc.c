/* Client code for BareMetal-OS TCP/IP stack
   Implements TCP connection establishment, data transmission, and response handling.
*/

#include "libBareMetal.h"

#define __USE_MISC

/* Global functions */
u16 checksum(u8* data, u16 bytes);
u16 checksum_tcp(u8* data, u16 bytes, u16 protocol, u16 length);
int net_init();
int net_exit();
int net_send(unsigned char* data, unsigned int bytes);
int net_recv(unsigned char* data);
u16 swap16(u16 in);
u32 swap32(u32 in);
void* memset(void* s, int c, int n);
void* memcpy(void* d, const void* s, int n);
int strlen(const char* s);
int memcmp(const void *s1, const void *s2, u32 n);

/* Global defines */
#undef ETH_FRAME_LEN
#define ETH_FRAME_LEN 1518
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPv4 0x0800
#define ETHERTYPE_IPv6 0x86DD
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define PROTOCOL_IP_ICMP 1
#define PROTOCOL_IP_TCP 6
#define PROTOCOL_IP_UDP 11
#define TCP_ACK 16
#define TCP_PSH 8
#define TCP_RST 4
#define TCP_SYN 2
#define TCP_FIN 1

// u8 src_MAC[6] = {0x08, 0x00, 0x27, 0xDD, 0x2E, 0xF1};
// u8 src_MAC[6] = {0x08, 0x00, 0x27, 0x2D, 0xDE, 0x1F};
u8 src_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 dst_IP[4] = {10, 13, 100, 131};    
u8 src_IP[4] = {10, 13, 100, 50};   // Client IP address
unsigned char buffer[ETH_FRAME_LEN];
unsigned char tosend[ETH_FRAME_LEN];
int running = 1, recv_packet_len;

/* TCP and IP header structures */
#pragma pack(1)
typedef struct eth_header {
    u8 dest_mac[6];
    u8 src_mac[6];
    u16 type;
} eth_header;

typedef struct arp_packet {
	eth_header ethernet;
	u16 hardware_type;
	u16 protocol;
	u8 hardware_size;
	u8 protocol_size;
	u16 opcode;
	u8 sender_mac[6];
	u8 sender_ip[4];
	u8 target_mac[6];
	u8 target_ip[4];
} arp_packet; // 28 bytes

typedef struct ipv4_packet {
    eth_header ethernet;
    u8 version;
    u8 dsf;
    u16 total_length;
    u16 id;
    u16 flags;
    u8 ttl;
    u8 protocol;
    u16 checksum;
    u8 src_ip[4];
    u8 dest_ip[4];
} ipv4_packet;

typedef struct tcp_packet {
    ipv4_packet ipv4;
    u16 src_port;
    u16 dest_port;
    u32 seqnum;
    u32 acknum;
    u8 data_offset;
    u8 flags;
    u16 window;
    u16 checksum;
    u16 urg_pointer;
} tcp_packet;


int main(){
    b_output("\nminIP Client2.0\n",(unsigned long)strlen("\nminIP Client2.0\n"));
    net_init();
    u16 dest_port = 8129;
    u8 server_ip[4] ={10,13,100,131};

    //ARP Packet
    arp_packet* tx_arp = (arp_packet*)tosend;
	// Ethernet
	memcpy(tx_arp->ethernet.dest_mac, (u8[]){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 6);
	memcpy(tx_arp->ethernet.src_mac, src_MAC, 6);
	tx_arp->ethernet.type = swap16(ETHERTYPE_ARP);
	// ARP
	tx_arp->hardware_type = swap16(1); // Ethernet
	tx_arp->protocol = swap16(ETHERTYPE_IPv4);
	tx_arp->hardware_size = 6;
	tx_arp->protocol_size = 4;
	tx_arp->opcode = swap16(ARP_REQUEST);
	memcpy(tx_arp->sender_mac, src_MAC, 6);
	memcpy(tx_arp->sender_ip, src_IP, 4);
	// memcpy(tx_arp->target_mac, rx_arp->sender_mac, 6);
	memcpy(tx_arp->target_ip, server_ip, 4);
	// Send the reply
	net_send(tosend, 42);
    b_output("ARP Request Sent. Waiting for Reply\n",(unsigned long)strlen("ARP Request Sent. Waiting for Reply\n"));

    //Recieve ARP Reply
    while (1) {
        recv_packet_len = net_recv(buffer);
        eth_header* rx_eth = (eth_header*)buffer;

        if (recv_packet_len > 0 && swap16(rx_eth->type) == ETHERTYPE_ARP) {
            arp_packet* rx_arp = (arp_packet*)buffer;
            if (swap16(rx_arp->opcode) == ARP_REPLY) {
                if (memcmp(server_ip, rx_arp->sender_ip, 4) == 0) {    
                    b_output("ARP Packet Received\n",(unsigned long)strlen("ARP Packet Received\n"));
                    memcpy(dst_MAC, rx_arp->sender_mac, 6);
                    b_output("Server MAC Address obtained.\n", (unsigned long)strlen("Server MAC Address obtained.\n"));
                    break;
                }else{
                    //ARP Packet
                    arp_packet* tx_arp = (arp_packet*)tosend;
                    // Ethernet
                    memcpy(tx_arp->ethernet.dest_mac, (u8[]){0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 6);
                    memcpy(tx_arp->ethernet.src_mac, src_MAC, 6);
                    tx_arp->ethernet.type = swap16(ETHERTYPE_ARP);
                    // ARP
                    tx_arp->hardware_type = swap16(1); // Ethernet
                    tx_arp->protocol = swap16(ETHERTYPE_IPv4);
                    tx_arp->hardware_size = 6;
                    tx_arp->protocol_size = 4;
                    tx_arp->opcode = swap16(ARP_REQUEST);
                    memcpy(tx_arp->sender_mac, src_MAC, 6);
                    memcpy(tx_arp->sender_ip, src_IP, 4);
                    // memcpy(tx_arp->target_mac, rx_arp->sender_mac, 6);
                    memcpy(tx_arp->target_ip, server_ip, 4);
                    // Send the reply
                    net_send(tosend, 42);
                }
            }
        }
    }

    // TCP Option - Maximum segment size: 1460 bytes
    u8 mss_option[] = {0x02, 0x04, 0x05, 0xb4};  // Kind: MSS(2), Length: 4, MSS Value: 1460

    // TCP Option - SACK permitted
    u8 sack_permitted[] = {0x04, 0x02};  // Kind: SACK Permitted(4), Length: 2
    
    // TCP Option - Timestamps: TSval 2291102, TSecr 0
    u8 timestamp_option[] = {0x08, 0x0A, 0x22, 0x45, 0x0E, 0x56, 0x00, 0x00, 0x00, 0x00};  // TSval: 2291102, TSecr: 0

    // TCP Option - No-Operation (NOP)
    u8 nop_option[] = {0x01};  // Kind: No-Operation (NOP)

    // TCP Option - Window scale: 7 (multiply by 128)
    u8 window_scale_option[] = {0x03, 0x03, 0x07};  // Kind: Window Scale(3), Length: 3, Shift count: 7

    // Combine the options into a single array
    u8 tcp_options[20];
    memcpy(tcp_options, mss_option, sizeof(mss_option));
    memcpy(tcp_options + sizeof(mss_option), sack_permitted, sizeof(sack_permitted));
    memcpy(tcp_options + sizeof(mss_option) + sizeof(sack_permitted), timestamp_option, sizeof(timestamp_option));
    memcpy(tcp_options + sizeof(mss_option) + sizeof(sack_permitted) + sizeof(timestamp_option), nop_option, sizeof(nop_option));
    memcpy(tcp_options + sizeof(mss_option) + sizeof(sack_permitted) + sizeof(timestamp_option) + sizeof(nop_option), window_scale_option, sizeof(window_scale_option));

    tcp_packet* tx_tcp = (tcp_packet*)tosend;
    memset(tx_tcp, 0, sizeof(tcp_packet));

    // Ethernet header 
    memcpy(tx_tcp->ipv4.ethernet.dest_mac, dst_MAC, 6);
    memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
    tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);

    // IPv4 header
    tx_tcp->ipv4.version = 4 << 4 | 5;
    tx_tcp->ipv4.dsf = 0; 
    tx_tcp->ipv4.ttl = 64;
    tx_tcp->ipv4.id = swap16(54321);
    tx_tcp->ipv4.protocol = PROTOCOL_IP_TCP;
    memcpy(tx_tcp->ipv4.src_ip, src_IP, 4);
    memcpy(tx_tcp->ipv4.dest_ip, dst_IP, 4);

    tx_tcp->ipv4.flags = swap16(0x4000); 
    tx_tcp->ipv4.total_length = swap16(20 + 20 + 20);
    tx_tcp->ipv4.checksum = 0;
    tx_tcp->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));

    // TCP header
    tx_tcp->src_port = swap16(12345);
    tx_tcp->dest_port = swap16(dest_port);
    tx_tcp->seqnum = swap32(0);
    tx_tcp->acknum = 0;
    tx_tcp->data_offset = 10 << 4;  
    tx_tcp->flags = TCP_SYN;
    tx_tcp->window = swap16(520);
    tx_tcp->checksum = 0;
    tx_tcp->urg_pointer = 0;

    memcpy((u8*)tx_tcp + sizeof(tcp_packet), tcp_options, sizeof(tcp_options));

    char pseudo_header[12];

    // Set the source and destination IP addresses in the pseudo-header
    memcpy(pseudo_header, src_IP, 4);        // Source IP (4 bytes)
    memcpy(pseudo_header + 4, dst_IP, 4);    // Destination IP (4 bytes)
    pseudo_header[8] = 0;                    // Zero byte
    pseudo_header[9] = PROTOCOL_IP_TCP;      // Protocol (TCP)
    pseudo_header[10] = (u8)((sizeof(tcp_packet) + sizeof(tcp_options) - sizeof(ipv4_packet)) >> 8);  // TCP Length (high byte)
    pseudo_header[11] = (u8)((sizeof(tcp_packet) + sizeof(tcp_options) - sizeof(ipv4_packet)) & 0xFF); // TCP Length (low byte)

    tx_tcp->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + checksum((u8*)tx_tcp + sizeof(ipv4_packet), sizeof(tcp_packet) + sizeof(tcp_options) - sizeof(ipv4_packet));

    // Send the SYN packet
    net_send(tosend, sizeof(tcp_packet) + sizeof(tcp_options)); 
    b_output("SYN packet sent.\n", (unsigned long)strlen("SYN packet sent.\n"));

    while (running)
    {
        recv_packet_len = net_recv(buffer);
        eth_header* rx_eth = (eth_header*)buffer;
		
        if (recv_packet_len > 0){
            if(swap16(rx_eth->type) == ETHERTYPE_IPv4){
                ipv4_packet* rx_ipv4 = (ipv4_packet*)buffer;
                if(rx_ipv4->protocol == PROTOCOL_IP_TCP){
                    tcp_packet* rx_tcp = (tcp_packet*)buffer;
                    if(((rx_tcp->flags == TCP_ACK) && (rx_tcp->seqnum == swap32(1))) || ((rx_tcp->flags & (TCP_SYN | TCP_ACK)) == (TCP_SYN | TCP_ACK))){
                        tcp_packet* tx_ack = (tcp_packet*)tosend;
                        memset(tx_ack, 0 ,sizeof(tcp_packet));
                        // Ethernet
                        memcpy(tx_ack->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
                        memcpy(tx_ack->ipv4.ethernet.src_mac, rx_tcp->ipv4.ethernet.dest_mac, 6);
                        tx_ack->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
                        // IPv4
                        tx_ack->ipv4.version = rx_tcp->ipv4.version;
                        tx_ack->ipv4.ttl = rx_tcp->ipv4.ttl;
                        tx_ack->ipv4.protocol = PROTOCOL_IP_TCP;
                        memcpy(tx_ack->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
                        memcpy(tx_ack->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
                        tx_ack->ipv4.total_length = swap16(20+20);
                        tx_ack->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));
                        // TCP
                        tx_ack->src_port = rx_tcp->dest_port;
                        tx_ack->dest_port = rx_tcp->src_port;
                        tx_ack->seqnum = swap32(swap32(rx_tcp->acknum));
                        tx_ack->acknum = swap32(swap32(rx_tcp->seqnum) + 1);
                        tx_ack->data_offset = 5<<4;
                        tx_ack->flags = TCP_ACK;
                        tx_ack->window = swap16(5840);
                        tx_ack->checksum = 0;
                        tx_ack->urg_pointer = 0;
                        
                        char pseudo_header[12];
                        // Set the source and destination IP addresses in the pseudo-header
                        memcpy(pseudo_header, rx_tcp->ipv4.dest_ip, 4);        // Source IP (4 bytes)
                        memcpy(pseudo_header + 4, rx_tcp->ipv4.src_ip, 4);    // Destination IP (4 bytes)
                        pseudo_header[8] = 0;                    // Zero byte
                        pseudo_header[9] = PROTOCOL_IP_TCP;      // Protocol (TCP)
                        pseudo_header[10] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) >> 8);  // TCP Length (high byte)
                        pseudo_header[11] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) & 0xFF); // TCP Length (low byte)

                        tx_tcp->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + checksum((u8*)tx_tcp + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        
                        //Send ACK packer
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Connection Established\n", (unsigned long)strlen("Connection Established\n"));

                        char* input = "Hello from the client";
                        tcp_packet* tx_psh = (tcp_packet*)tosend;
                        memset(tx_psh, 0, sizeof(tcp_packet));

                        // Ethernet
                        memcpy(tx_psh->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
                        memcpy(tx_psh->ipv4.ethernet.src_mac, rx_tcp->ipv4.ethernet.dest_mac, 6);
                        tx_psh->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
                        // IPv4
                        tx_ack->ipv4.version = rx_tcp->ipv4.version;
                        tx_tcp->ipv4.dsf = rx_tcp->ipv4.dsf; 
                        tx_ack->ipv4.ttl = rx_tcp->ipv4.ttl;
                        tx_psh->ipv4.protocol = PROTOCOL_IP_TCP;
                        memcpy(tx_psh->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
                        memcpy(tx_psh->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
                        int payload_len = strlen(input);
                        tx_psh->ipv4.total_length = swap16(20 + 20 + payload_len); 
                        tx_psh->ipv4.checksum = 0;
                        tx_psh->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));
                        // TCP
                        tx_psh->src_port = rx_tcp->dest_port;
                        tx_psh->dest_port = rx_tcp->src_port;
                        tx_psh->seqnum = swap32(swap32(rx_tcp->acknum));
                        tx_psh->acknum = swap32(swap32(rx_tcp->seqnum) + 1);
                        tx_psh->data_offset = 5 << 4;  // 4-byte aligned

                        tx_psh->flags = TCP_PSH | TCP_ACK;
                        tx_psh->window = swap16(512);
                        tx_psh->checksum = 0; 
                        tx_psh->urg_pointer = 0;

                        // Pseudo-header for checksum calculation
                        char pseudo[12];
                        memcpy(pseudo, tx_psh->ipv4.src_ip, 4);     // Source IP
                        memcpy(pseudo + 4, tx_psh->ipv4.dest_ip, 4); // Destination IP
                        pseudo[8] = 0;                               // Reserved
                        pseudo[9] = PROTOCOL_IP_TCP;                 // Protocol
                        pseudo[10] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len) >> 8);  // Updated TCP Length (high byte)
                        pseudo[11] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len) & 0xFF);  // Updated TCP Length (low byte)

                        // Calculate TCP checksum
                        tx_psh->checksum = checksum(pseudo, sizeof(pseudo)) +
                                        checksum((u8*)tx_psh + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len);

                        // Copy the payload after the TCP header
                        memcpy((u8*)tx_psh + sizeof(tcp_packet), input, payload_len);  // Adjusted offset for options

                        // Send the packet
                        net_send(tosend, sizeof(tcp_packet) + payload_len);  // Include size of options
                        b_output("Message Sent\n", (unsigned long)strlen("Message Sent\n"));
                    }else if((rx_tcp->flags & (TCP_PSH | TCP_ACK)) == (TCP_PSH | TCP_ACK)){
                        tcp_packet* tx_ack = (tcp_packet*)tosend;
                        memset(tx_ack, 0 ,sizeof(tcp_packet));
                        // Ethernet
                        memcpy(tx_ack->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
                        memcpy(tx_ack->ipv4.ethernet.src_mac, rx_tcp->ipv4.ethernet.dest_mac, 6);
                        tx_ack->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
                        // IPv4
                        tx_ack->ipv4.version = rx_tcp->ipv4.version;
                        tx_ack->ipv4.ttl = rx_tcp->ipv4.ttl;
                        tx_ack->ipv4.protocol = PROTOCOL_IP_TCP;
                        memcpy(tx_ack->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
                        memcpy(tx_ack->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
                        tx_ack->ipv4.total_length = swap16(20+20);
                        tx_ack->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));
                        // TCP
                        tx_ack->src_port = rx_tcp->dest_port;
                        tx_ack->dest_port = rx_tcp->src_port;
                        tx_ack->seqnum = swap32(swap32(rx_tcp->acknum));
                        tx_ack->acknum = swap32(swap32(rx_tcp->seqnum) + (swap16(rx_tcp->ipv4.total_length) - 52));
                        tx_ack->data_offset = 5<<4;
                        tx_ack->flags = TCP_ACK;
                        tx_ack->window = swap16(5840);
                        tx_ack->checksum = 0;
                        tx_ack->urg_pointer = 0;
                        
                        char pseudo_header[12];
                        // Set the source and destination IP addresses in the pseudo-header
                        memcpy(pseudo_header, rx_tcp->ipv4.dest_ip, 4);        // Source IP (4 bytes)
                        memcpy(pseudo_header + 4, rx_tcp->ipv4.src_ip, 4);    // Destination IP (4 bytes)
                        pseudo_header[8] = 0;                    // Zero byte
                        pseudo_header[9] = PROTOCOL_IP_TCP;      // Protocol (TCP)
                        pseudo_header[10] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) >> 8);  // TCP Length (high byte)
                        pseudo_header[11] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) & 0xFF); // TCP Length (low byte)

                        tx_tcp->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + checksum((u8*)tx_tcp + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        
                        //Send ACK packer
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Ack Sent\n", (unsigned long)strlen("Ack Sent\n"));

                        char* message = (char*)rx_tcp + sizeof(tcp_packet) + 12;
						b_output(message,(unsigned long)strlen(message));
                        tcp_packet* tx_fin = (tcp_packet*)tosend;
                        memset(tx_fin, 0 ,sizeof(tcp_packet));
                        // Ethernet
                        memcpy(tx_fin->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
                        memcpy(tx_fin->ipv4.ethernet.src_mac, rx_tcp->ipv4.ethernet.dest_mac, 6);
                        tx_fin->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
                        // IPv4
                        tx_fin->ipv4.version = rx_tcp->ipv4.version;
                        tx_fin->ipv4.ttl = rx_tcp->ipv4.ttl;
                        tx_fin->ipv4.protocol = PROTOCOL_IP_TCP;
                        memcpy(tx_fin->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
                        memcpy(tx_fin->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
                        tx_fin->ipv4.total_length = swap16(20 + 20);
                        tx_fin->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));
                        // TCP
                        tx_fin->src_port = rx_tcp->dest_port;
                        tx_fin->dest_port = rx_tcp->src_port;
                        tx_fin->seqnum = swap32(swap32(rx_tcp->acknum));
                        tx_fin->acknum = swap32(swap32(rx_tcp->seqnum) + 1);
                        tx_fin->data_offset = 5 << 4;
                        tx_fin->flags = TCP_FIN | TCP_ACK;  // Set FIN and ACK flags
                        tx_fin->window = swap16(5840);
                        tx_fin->checksum = 0;
                        tx_fin->urg_pointer = 0;

                        char pseudo[12];
                        memcpy(pseudo, tx_fin->ipv4.src_ip, 4);     // Source IP
                        memcpy(pseudo + 4, tx_fin->ipv4.dest_ip, 4); // Destination IP
                        pseudo[8] = 0;                               // Reserved
                        pseudo[9] = PROTOCOL_IP_TCP;                 // Protocol
                        pseudo[10] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) >> 8);  // Updated TCP Length (high byte)
                        pseudo[11] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) & 0xFF);  // Updated TCP Length (low byte)

                        // Calculate TCP checksum
                        tx_fin->checksum = checksum(pseudo, sizeof(pseudo)) +
                                        checksum((u8*)tx_fin + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));

                        // Send the ACK packet
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Connection Break Request.\n", (unsigned long)strlen("Connection Break Request.\n"));
                        break;
                    }else if((rx_tcp->flags & (TCP_FIN | TCP_ACK)) == (TCP_FIN | TCP_ACK)){
                        tcp_packet* tx_fin = (tcp_packet*)tosend;
                        memset(tx_fin, 0 ,sizeof(tcp_packet));
                        // Ethernet
                        memcpy(tx_fin->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
                        memcpy(tx_fin->ipv4.ethernet.src_mac, rx_tcp->ipv4.ethernet.dest_mac, 6);
                        tx_fin->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
                        // IPv4
                        tx_fin->ipv4.version = rx_tcp->ipv4.version;
                        tx_fin->ipv4.ttl = rx_tcp->ipv4.ttl;
                        tx_fin->ipv4.protocol = PROTOCOL_IP_TCP;
                        memcpy(tx_fin->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
                        memcpy(tx_fin->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
                        tx_fin->ipv4.total_length = swap16(20+20);
                        tx_fin->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));
                        // TCP
                        tx_fin->src_port = rx_tcp->dest_port;
                        tx_fin->dest_port = rx_tcp->src_port;
                        tx_fin->seqnum = swap32(swap32(rx_tcp->acknum));
                        tx_fin->acknum = swap32(swap32(rx_tcp->seqnum) + 1);
                        tx_fin->data_offset = 5<<4;
                        tx_fin->flags = TCP_ACK;
                        tx_fin->window = swap16(5840);
                        tx_fin->checksum = 0;
                        tx_fin->urg_pointer = 0;
                        
                        char pseudo_header[12];
                        // Set the source and destination IP addresses in the pseudo-header
                        memcpy(pseudo_header, rx_tcp->ipv4.dest_ip, 4);        // Source IP (4 bytes)
                        memcpy(pseudo_header + 4, rx_tcp->ipv4.src_ip, 4);    // Destination IP (4 bytes)
                        pseudo_header[8] = 0;                    // Zero byte
                        pseudo_header[9] = PROTOCOL_IP_TCP;      // Protocol (TCP)
                        pseudo_header[10] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) >> 8);  // TCP Length (high byte)
                        pseudo_header[11] = (u8)((sizeof(tcp_packet) - sizeof(ipv4_packet)) & 0xFF); // TCP Length (low byte)

                        tx_tcp->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + checksum((u8*)tx_tcp + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        
                        //Send ACK packer
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Connection Terminated\n", (unsigned long)strlen("Connection terminated\n"));
                        break;
                    }
                }
            }
        }
    }
    b_output("Done successfully\n", (unsigned long)strlen("Done successfully\n"));
    net_exit();

    return 0;
}

/* checksum and checksum_tcp functions remain unchanged */



/* checksum - Calculate a checksum value */
// Returns 16-bit checksum
u16 checksum(u8* data, u16 bytes)
{
	u32 sum = 0;
	u16 i;

	for (i=0; i<bytes-1; i+=2) // Add up the words
		sum += *(u16 *) &data[i];

	if (bytes & 1) // Add the left-over byte if there is one
		sum += (u8) data[i];

	while (sum >> 16) // Fold total to 16-bits
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ~sum; // Return 1's complement
}


/* checksum_tcp - Calculate a TCP checksum value */
// Returns 16-bit checksum
u16 checksum_tcp(u8* data, u16 bytes, u16 protocol, u16 length)
{
	u32 sum = 0;
	u16 i;
	data -= 8; // Start at the source and dest IPs
	bytes += 8;

	for (i=0; i<bytes-1; i+=2) // Add up the words
		sum += *(u16 *) &data[i];

	if (bytes & 1) // Add the left-over byte if there is one
		sum += (u8) data[i];

	sum += swap16(protocol);
	sum += swap16(length);

	while (sum >> 16) // Fold total to 16-bits
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ~sum; // Return 1's complement
}


/* net_init - Initialize a raw socket */
int net_init()
{
	u64 val0 = 0, val1 = 0, val2 = 0;
	val0 = b_system(MAC_GET, val1, val2); // Get the MAC as a u64
	char * MAC = (void*)&val0;

	src_MAC[0] = MAC[5]; // Get the MAC bytes in the correct order
	src_MAC[1] = MAC[4];
	src_MAC[2] = MAC[3];
	src_MAC[3] = MAC[2];
	src_MAC[4] = MAC[1];
	src_MAC[5] = MAC[0];

	return 0;
}


/* net_exit - Clean up and exit */
int net_exit()
{
	return 0;
}

/* net_send - Send a raw Ethernet packet */
// Wrapper for OS send function
// Returns number of bytes sent
int net_send(unsigned char* data, unsigned int bytes)
{
	b_net_tx(data, bytes, 0);
}


/* net_recv - Receive a raw Ethernet packet */
// Wrapper for OS recv function
// Returns number of bytes read
int net_recv(unsigned char* data)
{
	return b_net_rx(data, 0);
}


/* swap16 - Change endianness on a 16-bit value */
// x86-64 uses little-endian while IP uses big-endian
u16 swap16(u16 in)
{
	u16 out = in<<8 | ((in&0xff00)>>8);
	return out;
}


/* swap32 - Change endianness on a 32-bit value */
// x86-64 uses little-endian while IP uses big-endian
u32 swap32(u32 in)
{
	u32 out = in<<24 | ((in&0xff00)<<8) | ((in&0xff0000)>>8) | ((in&0xff000000)>>24);
	return out;
}

void* memset(void* s, int c, int n)
{
	char* _src;

	_src = (char*)s;

	while (n--) {
		*_src++ = c;
	}

	return s;
}

void* memcpy(void* d, const void* s, int n)
{
	char* dest;
	char* src;

	dest = (char*)d;
	src = (char*)s;

	while (n--) {
		*dest++ = *src++;
	}

	return d;
}

int strlen(const char* s)
{
	int r = 0;

	for(; *s++ != 0; r++) { }

	return r;
}

int memcmp(const void *s1, const void *s2, u32 n) {
    unsigned char *p1 = (unsigned char *)s1;
    unsigned char *p2 = (unsigned char *)s2;

    for (u32 i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];  // Return the difference between the first non-matching byte
        }
    }

    return 0;  // Return 0 if all bytes are identical
}

/* EOF */