// /* minIP */
// /* Written by Ian Seyler */

// #define __USE_MISC

// /* Global Includes */
// #include "libBareMetal.h"

// /* Global functions */
// u16 checksum(u8* data, u16 bytes);
// u16 checksum_tcp(u8* data, u16 bytes, u16 protocol, u16 length);
// int net_init();
// int net_exit();
// int net_send(unsigned char* data, unsigned int bytes);
// int net_recv(unsigned char* data);
// u16 swap16(u16 in);
// u32 swap32(u32 in);
// void* memset(void* s, int c, int n);
// void* memcpy(void* d, const void* s, int n);
// int strlen(const char* s);


// /* Global defines */
// #undef ETH_FRAME_LEN
// #define ETH_FRAME_LEN 1518
// #define ETHERTYPE_ARP 0x0806
// #define ETHERTYPE_IPv4 0x0800
// #define ETHERTYPE_IPv6 0x86DD
// #define ARP_REQUEST 1
// #define ARP_REPLY 2
// #define PROTOCOL_IP_ICMP 1
// #define PROTOCOL_IP_TCP 6
// #define PROTOCOL_IP_UDP 11
// #define ICMP_ECHO_REPLY 0
// #define ICMP_ECHO_REQUEST 8
// #define TCP_ACK 16
// #define TCP_PSH 8
// #define TCP_RST 4
// #define TCP_SYN 2
// #define TCP_FIN 1

// /* Global variables */
// u8 src_MAC[6] = {0x08, 0x00, 0x27, 0x2D, 0xDE, 0x1F};
// u8 dst_MAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// u8 dst_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
// u8 src_IP[4] = {0, 0, 0, 0};
// u8 src_SN[4] = {0, 0, 0, 0};
// u8 src_GW[4] = {0, 0, 0, 0};
// u8 dst_IP[4] = {0, 0, 0, 0};
// unsigned char buffer[ETH_FRAME_LEN];
// unsigned char tosend[ETH_FRAME_LEN];
// int s; // Socket variable
// int running = 1, c, recv_packet_len;
// unsigned int tint, tint0, tint1, tint2, tint3;

// /* Global structs */
// #pragma pack(1)
// typedef struct eth_header {
// 	u8 dest_mac[6];
// 	u8 src_mac[6];
// 	u16 type;
// } eth_header; // 14 bytes
// typedef struct arp_packet {
// 	eth_header ethernet;
// 	u16 hardware_type;
// 	u16 protocol;
// 	u8 hardware_size;
// 	u8 protocol_size;
// 	u16 opcode;
// 	u8 sender_mac[6];
// 	u8 sender_ip[4];
// 	u8 target_mac[6];
// 	u8 target_ip[4];
// } arp_packet; // 28 bytes
// typedef struct ipv4_packet {
// 	eth_header ethernet;
// 	u8 version;
// 	u8 dsf;
// 	u16 total_length;
// 	u16 id;
// 	u16 flags;
// 	u8 ttl;
// 	u8 protocol;
// 	u16 checksum;
// 	u8 src_ip[4];
// 	u8 dest_ip[4];
// } ipv4_packet; // 20 bytes since we don't support options
// typedef struct icmp_packet {
// 	ipv4_packet ipv4;
// 	u8 type;
// 	u8 code;
// 	u16 checksum;
// 	u16 id;
// 	u16 sequence;
// 	u64 timestamp;
// 	u8 data[2]; // Set to 2 so can be used as pointer
// } icmp_packet;
// typedef struct udp_packet {
// 	ipv4_packet ipv4;
// 	u16 src_port;
// 	u16 dest_port;
// 	u16 length;
// 	u16 checksum;
// 	u8 data[2]; // Set to 2 so can be used as pointer
// } udp_packet;
// typedef struct tcp_packet {
// 	ipv4_packet ipv4;
// 	u16 src_port;
// 	u16 dest_port;
// 	u32 seqnum;
// 	u32 acknum;
// 	u8 data_offset;
// 	u8 flags;
// 	u16 window;
// 	u16 checksum;
// 	u16 urg_pointer;
// 	// Options and data
// //	u8 data[2]; // Set to 2 so can be used as pointer
// } tcp_packet;

