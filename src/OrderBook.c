/* minIP */
/* Written by Ian Seyler */

// Linux compile: gcc minIP.c -o minIP
// Linux usage: ./minIP eth1 192.168.0.99 255.255.255.0 192.168.0.1

// BareMetal compile: ./build.sh
// BareMetal usage: minIP.app

const char version_string[] = "minIP v0.7.0 (2024 06 11)\n";

#define __USE_MISC

/* Global Includes */
#include "libBareMetal.h"

/* Global functions */
u16 checksum(u8* data, u16 bytes);
u16 checksum_tcp(u8* data, u16 bytes, u16 protocol, u16 length);
int net_init();
int net_exit();
int net_send(unsigned char* data, unsigned int bytes);
int net_recv(unsigned char* data);
u16 swap16(u16 in);
u32 swap32(u32 in);
void send_igmp_request(u8 type);
void get_input();
void* memset(void* s, int c, int n);
void* memcpy(void* d, const void* s, int n);
int strlen(const char* s);
void itoa(int num);

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
#define PROTOCOL_IP_UDP 17
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8
#define TCP_ACK 16
#define TCP_PSH 8
#define TCP_RST 4
#define TCP_SYN 2
#define TCP_FIN 1

/* Global variables */
u8 src_MAC[6] = {0x08, 0x00, 0x27, 0x7a, 0xbb, 0x8a};
u8 dst_MAC[6] = {0x01, 0x00, 0x5e, 0x64, 0x27, 0x59};
u8 leave_MAC[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x02};
u8 dst_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
// u8 src_IP[4] = {10, 13, 100, 1};
u8 src_IP[4] = {10, 244, 176, 89};           // Test Market
u8 src_SN[4] = {255, 255, 255, 240};
u8 src_GW[4] = {10, 244, 176, 81};           // Test Market
// u8 src_GW[4] = {10, 13, 100, 254};
u8 dst_IP[4] = {239, 100, 39, 89};           // Test Market
// u8 dst_IP[4] = {225, 5, 6, 7};
u8 leave_IP[4] = {224, 0, 0, 2};

unsigned char buffer[ETH_FRAME_LEN];
unsigned char tosend[ETH_FRAME_LEN];
unsigned char aux[11];
int s; // Socket variable
int running = 1, c, recv_packet_len;
unsigned int tint, tint0, tint1, tint2, tint3;

/* Global structs */
#pragma pack(1)
typedef struct eth_header {
	u8 dest_mac[6];
	u8 src_mac[6];
	u16 type;
} eth_header; // 14 bytes
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
} ipv4_packet; // 20 bytes since we don't support options
typedef struct icmp_packet {
	ipv4_packet ipv4;
	u8 type;
	u8 code;
	u16 checksum;
	u16 id;
	u16 sequence;
	u64 timestamp;
	u8 data[2]; // Set to 2 so can be used as pointer
} icmp_packet;
typedef struct igmp_packet {
	ipv4_packet ipv4;
	u8 opts[4];
	u8 type;
	u8 max_resp;
	u16 checksum;
	u8 mcast_addr[4];
} igmp_packet;
typedef struct udp_packet {
	ipv4_packet ipv4;
	u16 src_port;
	u16 dest_port;
	u16 length;
	u16 checksum;
	u8 data[2]; // Set to 2 so can be used as pointer
} udp_packet;
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
	// Options and data
//	u8 data[2]; // Set to 2 so can be used as pointer
} tcp_packet;

#define SECTOR_SIZE 4096

char sector_buffer[SECTOR_SIZE];
unsigned int buffer_index = 0; // Tracks the current buffer index
int starting_sector = 1; // Tracks the current sector for writing logs
int total_sector_written = 1;

