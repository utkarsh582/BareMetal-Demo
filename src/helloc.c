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

//Construction and Sending of ARP Request Packet
void send_arp_request(u8* dest_ip, u8* dest_mac, u8* src_mac, u8* src_ip, u8* tosend) {
    arp_packet* tx_arp = (arp_packet*)tosend;

    // Ethernet header
    memcpy(tx_arp->ethernet.dest_mac, dest_mac, 6);
    memcpy(tx_arp->ethernet.src_mac, src_mac, 6);
    tx_arp->ethernet.type = swap16(ETHERTYPE_ARP);

    // ARP packet
    tx_arp->hardware_type = swap16(1); // Ethernet
    tx_arp->protocol = swap16(ETHERTYPE_IPv4);
    tx_arp->hardware_size = 6;
    tx_arp->protocol_size = 4;
    tx_arp->opcode = swap16(ARP_REQUEST);
    memcpy(tx_arp->sender_mac, src_mac, 6);
    memcpy(tx_arp->sender_ip, src_ip, 4);
    memcpy(tx_arp->target_ip, dest_ip, 4);

    // Send the ARP request
    net_send(tosend, 42);
}

//Construction of Pseudo Header for TCP Checksum
void create_pseudo_header(char *pseudo_header, u8 *src_ip, u8 *dst_ip, u8 tcp_len) {
    memcpy(pseudo_header, src_ip, 4);        // Source IP (4 bytes)
    memcpy(pseudo_header + 4, dst_ip, 4);    // Destination IP (4 bytes)
    pseudo_header[8] = 0;                    // Zero byte - Fixed Byte
    pseudo_header[9] = PROTOCOL_IP_TCP;      // Protocol (TCP)
    pseudo_header[10] = (u8)(tcp_len >> 8);  // TCP Length (high byte)
    pseudo_header[11] = (u8)(tcp_len & 0xFF); // TCP Length (low byte)
}


// Function to fill in the common TCP header fields
void build_tcp_header(tcp_packet *tx_tcp, u8 *src_ip, u8 *dst_ip, u16 ipv4_total_length ,u16 src_port, u16 dest_port,
                      uint32_t seq_num, uint32_t ack_num, uint8_t flags, u16 window_size, u8 *options, u8 option_len) {
    memset(tx_tcp, 0, sizeof(tcp_packet));

    // Ethernet header
    memcpy(tx_tcp->ipv4.ethernet.dest_mac, dst_MAC, 6);
    memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
    tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);

    // IPv4 header
    tx_tcp->ipv4.version = 4 << 4 | 5;
    tx_tcp->ipv4.ttl = 64;
    tx_tcp->ipv4.protocol = PROTOCOL_IP_TCP;
    memcpy(tx_tcp->ipv4.src_ip, src_ip, 4);
    memcpy(tx_tcp->ipv4.dest_ip, dst_ip, 4);

    tx_tcp->ipv4.total_length = swap16(ipv4_total_length + option_len);  // IP + TCP + options
    tx_tcp->ipv4.checksum = 0;
    tx_tcp->ipv4.checksum = checksum((u8*)&tx_tcp->ipv4 + sizeof(eth_header), sizeof(ipv4_packet) - sizeof(eth_header));

    // TCP header
    tx_tcp->src_port = src_port;
    tx_tcp->dest_port = dest_port;
    tx_tcp->seqnum = swap32(seq_num);
    tx_tcp->acknum = swap32(ack_num + 1);
    tx_tcp->data_offset = (5 + (option_len/4)) << 4; // Data offset (header length)
    tx_tcp->flags = flags;
    tx_tcp->window = swap16(window_size);
    tx_tcp->checksum = 0; // Will calculate after adding options
    tx_tcp->urg_pointer = 0;

    // Copy options if any
    if (option_len > 0) {
        memcpy((u8*)tx_tcp + sizeof(tcp_packet), options, option_len);
    }
}