// /* Default HTTP page with HTTP headers */
// const char webpage[] =
// "HTTP/1.0 200 OK\n"
// "Server: BareMetal (http://www.returninfinity.com)\n"
// "Content-type: text/html\n"
// "\n"
// "<!DOCTYPE html>\n"
// "<html>\n"
// "\t<head>\n"
// "\t\t<title>minIP</title>\n"
// "\t\t<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css\">\n"
// "\t\t<link href='https://fonts.googleapis.com/css?family=Roboto' rel='stylesheet' type='text/css'>\n"
// "\t\t<style>\n"
// "\t\t\tbody, h1, h2, h3, h4, h5, h6, .h1, .h2, .h3, .h4, .h5, .h6 {\n"
// "\t\t\t\tfont-family: \"Avenir Next\", \"Roboto\", sans-serif;\n"
// "\t\t\t\tline-height: 1.5;\n"
// "\t\t\t}\n"
// "\t\t</style>\n"
// "\t</head>\n"
// "\t<body>\n"
// "\t\t<div class=\"container\">\n"
// "\t\t\t<p />\n"
// "\t\t\t<h1>Hello, from minIP!</h1>\n"
// "\t\t\t<h3><a href=\"https://github.com/IanSeyler/minIP/\">minIP on GitHub</a></h3>\n"
// "\t\t\t<p>minIP is a very tiny TCP/IP stack implementation in C.</p>\n"
// "\t\t\t<p>It cointains just enough code to serve this webpage.</p>\n"
// "\t\t</div>\n"
// "\t</body>\n"
// "</html>\n";
// const char version_string[] = "\nminIP v0.7.0 (2023 11 11)\nListening on 10.13.100.50...\n";
// const char arp[] = "arp\n";
// const char ipv4[] = "ipv4\n";
// const char ping[] = "ping\n";

// /* Main code */
// int main()
// {
// 	b_output(version_string, (unsigned long)strlen(version_string));
// 	src_IP[0] = 10;
// 	src_IP[1] = 13;
// 	src_IP[2] = 100;
// 	src_IP[3] = 50;
// 	src_SN[0] = 255;
// 	src_SN[1] = 255;
// 	src_SN[2] = 255;
// 	src_SN[3] = 0;
// 	src_GW[0] = 10;
// 	src_GW[1] = 13;
// 	src_GW[2] = 100;
// 	src_GW[3] = 255;
// 	net_init();

// 	while(running == 1)
// 	{
// 		recv_packet_len = net_recv(buffer);
// 		eth_header* rx = (eth_header*)buffer;