void inttostr(unsigned int value, char* str, int base) {
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

void update_written_sector_count() {
    char buffer[SECTOR_SIZE];
    memset(buffer, 0, sizeof(buffer)); // Clear the buffer

    int j =total_sector_written;
    // Convert the total sectors written to a string and store it at the start of the buffer
    char sector_count_str[16];
    inttostr(j, sector_count_str, 10); // Base 10
    for (int i = 0; i < sizeof(sector_count_str); i++) {
        buffer[i] = sector_count_str[i];
    }
    // Write the buffer to the 0th sector
    b_storage_write(buffer, 0, 1, 0);

    // Increment the sector count
    total_sector_written++;
}

void write_sector_buffer() {
    if (buffer_index > 0) {
        char buffer[4096];
        for (int i = 0; i < sizeof(sector_buffer); i++) {
            buffer[i] = sector_buffer[i];
        }
        int i=starting_sector;
        // b_output(buffer,sizeof(buffer));
        b_storage_write(buffer, i, 1, 0); // Write one sector
        update_written_sector_count();
        starting_sector++; // Increment the starting sector
        buffer_index = 0;  // Reset the buffer index
        memset(buffer,0,sizeof(buffer));
        memset(sector_buffer, 0, SECTOR_SIZE); // Clear the buffer
    }
}

// Enhanced log_output function to log data
void log_output(const char *data, unsigned long length) {
    // Ensure all data fits in the buffer
    for (unsigned long i = 0; i < length; i++) {
        sector_buffer[buffer_index++] = data[i];
        if (buffer_index == SECTOR_SIZE) {
            write_sector_buffer(); // Flush buffer to disk when full
        }
    }
    // Optionally send output to the standard log_output
    b_output(data,length);
}

const char arp[] = "arp\n";
const char ipv4[] = "ipv4\n";
const char ping[] = "ping\n";

#include "orderbook.h"

/* Main code */
int main()
{

	Header message_header;
	TradeMessageBody trade_message_body;
	OrderMessageBody order_message_body;

	void* parse_data(u8* data);

	static PriceTable buy_array[MAX_PRICE], sell_array[MAX_PRICE];

	int buy_set[5], sell_set[5];

	static OrderBookData uniqueOrderIdData[MAX_ORDER_OFFSET];

	int buy_size = 0;
	int sell_size = 0;
	u64 first_order_id = 0;

	// u16 msg_len, stream_id;
	// u32 seq_no;
	char msg_type;

	// u8 trade_executed = 0;
	u64 buy_order_id, sell_order_id;
	u32 buy_order_offset, sell_order_offset, day_order_offset;

	// u64 epoch;
	u64 day_unique_order_id;
	u32 token;
	char order_type;
	u32 price,quantity;


	int i,j;
	for(i=0 ; i<MAX_ORDER_OFFSET; i++)
	{
		uniqueOrderIdData[i].order_type = '\0';
		uniqueOrderIdData[i].price = 0;
		uniqueOrderIdData[i].quantity = 0;
	}
	// uniqueOrderIdData[i].order_type = '\0';

	for(j=0; j < MAX_PRICE; j++)
	{
		buy_array[j].price = 0;
		buy_array[j].quantity=0;
		buy_array[j].participant=0;
		buy_array[j].volume=0;
		sell_array[j].price =0;
		sell_array[j].quantity=0;
		sell_array[j].participant=0;
		sell_array[j].volume=0;
		
	}
	
	log_output(version_string, (unsigned long)strlen(version_string));
	net_init();

	for(int i = 0; i<5; ++i)
		send_igmp_request('\x16');

	get_input();

	u32 count_packet = 0;
	u32  counter_T = 0;
	u32 counter_N = 0;
	
	while(running == 1)
	{
		recv_packet_len = net_recv(buffer);
		eth_header* rx = (eth_header*)buffer;

		if (recv_packet_len > 0) // Make sure we received a packet
		{
			memset(tosend, 0, ETH_FRAME_LEN); // clear the send buffer
			if (swap16(rx->type) == ETHERTYPE_IPv4)
			{
				ipv4_packet* rx_ipv4 = (ipv4_packet*)buffer;
				if (rx_ipv4->protocol == PROTOCOL_IP_UDP)
				{
					if (*(u32*)rx_ipv4->dest_ip != *(u32*)dst_IP)
						continue;

					udp_packet* rx_udp = (udp_packet*)buffer;
					u8* buff = rx_udp->data;
					
					memcpy((u8*)&message_header, buff, MESSAGE_HEADER_BYTES);
					msg_type = message_header.msg_type;
					msg_len = message_header.msg_len;
					stream_id = message_header.stream_id;
					seq_no = message_header.seq_no;

					count_packet++;

					log_output("\ncounter : ", 12);
					itoa(count_packet);


					// continue;



						if (msg_type == 'T')
						{
							if(first_order_id == 0)
								continue;

							memcpy((char *)&trade_message_body, buff + MESSAGE_HEADER_BYTES,
								TRADE_MESSAGE_BODY_BYTES);
							epoch = (trade_message_body.epoch);
							buy_order_id = (trade_message_body.buy_order_id);
							sell_order_id = (trade_message_body.sell_order_id);
							token = (trade_message_body.token);
							price = (trade_message_body.price);
							quantity = (trade_message_body.quantity);

							// if ((buy_order_id < first_order_id) || (sell_order_id < first_order_id))
							// 	continue;
							
							buy_order_offset = buy_order_id - first_order_id;
							sell_order_offset = sell_order_id - first_order_id;

							u32 token_index = token;

	
							log_output("\nToken : ",9);
							itoa(token);
							
							if (token_index != SELECTED_TOKEN)
								continue;

							counter_T++;
							
	
							log_output("\ncounter_T : ", 13);
							itoa(counter_T);
	
							log_output("\nBuy Order Id : ", 17);
							itoa(buy_order_id & 0xFFFFFFFF);
							log_output("\nSell Order Id : ", 18);
							itoa(sell_order_id & 0xFFFFFFFF);

							// cout << "T message Info "
							//      << "epoch : " << epoch << " buy_order_id : " << buy_order_id << " sell_order_id : " << sell_order_id << " token : " << token << " price : " << price << " quantity : " << quantity << endl;
							if(buy_order_id >= first_order_id)
								if (buy_order_id != 0 && findMap(uniqueOrderIdData, buy_order_offset))
							{

								log_output("\nDebug 1", 8);
								if (uniqueOrderIdData[buy_order_offset].quantity < quantity)
								{
									log_output("\nDebug 2", 8);
									// cout<<"quantity being executed is larger than ordered. Denied"<<endl;
									continue;
								}

								log_output("\nDebug 1.1", 10);

								uniqueOrderIdData[buy_order_offset].quantity -= quantity; // buy execution

								int buy_price = uniqueOrderIdData[buy_order_offset].price;
								if((0>buy_price)  || (buy_price>1000000) )
								continue;

								int buy_price_index = buy_price / 5;

								log_output("\nDebug 1.2", 10);

								if (uniqueOrderIdData[buy_order_offset].quantity == 0)
								{
									eraseMap(uniqueOrderIdData, buy_order_offset);

								}

								log_output("\nDebug 1.3", 10);

								log_output("\nPrice : ", 9);
								itoa(buy_price_index);
								log_output("\tB_A_P : ", 9);
								itoa(buy_array[buy_price_index].price);
								log_output("\tB_A_Q : ", 9);
								itoa(buy_array[buy_price_index].quantity);
								log_output("\tB_A_V : ", 9);
								itoa(buy_array[buy_price_index].volume);

								if (buy_array[buy_price_index].price == 0)
								{
									log_output("\nDebug 3", 8);
									//  Logical Error, check program
									// cout << "Logical error from buy trade , check program" << endl;
									continue;
								}

								buy_array[buy_price_index].quantity -= quantity;
								buy_array[buy_price_index].volume += quantity;
								// tradeVolume[buy_price] += quantity;

								log_output("\nDebug 4", 8);
		

								if (buy_array[buy_price_index].quantity == 0)
								{

								log_output("\nDebug 5", 8);
									buy_array[buy_price_index].price = 0;
									if (find(buy_set,&buy_size,buy_price, ASC) != -1)
									{
										erase(buy_set,&buy_size,buy_price, ASC);
										if (buy_size == (BUY_SELL_DEPTH - 1))
										{
											for (u32 itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
											{
												if (buy_array[itr / 5].price != 0)
												{
													insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
													break;
												}
											}
										}
									}
								}

								trade_executed = 1;


								log_output("\nTrade_B : ", 11);
								itoa(trade_executed);

							}

							// log_output("\nTrade_BB : ", 12);
							// itoa(trade_executed);


							if(sell_order_id >= first_order_id)
								if (sell_order_id != 0 && findMap(uniqueOrderIdData, sell_order_offset))
							{

								log_output("\nDebug 6", 8);

								if (uniqueOrderIdData[sell_order_offset].quantity < quantity)
								{
									log_output("\nDebug 7", 8);
									// quantity being executed is larger than ordered. Denied
									continue;
								}

								log_output("\nDebug 6.1", 10);
								
								uniqueOrderIdData[sell_order_offset].quantity -= quantity;  //sell execution

								int sell_price = uniqueOrderIdData[sell_order_offset].price;
								if((0>sell_price) || (sell_price>1000000) )
								continue;
								int sell_price_index = sell_price / 5;

								log_output("\nDebug 6.2", 10);
								
								if (uniqueOrderIdData[sell_order_offset].quantity == 0)
								{
									log_output("\nDebug 8", 8);
									eraseMap(uniqueOrderIdData, sell_order_offset);
								}

								log_output("\nDebug 6.3", 10);
								
								log_output("\nPrice : ", 9);
								itoa(sell_price_index);
								log_output("\nS_A_P : ", 9);
								itoa(sell_array[sell_price_index].price);
								log_output("\tS_A_Q : ", 9);
								itoa(sell_array[sell_price_index].quantity);
								log_output("\tS_A_V : ", 9);
								itoa(sell_array[sell_price_index].volume);
								
								if (sell_array[sell_price_index].price == 0)
								{	
									log_output("\nDebug 9", 8);
									// cout << "[orderbook][error]: Logical Error, check program";
									// cout << endl
									//      << "[orderbook][debug]: "
									//      << sell_price << " :"
									//      << sell_array[sell_price_index].price;
									continue; 
								}

								sell_array[sell_price_index].quantity -= quantity;
								sell_array[sell_price_index].volume += quantity;
								// tradeVolume[sell_price] += quantity;

								log_output("\nDebug 6.4", 10);
								
								if (sell_array[sell_price_index].quantity == 0)
								{
									log_output("\nDebug 10", 9);
									sell_array[sell_price_index].price = 0;
									if (find(sell_set, &sell_size, sell_price, DSC) != -1)
									{
										erase(sell_set, &sell_size, sell_price, DSC);
										if (sell_size == (BUY_SELL_DEPTH - 1))
										{
											for (u32 itr = sell_set[0] + 5; itr <= minMax.max; itr = itr + 5)
											{
												if (sell_array[itr / 5].price != 0)
												{
													insert(sell_set, &sell_size, sell_array[itr / 5].price, DSC);
													break;
												}
											}
										}
									}
								}

								trade_executed = 1;

								log_output("\nTrade_S : ", 11);
								itoa(trade_executed);
							}

							// if(trade_executed)
							// {
							//     curr_trade_info.price = price;
							//     curr_trade_info.quantity = quantity;
							//     curr_trade_info.time = epoch;

							//     // cout << "LTP : " << curr_trade_info.price << " LTQ : " << curr_trade_info.quantity << " LTT : " << curr_trade_info.time << endl;
							// }

							// log_output("\nTrade_SS : ", 12);
							// itoa(trade_executed);
							
						}

						else if(msg_type == 'N' || msg_type == 'M' || msg_type == 'X' )
						{
							memcpy((char *)&order_message_body, buff + MESSAGE_HEADER_BYTES,
								ORDER_MESSAGE_BODY_BYTES);

							token = order_message_body.token;
							log_output("\nToken : ", 9);
							itoa(token);
							
							if (token != SELECTED_TOKEN)
								continue;
							
							counter_N++;
							
							log_output("\ncounter_NMX : ", 15);
							itoa(counter_N);
							
							epoch = order_message_body.epoch;
							day_unique_order_id = order_message_body.day_unique_order_id;

							if (first_order_id == 0)
								first_order_id = day_unique_order_id;
							
							else if (day_unique_order_id < first_order_id)
							continue;

							day_order_offset = day_unique_order_id - first_order_id;

							order_type = order_message_body.order_type;
							price = order_message_body.price;
							quantity = order_message_body.quantity;

							// u32 token_index = token;

							// log_output("Message type : ", 16);
							// log_output(&msg_type, 1);
							// log_output("\n", 1);
							// log_output("Order type : ", 14);
							// log_output(&order_type, 1);
							// log_output("\n", 1);
							// log_output("Unique Order ID : ", 19);
							// itoa(day_unique_order_id & 0xFFFFFFFF);
							// log_output("\n", 1);
							
							// cout << "N,M,X message info "
							//      << "epoch : " << epoch << " msg type : " << msg_type << ", day_unique_order_id : " << day_unique_order_id << " token : " << token << " order_type : " << order_type << " price : " << price << " quantity : " << quantity << endl;

							u32 price_index = price / 5;
							

							if (price !=0)
								if (minMax.min > price)
									minMax.min = price;

							if (minMax.max < price)
								minMax.max = price;

							OrderBookData entry;
							entry.order_type = order_type;
							entry.price = price;
							entry.quantity = quantity;

							if (msg_type == 'N')
							{
								uniqueOrderIdData[day_order_offset] = entry;

								if (order_type == 'B')
								{
									if (buy_array[price_index].price == 0)
									{
										buy_array[price_index].price = price;
										buy_array[price_index].quantity = quantity;
										buy_array[price_index].participant = 1;

										if (buy_size < BUY_SELL_DEPTH)
										{
											insert(buy_set, &buy_size, price, ASC);
										}
										else
										{
											if (buy_set[0] < price)
											{
												erase(buy_set, &buy_size, buy_set[0], ASC);
												insert(buy_set, &buy_size, price, ASC);
											}
										}
									}
									else
									{
										buy_array[price_index].quantity =  buy_array[price_index].quantity + quantity;
										buy_array[price_index].participant++;
									}
								}

								else if (order_type == 'S')
								{
									if (sell_array[price_index].price == 0)
									{
										sell_array[price_index].price = price;
										sell_array[price_index].quantity = quantity;
										sell_array[price_index].participant = 1;

										if (sell_size < BUY_SELL_DEPTH)
										{
											insert(sell_set, &sell_size, price, DSC);
										}
										else
										{
											if (sell_set[0] > price)
											{
												erase(sell_set, &sell_size, sell_set[0], DSC);
												insert(sell_set, &sell_size, price, DSC);
											}
										}
									}
									else
									{
										sell_array[price_index].quantity = sell_array[price_index].quantity + quantity;
										sell_array[price_index].participant++;
									}
								}
							}

							else if (msg_type == 'M')
							{

								// if(day_unique_order_id < first_order_id)
								// continue;

								u32 old_price, old_quantity;
								u32 old_price_index;

								// if the record of the order exists with us
								if(findMap(uniqueOrderIdData, day_order_offset))
								{
									old_price = uniqueOrderIdData[day_order_offset].price;
									old_price_index = old_price / 5;

									old_quantity = uniqueOrderIdData[day_order_offset].quantity;

									// uniqueOrderIdData[day_order_offset] = entry;

									if (order_type == 'B')
									{
										buy_array[old_price_index].quantity -= old_quantity;
										buy_array[old_price_index].participant--;

										// *****
										if (buy_array[old_price_index].quantity == 0)
										{
											buy_array[old_price_index].price = 0;
											if (find(buy_set,&buy_size,old_price, ASC) != -1)
											{
												erase(buy_set,&buy_size,old_price, ASC);
												if (buy_size == (BUY_SELL_DEPTH - 1))
												{
													for (u32 itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
													{
														if (buy_array[itr / 5].price != 0)
														{
															insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
															break;
														}
													}
												}
											}
										}
									}
									// if (order_type == 'S')
									else 
									{
										sell_array[old_price_index].quantity -= old_quantity;
										sell_array[old_price_index].participant--;

										if (sell_array[old_price_index].quantity == 0)
										{
											sell_array[old_price_index].price = 0;
											if (find(sell_set, &sell_size, old_price, DSC) != -1)
											{
												erase(sell_set, &sell_size, old_price, DSC);
												if (sell_size == (BUY_SELL_DEPTH - 1))
												{
													for (u32 itr = sell_set[0] + 5; itr <= minMax.max; itr = itr + 5)
													{
														if (sell_array[itr / 5].price != 0)
														{
															insert(sell_set, &sell_size, sell_array[itr / 5].price, DSC);
															break;
														}
													}
												}
											}
										}
									}
								}

								// anyways, place the new order
								if (order_type == 'B')
								{
									if (buy_array[price_index].price == 0)
									{
										buy_array[price_index].price = price;
										buy_array[price_index].quantity = quantity;
										buy_array[price_index].participant = 1;

										if (buy_size < BUY_SELL_DEPTH)
										{
											insert(buy_set, &buy_size, price, ASC);
										}
										else
										{
											if (buy_set[0] < price)
											{
												erase(buy_set, &buy_size, buy_set[0], ASC);
												insert(buy_set, &buy_size, price, ASC);
											}
										}
									}
									else
									{
										buy_array[price_index].quantity += quantity;
										buy_array[price_index].participant++;
									}
								}
								// if (order_type == 'S')
								else 
								{
									if (sell_array[price_index].price == 0)
									{
										sell_array[price_index].price = price;
										sell_array[price_index].quantity = quantity;
										sell_array[price_index].participant = 1;

										if (sell_size < BUY_SELL_DEPTH)
										{
											insert(sell_set, &sell_size, price, DSC);
										}
										else
										{
											if (sell_set[0] > price)
											{
												erase(sell_set, &sell_size, sell_set[0], DSC);
												insert(sell_set, &sell_size, price, DSC);
											}
										}
									}
									else
									{
										sell_array[price_index].quantity += quantity;
										sell_array[price_index].participant++;
									}
								}

								uniqueOrderIdData[day_order_offset] = entry;
							}

							else if (msg_type == 'X')
							{
								if(findMap(uniqueOrderIdData, day_order_offset) == 0)
									continue; 

								u32 cancel_price = uniqueOrderIdData[day_order_offset].price;
								u32 cancel_price_index = cancel_price / 5;

								u32 cancel_quantity = uniqueOrderIdData[day_order_offset].quantity;

								eraseMap(uniqueOrderIdData, day_order_offset);

								if (order_type == 'B')
								{
									buy_array[cancel_price_index].quantity -= cancel_quantity;
									buy_array[cancel_price_index].participant--;

									// *****
									if (buy_array[cancel_price_index].quantity == 0)
									{
										buy_array[cancel_price_index].price = 0;
										if (find(buy_set,&buy_size,cancel_price, ASC) != -1)
										{
											erase(buy_set,&buy_size,cancel_price, ASC);
											if (buy_size == (BUY_SELL_DEPTH - 1))
											{
												for (int itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
												{
													if (buy_array[itr / 5].price != 0)
													{
														insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
														break;
													}
												}
											}
										}
									}
								}

								else if (order_type == 'S')
								{
									sell_array[cancel_price_index].quantity -= cancel_quantity;
									sell_array[cancel_price_index].participant--;

									if (sell_array[cancel_price_index].quantity == 0)
									{
										sell_array[cancel_price_index].price = 0;
										if (find(sell_set, &sell_size, cancel_price, DSC) != -1)
										{
											erase(sell_set, &sell_size, cancel_price, DSC);
											if (sell_size == (BUY_SELL_DEPTH - 1))
											{
												for (int i = sell_set[0] + 5; i <= minMax.max; i = i + 5)
												{
													if (sell_array[i / 5].price != 0)
													{
														insert(sell_set, &sell_size, sell_array[i / 5].price, DSC);
														break;
													}
												}
											}
										}
									}
								}
							}
						}

						else if (msg_type == 'Z')
						{
							HeartBeatBody last_sequence;

							memcpy((char *)&last_sequence, buff + MESSAGE_HEADER_BYTES,
								HEARTBEAT_MESSAGE_BODY_BYTES);

							// int lst_seq_num = last_sequence.lst_seq_num;

							// cout << "Z [info]: last sequence number: " << lst_seq_num << endl;

							log_output("\nReceived Heartbeat message\n", 29);
							continue;
						}

						else
						{
							// cout << "[orderbook][debug]: " << msg_type << endl;
							// cout << "[orderbook][info]: Unhandeled message type, Exiting\n";
							continue;
						}
					
					log_output("\nBUY_SIZE : ",12);
					itoa(buy_size);
					log_output("\tSELL_SIZE : ",13);
					itoa(sell_size);

					log_output("\nData : B", 10);
					for(int i = 0; i<buy_size; ++i)
					{
						itoa(buy_set[i]);
						log_output(" ", 1);
					}
					log_output("S ", 2);
					for(int i = 0; i<sell_size; ++i)
					{
						itoa(sell_set[i]);
						log_output(" ", 1);
					}
					log_output("\n", 1);
				
					// parse_data(data);

				}

			}
			
		}
	}

	log_output("\n", 1);
	net_exit();
	return 0;
}

void send_igmp_request(u8 type)
{
	memset(tosend, 0, ETH_FRAME_LEN);
	igmp_packet* tx_igmp = (igmp_packet*)tosend;
	// Ethernet
	if (type == '\x16'){
		memcpy(tx_igmp->ipv4.ethernet.dest_mac, dst_MAC, 6);
		memcpy(tx_igmp->ipv4.dest_ip, dst_IP, 4);
	}
	else if (type == '\x17'){
		memcpy(tx_igmp->ipv4.ethernet.dest_mac, leave_MAC, 6);
		memcpy(tx_igmp->ipv4.dest_ip, leave_IP, 4);
	}
	tx_igmp->ipv4.ethernet.type = swap16(ETHERTYPE_IPv4);
	// IPv4
	memcpy(tx_igmp->ipv4.ethernet.src_mac, src_MAC, 6);
	memcpy(tx_igmp->ipv4.src_ip, src_IP, 4);
	tx_igmp->ipv4.version = '\x46';
	tx_igmp->ipv4.dsf = '\xc0';
	tx_igmp->ipv4.total_length = swap16(32);
	tx_igmp->ipv4.id = 0;
	tx_igmp->ipv4.flags = 64;
	tx_igmp->ipv4.ttl = 1;
	tx_igmp->ipv4.protocol = 2;
	tx_igmp->opts[0] = '\x94';
	tx_igmp->opts[1] = '\x04';
	tx_igmp->ipv4.checksum = checksum(tosend + 14, 24); // Need to recalculate checksum
	// IGMP
	tx_igmp->type = type;
	memcpy(tx_igmp->mcast_addr, dst_IP, 4);
	tx_igmp->checksum = checksum(tosend + 38, sizeof(igmp_packet)-14-20); // Frame length - MAC header - IPv4 header
	// Send the reply
	log_output("Sending IGMP packet\n", 20);
	net_send(tosend, sizeof(igmp_packet));
}

void get_input()
{
		SELECTED_TOKEN = SELECTED_TOKEN_DEFAULT;
		return;
		log_output("Please enter token number : ", 29);
		int token = 0, flag = 0;
		char key;
		key = b_input();
		log_output("Got key : ", 11);
		int asc = key - '\0';
		// log_output(&key, 1);
		itoa(asc);
		log_output("\n", 1);
		while(key != '\n')
		{
			if (key >= '0' && key <= '9')
			{
				token = 10*token + key - '0';
			}
			else
			{
				flag = 1; break;
			}
		}
		if (token == 0 || flag == 1)
		{
				log_output("Invalid Character. Please Reenter.", 35);
				get_input();
		}
		else
			SELECTED_TOKEN = token;
}

/* checksum - Calculate a checksum value */
// Returns 16-bit checksum
u16 checksum(u8* data, u16 bytes)
{
	u32 sum = 0;
	u16 i;

	for (i=0; i<bytes-1; i+=2) // Add up the words
		sum += swap16(*(u16 *) &data[i]);

	if (bytes & 1) // Add the left-over byte if there is one
		sum += (u8) data[i];

	while (sum >> 16) // Fold total to 16-bits
		sum = (sum & 0xFFFF) + (sum >> 16);

	return swap16(~sum); // Return 1's complement
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
	/* Populate the MAC Address */
	/* Pulls the MAC from the OS sys var table... so gross */
	// char * os_MAC = (void*)0x110050;
	// src_MAC[0] = os_MAC[0];
	// src_MAC[1] = os_MAC[1];
	// src_MAC[2] = os_MAC[2];
	// src_MAC[3] = os_MAC[3];
	// src_MAC[4] = os_MAC[4];
	// src_MAC[5] = os_MAC[5];

	// ABOVE CODE DOESN'T FETCH THE RIGHT VALUE
    u64 val0 = 0, val1 = 0, val2 = 0;
	val0 = b_system(MAC_GET, val1, val2); // Get the MAC as a u64
	char * MAC = (void*)&val0;

	src_MAC[0] = MAC[5]; // Get the MAC bytes in the correct order
	src_MAC[1] = MAC[4];
	src_MAC[2] = MAC[3];
	src_MAC[3] = MAC[2];
	src_MAC[4] = MAC[1];
	src_MAC[5] = MAC[0];
	log_output("Our MAC address : ", 19);
	for(int i = 0; i<6; ++i)
	{
		itoa(src_MAC[i]);
		if (i != 5)
			log_output(":", 1);
	}
	log_output("\n", 1);
	/* We should now have a working port to send/recv raw frames */
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
	return bytes;
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

void reverse(unsigned char* str, int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

void itoa(int num)
{
	int i = 0, neg = 0;
	if (num == 0)
	{
		aux[i++] = '0';
		aux[i] = '\0';
		log_output((const char *)aux, strlen((const char *)aux));
		return;
	}

	if(num < 0)
		neg = 1;

	while(num != 0)
	{
		int rem = num % 10;
		aux[i++] = rem + '0';
		num /= 10;
	}

	if (neg)
		aux[i++] = '-';
	
	aux[i] = '\0';
	reverse(aux, i);

	log_output((const char *)aux, strlen((const char *)aux));
	return;
}
#include "orderbook.c"

/* EOF */