//Construction and Sending of SYN packet to send connection request to server
// Function to handle the SYN packet construction and sending
void send_syn_packet(u8 *tosend, u8 *src_ip, u8 *dst_ip, u16 dest_port) {
    u8 tcp_options[20];
    u8 option_len = 0;

    // Define TCP options (these could be combined or extended based on need)
    u8 mss_option[] = {0x02, 0x04, 0x05, 0xb4};  // MSS: 1460 bytes
    u8 sack_permitted[] = {0x04, 0x02};          // SACK Permitted
    u8 timestamp_option[] = {0x08, 0x0A, 0x22, 0x45, 0x0E, 0x56, 0x00, 0x00, 0x00, 0x00};  // Timestamps
    u8 nop_option[] = {0x01};                     // NOP
    u8 window_scale_option[] = {0x03, 0x03, 0x07}; // Window Scale: 7

    // Concatenate options
    memcpy(tcp_options + option_len, mss_option, sizeof(mss_option)); option_len += sizeof(mss_option);
    memcpy(tcp_options + option_len, sack_permitted, sizeof(sack_permitted)); option_len += sizeof(sack_permitted);
    memcpy(tcp_options + option_len, timestamp_option, sizeof(timestamp_option)); option_len += sizeof(timestamp_option);
    memcpy(tcp_options + option_len, nop_option, sizeof(nop_option)); option_len += sizeof(nop_option);
    memcpy(tcp_options + option_len, window_scale_option, sizeof(window_scale_option)); option_len += sizeof(window_scale_option);

    // TCP packet
    tcp_packet* tx_tcp = (tcp_packet*)tosend;
    build_tcp_header(tx_tcp, src_ip, dst_ip, sizeof(tcp_packet) - sizeof(eth_header), swap16(12345), swap16(dest_port), 0, 0, TCP_SYN, 520, tcp_options, option_len);

    // Pseudo-header for checksum calculation
    char pseudo_header[12];
    create_pseudo_header(pseudo_header, src_ip, dst_ip, sizeof(tcp_packet) + option_len - sizeof(ipv4_packet));

    // Calculate checksum
    tx_tcp->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + checksum((u8*)tx_tcp + sizeof(ipv4_packet), sizeof(tcp_packet) + option_len - sizeof(ipv4_packet));

    // Send SYN packet
    net_send(tosend, sizeof(tcp_packet) + option_len);
    b_output("SYN packet sent.\n", (unsigned long)strlen("SYN packet sent.\n"));
}

// Function to convert u64 to a string (manual integer to string conversion)
void u64_to_str(u64 value, char* str) {
    int i = 0;
    if (value == 0) {
        str[i++] = '0';
    } else {
        while (value > 0) {
            str[i++] = '0' + (value % 10);
            value /= 10;
        }
        for (int j = 0; j < i / 2; j++) {
            char temp = str[j];
            str[j] = str[i - j - 1];
            str[i - j - 1] = temp;
        }
    }
    str[i] = '\0';
}