// 		if (recv_packet_len > 0) // Make sure we received a packet
// 		{
// 			memset(tosend, 0, ETH_FRAME_LEN); // clear the send buffer
// 			if (swap16(rx->type) == ETHERTYPE_ARP)
// 			{
// 				arp_packet* rx_arp = (arp_packet*)buffer;
// 				if (swap16(rx_arp->opcode) == ARP_REQUEST)
// 				{
// 					// b_output(arp, (unsigned long)strlen(arp));
// 					if (*(u32*)rx_arp->target_ip == *(u32*)src_IP)
// 					{
// 						arp_packet* tx_arp = (arp_packet*)tosend;
// 						// Ethernet
// 						memcpy(tx_arp->ethernet.dest_mac, rx_arp->sender_mac, 6);
// 						memcpy(tx_arp->ethernet.src_mac, src_MAC, 6);
// 						tx_arp->ethernet.type = swap16(ETHERTYPE_ARP);
// 						// ARP
// 						tx_arp->hardware_type = swap16(1); // Ethernet
// 						tx_arp->protocol = swap16(ETHERTYPE_IPv4);
// 						tx_arp->hardware_size = 6;
// 						tx_arp->protocol_size = 4;
// 						tx_arp->opcode = swap16(ARP_REPLY);
// 						memcpy(tx_arp->sender_mac, src_MAC, 6);
// 						memcpy(tx_arp->sender_ip, rx_arp->target_ip, 4);
// 						memcpy(tx_arp->target_mac, rx_arp->sender_mac, 6);
// 						memcpy(tx_arp->target_ip, rx_arp->sender_ip, 4);
// 						// Send the reply
// 						net_send(tosend, 42);
// 					}
// 				}
// 				else if (buffer[21] == ARP_REPLY)
// 				{
// 					// TODO - Responses to our requests
// 				}
// 			}
// 			else if (swap16(rx->type) == ETHERTYPE_IPv4)
// 			{
// 				ipv4_packet* rx_ipv4 = (ipv4_packet*)buffer;
// 				if(rx_ipv4->protocol == PROTOCOL_IP_ICMP)
// 				{
// 					b_output(ping, (unsigned long)strlen(ping));
// 					icmp_packet* rx_icmp = (icmp_packet*)buffer;
// 					if(rx_icmp->type == ICMP_ECHO_REQUEST)
// 					{
// 						if (*(u32*)rx_icmp->ipv4.dest_ip == *(u32*)src_IP)
// 						{
// 							b_output(ping, (unsigned long)strlen(ping));
// 							// Reply to the ping request
// 							icmp_packet* tx_icmp = (icmp_packet*)tosend;
// 							// Ethernet
// 							memcpy(tx_icmp->ipv4.ethernet.dest_mac, rx_icmp->ipv4.ethernet.src_mac, 6);
// 							memcpy(tx_icmp->ipv4.ethernet.src_mac, src_MAC, 6);
// 							tx_icmp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
// 							// IPv4
// 							tx_icmp->ipv4.version = rx_icmp->ipv4.version;
// 							tx_icmp->ipv4.dsf = rx_icmp->ipv4.dsf;
// 							tx_icmp->ipv4.total_length = rx_icmp->ipv4.total_length;
// 							tx_icmp->ipv4.id = rx_icmp->ipv4.id;
// 							tx_icmp->ipv4.flags = rx_icmp->ipv4.flags;
// 							tx_icmp->ipv4.ttl = rx_icmp->ipv4.ttl;
// 							tx_icmp->ipv4.protocol = rx_icmp->ipv4.protocol;
// 							tx_icmp->ipv4.checksum = rx_icmp->ipv4.checksum; // No need to recalculate checksum
// 							memcpy(tx_icmp->ipv4.src_ip, rx_icmp->ipv4.dest_ip, 4);
// 							memcpy(tx_icmp->ipv4.dest_ip, rx_icmp->ipv4.src_ip, 4);
// 							// ICMP
// 							tx_icmp->type = ICMP_ECHO_REPLY;
// 							tx_icmp->code = rx_icmp->code;
// 							tx_icmp->checksum = 0;
// 							tx_icmp->id = rx_icmp->id;
// 							tx_icmp->sequence = rx_icmp->sequence;
// 							tx_icmp->timestamp = rx_icmp->timestamp;
// 							memcpy (tx_icmp->data, rx_icmp->data, (swap16(rx_icmp->ipv4.total_length)-20-16)); // IP length - IPv4 header - ICMP header
// 							tx_icmp->checksum = checksum(&tosend[34], recv_packet_len-14-20); // Frame length - MAC header - IPv4 header
// 							// Send the reply
// 							net_send(tosend, recv_packet_len);
// 						}
// 					}
// 					else if (rx_icmp->type == ICMP_ECHO_REPLY)
// 					{
// 			//			printf("Reply");
// 					}
// 					else
// 					{
// 			//			printf("Unknown ICMP packet");
// 					}
// 				}
// 				else if(rx_ipv4->protocol == PROTOCOL_IP_TCP)
// 				{
// //					printf("TCP");
// 					b_output(ipv4, (unsigned long)strlen(ipv4));
// 					tcp_packet* rx_tcp = (tcp_packet*)buffer;
// 					if (rx_tcp->flags == TCP_SYN)
// 					{
// //						printf(" - SYN");
// 						b_output("SYN\n", (unsigned long)strlen("SYN\n"));
// 						// b_output(rx_tcp->ipv4.src_ip, (unsigned long)strlen(rx_tcp->ipv4.src_ip));
// 						tcp_packet* tx_tcp = (tcp_packet*)tosend;
// 						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
// 						// Ethernet
// 						memcpy(tx_tcp->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
// 						memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
// 						tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
// 						// IPv4
// 						tx_tcp->ipv4.version = rx_tcp->ipv4.version;
// 						tx_tcp->ipv4.dsf = rx_tcp->ipv4.dsf;
// 						tx_tcp->ipv4.total_length = rx_tcp->ipv4.total_length;
// 						tx_tcp->ipv4.id = rx_tcp->ipv4.id;
// 						tx_tcp->ipv4.flags = rx_tcp->ipv4.flags;
// 						tx_tcp->ipv4.ttl = rx_tcp->ipv4.ttl;
// 						tx_tcp->ipv4.protocol = rx_tcp->ipv4.protocol;
// 						tx_tcp->ipv4.checksum = 0;
// 						memcpy(tx_tcp->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
// 						memcpy(tx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
// 						tx_tcp->ipv4.checksum = checksum(&tosend[14], 20);
// 						// TCP
// 						tx_tcp->src_port = rx_tcp->dest_port;
// 						tx_tcp->dest_port = rx_tcp->src_port;
// 						tx_tcp->seqnum = rx_tcp->seqnum;
// 						tx_tcp->acknum = swap32(swap32(rx_tcp->seqnum)+1);
// 						tx_tcp->data_offset = rx_tcp->data_offset;
// 						tx_tcp->flags = TCP_SYN|TCP_ACK;
// 						tx_tcp->window = rx_tcp->window;
// 						tx_tcp->checksum = 0;
// 						tx_tcp->urg_pointer = rx_tcp->urg_pointer;
// 						tx_tcp->checksum = checksum_tcp(&tosend[34], recv_packet_len-34, PROTOCOL_IP_TCP, recv_packet_len-34);
// 						// Send the reply
// 						net_send(tosend, recv_packet_len);
// 					}
// 					else if (rx_tcp->flags == TCP_ACK)
// 					{
// 						b_output("ACK Flag\n",(unsigned long)strlen("ACK Flag\n"));
// //						printf(" - ACK");
// 						// Ignore these for now.
// 					}
// 					else if (rx_tcp->flags == (TCP_PSH|TCP_ACK))
// 					{
// //						printf(" - PSH");
// 						b_output("PSH\n", (unsigned long)strlen("PSH\n"));
					
// 						char* message = (char*)rx_tcp + sizeof(tcp_packet);
// 						b_output(message,(unsigned long)strlen(message));

// 						tcp_packet* tx_tcp = (tcp_packet*)tosend;
// 						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
// 						// Ethernet
// 						memcpy(tx_tcp->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
// 						memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
// 						tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
// 						// IPv4
// 						tx_tcp->ipv4.version = rx_tcp->ipv4.version;
// 						tx_tcp->ipv4.dsf = rx_tcp->ipv4.dsf;
// 						tx_tcp->ipv4.total_length = rx_tcp->ipv4.total_length;
// 						tx_tcp->ipv4.id = rx_tcp->ipv4.id;
// 						tx_tcp->ipv4.flags = rx_tcp->ipv4.flags;
// 						tx_tcp->ipv4.ttl = rx_tcp->ipv4.ttl;
// 						tx_tcp->ipv4.protocol = rx_tcp->ipv4.protocol;
// 						tx_tcp->ipv4.checksum = 0;
// 						memcpy(tx_tcp->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
// 						memcpy(tx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
// 						tx_tcp->ipv4.checksum = checksum(&tosend[14], 20);
// 						// TCP
// 						tx_tcp->src_port = rx_tcp->dest_port;
// 						tx_tcp->dest_port = rx_tcp->src_port;
// 						tx_tcp->seqnum = rx_tcp->seqnum;
// 						tx_tcp->acknum = swap32(swap32(rx_tcp->seqnum)+(recv_packet_len-14-20-32)); // Add the bytes received
// 						tx_tcp->data_offset = rx_tcp->data_offset;
// 						// tx_tcp->flags = TCP_ACK;
// 						tx_tcp->window = rx_tcp->window;
// 						tx_tcp->checksum = 0;
// 						tx_tcp->urg_pointer = rx_tcp->urg_pointer;
// 						tx_tcp->checksum = checksum_tcp(&tosend[34], 32, PROTOCOL_IP_TCP, 32);
// 						// Send the reply
// 						// net_send(tosend, 66);
// 						// Send the webpage
// 						// tx_tcp->ipv4.total_length = swap16(52+strlen(webpage));
// 						tx_tcp->ipv4.checksum = 0;
// 						tx_tcp->ipv4.checksum = checksum(&tosend[14], 20);
// 						tx_tcp->flags = TCP_PSH|TCP_ACK;
// 						tx_tcp->checksum = 0;
// 						memcpy((char*)tosend+66, (char*)webpage, strlen(webpage));
// 						tx_tcp->checksum = checksum_tcp(&tosend[34], 32+strlen(webpage), PROTOCOL_IP_TCP, 32+strlen(webpage));
// 						net_send(tosend, 66+strlen(webpage));
// 						// Disconnect the client
// 						// tx_tcp->ipv4.total_length = swap16(52);
// 						// tx_tcp->ipv4.checksum = 0;
// 						// tx_tcp->ipv4.checksum = checksum(&tosend[14], 20);
// 						// tx_tcp->seqnum = swap32(swap32(tx_tcp->seqnum)+strlen(webpage));
// 						// tx_tcp->flags = TCP_FIN|TCP_ACK;
// 						// tx_tcp->checksum = 0;
// 						// tx_tcp->checksum = checksum_tcp(&tosend[34], 32, PROTOCOL_IP_TCP, 32);
// 						// net_send(tosend, 66);
// 					}
// 					else if (rx_tcp->flags == (TCP_FIN|TCP_ACK))
// 					{
// //						printf(" - FIN");
// 						b_output("FIN\n", (unsigned long)strlen("FIN\n"));
// 						tcp_packet* tx_tcp = (tcp_packet*)tosend;
// 						memcpy((void*)tosend, (void*)buffer, ETH_FRAME_LEN); // make a copy of the original frame
// 						// Ethernet
// 						memcpy(tx_tcp->ipv4.ethernet.dest_mac, rx_tcp->ipv4.ethernet.src_mac, 6);
// 						memcpy(tx_tcp->ipv4.ethernet.src_mac, src_MAC, 6);
// 						tx_tcp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
// 						// IPv4
// 						tx_tcp->ipv4.version = rx_tcp->ipv4.version;
// 						tx_tcp->ipv4.dsf = rx_tcp->ipv4.dsf;
// 						tx_tcp->ipv4.total_length = swap16(52);
// 						tx_tcp->ipv4.id = rx_tcp->ipv4.id;
// 						tx_tcp->ipv4.flags = rx_tcp->ipv4.flags;
// 						tx_tcp->ipv4.ttl = rx_tcp->ipv4.ttl;
// 						tx_tcp->ipv4.protocol = rx_tcp->ipv4.protocol;
// 						tx_tcp->ipv4.checksum = 0;
// 						memcpy(tx_tcp->ipv4.src_ip, rx_tcp->ipv4.dest_ip, 4);
// 						memcpy(tx_tcp->ipv4.dest_ip, rx_tcp->ipv4.src_ip, 4);
// 						tx_tcp->ipv4.checksum = checksum(&tosend[14], 20);
// 						// TCP
// 						tx_tcp->src_port = rx_tcp->dest_port;
// 						tx_tcp->dest_port = rx_tcp->src_port;
// 						tx_tcp->seqnum = rx_tcp->acknum;
// 						tx_tcp->acknum = swap32(swap32(rx_tcp->seqnum)+1);
// 						tx_tcp->data_offset = rx_tcp->data_offset;
// 						tx_tcp->flags = TCP_ACK;
// 						tx_tcp->window = rx_tcp->window;
// 						tx_tcp->checksum = 0;
// 						tx_tcp->urg_pointer = rx_tcp->urg_pointer;
// 						tx_tcp->checksum = checksum_tcp(&tosend[34], 32, PROTOCOL_IP_TCP, 32);
// 						// Send the reply
// 						net_send(tosend, 66);
// 					}
// //					printf("\n");
// 				}
// 				else if (rx_ipv4->protocol == PROTOCOL_IP_UDP)
// 				{
// 					// TODO - UDP
// 				}
// 				else
// 				{
// 			//		printf("Unknown protocol");
// 				}
// 			}
// 			else if (swap16(rx->type) == ETHERTYPE_IPv6)
// 			{
// 				// TODO - IPv6
// 			}
// 		}
// 	}