int main(){
    b_output("\nminIP Client2.0\n", (unsigned long)strlen("\nminIP Client2.0\n"));

    u64 time = b_system(TIMECOUNTER, 0, 0);

    char timeStr[32]; 
    u64_to_str(time, timeStr); 

    b_output(timeStr, (unsigned long)strlen(timeStr)); 
    b_output("Desired Time\n", (unsigned long)strlen("Desired Time\n")); 

    net_init();

    u16 dest_port = 8129;
    u8 server_ip[4] = {10, 13, 100, 131};
    u8 broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    // Send initial ARP request
    send_arp_request(server_ip, broadcast_mac, src_MAC, src_IP, tosend);
    b_output("ARP Request Sent. Waiting for Reply\n", (unsigned long)strlen("ARP Request Sent. Waiting for Reply\n"));

    // Receive ARP reply
    while (1) {
        recv_packet_len = net_recv(buffer);
        eth_header* rx_eth = (eth_header*)buffer;

        if (recv_packet_len > 0 && swap16(rx_eth->type) == ETHERTYPE_ARP) {
            arp_packet* rx_arp = (arp_packet*)buffer;
            if (swap16(rx_arp->opcode) == ARP_REPLY) {
                if (memcmp(server_ip, rx_arp->sender_ip, 4) == 0) {
                    b_output("ARP Packet Received\n", (unsigned long)strlen("ARP Packet Received\n"));
                    memcpy(dst_MAC, rx_arp->sender_mac, 6);
                    b_output("Server MAC Address obtained.\n", (unsigned long)strlen("Server MAC Address obtained.\n"));
                    break;
                } else {
                    // Send ARP request again if the response is not from the correct server
                    send_arp_request(server_ip, broadcast_mac, src_MAC, src_IP, tosend);
                }
            }
        }
    }

    // Sending SYN Packet
    send_syn_packet(tosend,src_IP,dst_IP,dest_port);
   
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
                        // Building TCP Header for SYN-ACK 
                        build_tcp_header(tx_ack, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header), rx_tcp->dest_port, rx_tcp->src_port,
                                     swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum), TCP_ACK, 512, 0, 0);
                        char pseudo_header[12];
                        create_pseudo_header(pseudo_header, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet)-sizeof(ipv4_packet));
                        tx_ack->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + 
                                        checksum((u8*)tx_ack + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        //Send ACK packet
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Connection Established\n", (unsigned long)strlen("Connection Established\n"));

                        // b_system(DELAY, 10000000, 0);

                        //Building the Message Packet and Sending to server
                        char* input = "Hello from the client";
                        int payload_len = strlen(input);
                        tcp_packet* tx_psh = (tcp_packet*)tosend;
                        build_tcp_header(tx_psh, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header) + payload_len,
                                     rx_tcp->dest_port, rx_tcp->src_port, swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum), TCP_PSH | TCP_ACK, 512, 0, 0);
                        char pseudo[12];
                        create_pseudo_header(pseudo, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len);
                        tx_psh->checksum = checksum(pseudo, sizeof(pseudo)) +
                                        checksum((u8*)tx_psh + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len);
                        memcpy((u8*)tx_psh + sizeof(tcp_packet), input, payload_len);
                        // Send the packet
                        net_send(tosend, sizeof(tcp_packet) + payload_len);
                        b_output("Message Sent\n", (unsigned long)strlen("Message Sent\n"));

                        // b_system(DELAY, 10000000, 0);

                        //Building the Message Packet and Sending to server
                        // char* input1 = "Hello from the client";
                        // int payload_len1 = strlen(input1);
                        // tcp_packet* tx_psh1 = (tcp_packet*)tosend;
                        // build_tcp_header(tx_psh1, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header) + payload_len1,
                        //              rx_tcp->dest_port, rx_tcp->src_port, swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum), TCP_PSH | TCP_ACK, 512, 0, 0);
                        // char pseudo1[12];
                        // create_pseudo_header(pseudo1, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len1);
                        // tx_psh1->checksum = checksum(pseudo1, sizeof(pseudo1)) +
                        //                 checksum((u8*)tx_psh1 + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len1);
                        // memcpy((u8*)tx_psh1 + sizeof(tcp_packet), input1, payload_len1);
                        // // Send the packet
                        // net_send(tosend, sizeof(tcp_packet) + payload_len1);
                        // b_output("Second Message Sent\n", (unsigned long)strlen("Second Message Sent\n"));
                    }else if((rx_tcp->flags & (TCP_PSH | TCP_ACK)) == (TCP_PSH | TCP_ACK)){
                        //Building ACK Packet for Message Packet for Server
                        tcp_packet* tx_ack = (tcp_packet*)tosend;
                        build_tcp_header(tx_ack, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header), rx_tcp->dest_port, rx_tcp->src_port,
                                     swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum) + (swap16(rx_tcp->ipv4.total_length) - 52) - 1, TCP_ACK, 512, 0, 0);    
                        char pseudo_header[12];
                        create_pseudo_header(pseudo_header, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet));
                        tx_ack->checksum = checksum(pseudo_header, sizeof(pseudo_header)) + 
                                        checksum((u8*)tx_ack + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        //Send the Packet
                        net_send(tosend, sizeof(tcp_packet));
                        b_output("Ack Sent\n", (unsigned long)strlen("Ack Sent\n"));

                        //Printing Message from Server
                        char* message = (char*)rx_tcp + sizeof(tcp_packet) + 12;
						b_output(message,(unsigned long)strlen(message));

                        //Building the Message Packet and Sending to server
                        char* input1 = "Hello from the client";
                        int payload_len1 = strlen(input1);
                        tcp_packet* tx_psh1 = (tcp_packet*)tosend;
                        build_tcp_header(tx_psh1, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header) + payload_len1,
                                     rx_tcp->dest_port, rx_tcp->src_port, swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum) + (swap16(rx_tcp->ipv4.total_length) - 52) - 1, TCP_PSH | TCP_ACK, 512, 0, 0);
                        char pseudo1[12];
                        create_pseudo_header(pseudo1, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len1);
                        tx_psh1->checksum = checksum(pseudo1, sizeof(pseudo1)) +
                                        checksum((u8*)tx_psh1 + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet) + payload_len1);
                        memcpy((u8*)tx_psh1 + sizeof(tcp_packet), input1, payload_len1);
                        // Send the packet
                        net_send(tosend, sizeof(tcp_packet) + payload_len1);
                        b_output("Message Sent\n", (unsigned long)strlen("Message Sent\n"));

                        //Building FIN packet for terminating Connection with Server
                        // tcp_packet* tx_fin = (tcp_packet*)tosend;
                        // build_tcp_header(tx_ack, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header), rx_tcp->dest_port, rx_tcp->src_port,
                        //              swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum) + (swap16(rx_tcp->ipv4.total_length) - 52) - 1, TCP_FIN, 512, 0, 0);
                        // char pseudo[12];
                        // create_pseudo_header(pseudo_header, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet));
                        // tx_fin->checksum = checksum(pseudo, sizeof(pseudo)) +
                        //                 checksum((u8*)tx_fin + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        // // Send the ACK packet
                        // net_send(tosend, sizeof(tcp_packet));
                        // b_output("Connection Break Request.\n", (unsigned long)strlen("Connection Break Request.\n"));
                    }else if((rx_tcp->flags & (TCP_FIN | TCP_ACK)) == (TCP_FIN | TCP_ACK)){
                        //Building ACK packet for FIN-ACK by Server
                        tcp_packet* tx_fin = (tcp_packet*)tosend;
                        build_tcp_header(tx_fin, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(eth_header), rx_tcp->dest_port, rx_tcp->src_port,
                                     swap32(rx_tcp->acknum), swap32(rx_tcp->seqnum) + (swap16(rx_tcp->ipv4.total_length) - 52) - 1, TCP_ACK, 512, 0, 0);
                        char pseudo_header[12];
                        create_pseudo_header(pseudo_header, rx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, sizeof(tcp_packet) - sizeof(ipv4_packet));
                        tx_fin->checksum = checksum(pseudo_header, sizeof(pseudo_header)) +
                                        checksum((u8*)tx_fin + sizeof(ipv4_packet), sizeof(tcp_packet) - sizeof(ipv4_packet));
                        //Send ACK packet
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