// 	b_output("\n", 1);
// 	net_exit();
// 	return 0;
// }


// /* checksum - Calculate a checksum value */
// // Returns 16-bit checksum
// u16 checksum(u8* data, u16 bytes)
// {
// 	u32 sum = 0;
// 	u16 i;

// 	for (i=0; i<bytes-1; i+=2) // Add up the words
// 		sum += *(u16 *) &data[i];

// 	if (bytes & 1) // Add the left-over byte if there is one
// 		sum += (u8) data[i];

// 	while (sum >> 16) // Fold total to 16-bits
// 		sum = (sum & 0xFFFF) + (sum >> 16);

// 	return ~sum; // Return 1's complement
// }


// /* checksum_tcp - Calculate a TCP checksum value */
// // Returns 16-bit checksum
// u16 checksum_tcp(u8* data, u16 bytes, u16 protocol, u16 length)
// {
// 	u32 sum = 0;
// 	u16 i;
// 	data -= 8; // Start at the source and dest IPs
// 	bytes += 8;

// 	for (i=0; i<bytes-1; i+=2) // Add up the words
// 		sum += *(u16 *) &data[i];

// 	if (bytes & 1) // Add the left-over byte if there is one
// 		sum += (u8) data[i];

// 	sum += swap16(protocol);
// 	sum += swap16(length);

// 	while (sum >> 16) // Fold total to 16-bits
// 		sum = (sum & 0xFFFF) + (sum >> 16);

// 	return ~sum; // Return 1's complement
// }


// /* net_init - Initialize a raw socket */
// int net_init()
// {
// 	u64 val0 = 0, val1 = 0, val2 = 0;
// 	val0 = b_system(MAC_GET, val1, val2); // Get the MAC as a u64
// 	char * MAC = (void*)&val0;

// 	src_MAC[0] = MAC[5]; // Get the MAC bytes in the correct order
// 	src_MAC[1] = MAC[4];
// 	src_MAC[2] = MAC[3];
// 	src_MAC[3] = MAC[2];
// 	src_MAC[4] = MAC[1];
// 	src_MAC[5] = MAC[0];

// 	return 0;
// }


// /* net_exit - Clean up and exit */
// int net_exit()
// {
// 	return 0;
// }

// /* net_send - Send a raw Ethernet packet */
// // Wrapper for OS send function
// // Returns number of bytes sent
// int net_send(unsigned char* data, unsigned int bytes)
// {
// 	b_net_tx(data, bytes, 0);
// }


// /* net_recv - Receive a raw Ethernet packet */
// // Wrapper for OS recv function
// // Returns number of bytes read
// int net_recv(unsigned char* data)
// {
// 	return b_net_rx(data, 0);
// }


// /* swap16 - Change endianness on a 16-bit value */
// // x86-64 uses little-endian while IP uses big-endian
// u16 swap16(u16 in)
// {
// 	u16 out = in<<8 | ((in&0xff00)>>8);
// 	return out;
// }


// /* swap32 - Change endianness on a 32-bit value */
// // x86-64 uses little-endian while IP uses big-endian
// u32 swap32(u32 in)
// {
// 	u32 out = in<<24 | ((in&0xff00)<<8) | ((in&0xff0000)>>8) | ((in&0xff000000)>>24);
// 	return out;
// }

// void* memset(void* s, int c, int n)
// {
// 	char* _src;

// 	_src = (char*)s;

// 	while (n--) {
// 		*_src++ = c;
// 	}

// 	return s;
// }

// void* memcpy(void* d, const void* s, int n)
// {
// 	char* dest;
// 	char* src;

// 	dest = (char*)d;
// 	src = (char*)s;

// 	while (n--) {
// 		*dest++ = *src++;
// 	}

// 	return d;
// }

// int strlen(const char* s)
// {
// 	int r = 0;

// 	for(; *s++ != 0; r++) { }

// 	return r;
// }

// /* EOF */
// #include "libBareMetal.h"


// int strlen(const char* s)
// {
// 	int r = 0;

// 	for(; *s++ != 0; r++) { }

// 	return r;
// }


// void write_to_storage(void *mem, u64 start, u64 num, u64 drivenum) {
//     u64 result = b_storage_write(mem, start, num, drivenum);
//     if (result == 512) {
//         b_output("\nWrite operation successful.\n", (unsigned long)strlen("\nWrite operation successful.\n"));
//     } else {
//         b_output("\nWrite operation failed.\n", (unsigned long)strlen("\nWrite operation failed.\n"));
//     }
// }

// void read_from_storage(void *mem, u64 start, u64 num, u64 drivenum) {
//     u64 result = b_storage_read(mem, start, num, drivenum);
//     if (result == num) {
//         b_output("Read operation successful.\n", (unsigned long)strlen("Read operation successful.\n"));
//     } else {
//         b_output("Read operation failed.\n", (unsigned long)strlen("Read operation failed.\n"));
//     }
// }

// int main() {
//     char buffer[512];

//     char data_to_write[] = "Hello, BareMetal OS Storage!";

//     for (int i = 0; i < sizeof(data_to_write); i++) {
//         buffer[i] = data_to_write[i];
//     }

//     write_to_storage(buffer, 0, 512, 0);
    
//     for (int i = 0; i < sizeof(buffer); i++) {
//         buffer[i] = 0;
//     }

//     read_from_storage(buffer, 0, 512, 0);

//     b_output("Data read from storage: ", (unsigned long)strlen("Data read from storage: "));
//     b_output(buffer, sizeof(data_to_write));  

//     return 0;
// }
#include "libBareMetal.h"

#define AHCI_PI            0x0C // Ports Implemented
#define AHCI_PxSSTS        0x28 // Port x Serial ATA Status

u8 buffer[512];

// Utility Functions
int strlen(const char* s) {
    int r = 0;
    for (; *s++ != 0; r++) {}
    return r;
}

void itoa(unsigned int value, char* str, int base) {
    char* ptr = str;
    char* end = str;
    char digits[] = "0123456789ABCDEF";

    do {
        *end++ = digits[value % base];
        value /= base;
    } while (value);

    *end-- = '\0';

    while (ptr < end) {
        char temp = *ptr;
        *ptr++ = *end;
        *end-- = temp;
    }
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

unsigned int mmio_read32(unsigned long address) {
    return *(volatile unsigned int*)address;
}

unsigned int find_ahci_base_address() {
    unsigned int base_address = b_system(AHCI_BASE, 0 ,0);

    if (base_address == 0 || base_address == 0xFFFFFFF0) {
        b_output("AHCI Base Address not found or invalid.\n", (unsigned long)strlen("AHCI Base Address not found or invalid.\n"));
        return 0;
    }

    b_output("\nAHCI Base Address: ", (unsigned long)strlen("\nAHCI Base Address: "));
    char address_str[12];
    itoa(base_address, address_str, 16);
    b_output(address_str, strlen(address_str));
    b_output("\n", 1);

    return base_address;
}

void print_port_message(int port, const char* msg) {
    char port_msg[64]; // Buffer for "Port X: " + message
    int index = 0;

    port_msg[index++] = 'P';
    port_msg[index++] = 'o';
    port_msg[index++] = 'r';
    port_msg[index++] = 't';
    port_msg[index++] = ' ';
    if (port > 9) {
        port_msg[index++] = '0' + (port / 10);
    }
    port_msg[index++] = '0' + (port % 10);
    port_msg[index++] = ':';
    port_msg[index++] = ' ';

    while (*msg && index < sizeof(port_msg) - 1) {
        port_msg[index++] = *msg++;
    }
    port_msg[index] = '\0'; // Null-terminate the string

    b_output(port_msg, strlen(port_msg));
    b_output("\n", 1);
}

void parse_sata_info(const u8* data, int port) {
    char msg[128];

    // Check if the buffer is non-empty by inspecting the serial number
    char serial[21] = {0};
    for (int i = 0; i < 20; i += 2) {
        serial[i] = data[20 + i + 1]; // High byte
        serial[i + 1] = data[20 + i]; // Low byte
    }

    if (strlen(serial) == 0) {
        print_port_message(port, "No Data Retrieved");
        return;
    }

    print_port_message(port, "Information of SATA device");

    // Print Serial Number
    b_output("Serial Number: ", strlen("Serial Number: "));
    b_output(serial, strlen(serial));
    b_output("\n", 1);

    // Model Number (Word 27–46)
    char model[41] = {0};
    for (int i = 0; i < 40; i += 2) {
        model[i] = data[54 + i + 1];
        model[i + 1] = data[54 + i];
    }

    b_output("Model: ", strlen("Model: "));
    b_output(model, strlen(model));
    b_output("\n", 1);

    // Firmware Revision (Word 23–26)
    char firmware[9] = {0};
    for (int i = 0; i < 8; i += 2) {
        firmware[i] = data[46 + i + 1];
        firmware[i + 1] = data[46 + i];
    }

    b_output("Firmware: ", strlen("Firmware: "));
    b_output(firmware, strlen(firmware));
    b_output("\n", 1);

    // User Addressable Sectors (Word 60–61)
    unsigned int sectors = (data[120] | (data[121] << 8)) | ((data[122] | (data[123] << 8)) << 16);
    unsigned long long capacity_bytes = (unsigned long long)sectors * 512; // Sectors * 512 bytes
    unsigned int size_mb = capacity_bytes / (1024 * 1024); // Convert to MB
    unsigned int size_gb = size_mb / 1024; // Convert to GB

    // Print Capacity
    itoa(size_mb, msg, 10);
    b_output("Capacity: ", strlen("Capacity: "));
    b_output(msg, strlen(msg));
    b_output(" MB (", 5);
    itoa(size_gb, msg, 10);
    b_output(msg, strlen(msg));
    b_output(" GB)\n", 5);
    b_output("\n", 1);
}

void detect_sata_devices(unsigned int ahci_base) {
    
    unsigned int pi = mmio_read32(ahci_base + AHCI_PI);

    // Output the value of PI for debugging
    b_output("\nPorts Implemented (PI) Register: ", strlen("\nPorts Implemented (PI) Register: "));
    char pi_str[9]; // Buffer for hexadecimal string (8 chars + null terminator)
    itoa(pi, pi_str, 2);
    b_output(pi_str, strlen(pi_str));
    b_output("\n", 1);

    unsigned int pa = b_system(AHCI_PA, 0 , 0);
    b_output("\nPorts Active (PA) Register: ", strlen("\nPorts Active (PA) Register: "));
    char pa_str[32];
    itoa(pa, pa_str, 2);
    b_output(pa_str, strlen(pa_str));
    b_output("\n",1);

    for (int port = 0; port < 10; port++) {
        memset(buffer, 0, sizeof(buffer)); 
        if (pi & (1 << port)) { 
            unsigned int ssts = mmio_read32(ahci_base + 0x100 + (port * 0x80) + AHCI_PxSSTS);
            unsigned int det = ssts & 0xF;    
            unsigned int ipm = (ssts >> 8) & 0xF;

            b_output("\n", 1);

            print_port_message(port, "Implemented Port.");

            b_output("Sata Status (PxSSTS) Register: ", strlen("Sata Status (PxSSTS) Register: "));
            char ssts_str[12];
            itoa(ssts, ssts_str, 2);
            b_output(ssts_str, strlen(ssts_str));
            b_output("\n", 1);

            if (det == 3 && ipm == 1) {
                b_system_mem(AHCI_ID, (char*)buffer, port);
                parse_sata_info((const u8*)buffer, port);
            } else {
                print_port_message(port, "No Active Device Found.");
                // b_output("\n",1);
            }
        }else{
            print_port_message(port, "Non - Implemented Port.");
        }
    }
}

void main() {
    b_output("\nStarting SATA device identification...\n", (unsigned long)strlen("\nStarting SATA device identification...\n"));

    unsigned int ahci_base = find_ahci_base_address();
    if (ahci_base != 0) {
        detect_sata_devices(ahci_base);
    }

    b_output("\nFinished.", 10);
}
