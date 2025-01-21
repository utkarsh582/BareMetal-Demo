// Pack all attributes after primary testing

#define MAX_PRICE 1000000
// #define SELECTED_TOKEN 5732
#define SELECTED_TOKEN_DEFAULT 22
#define PORT  5004
// #define MULTICAST_GROUP_IP "239.100.39.89"
#define MAXLINE 64
#define MESSAGE_HEADER_BYTES 9
#define TRADE_MESSAGE_BODY_BYTES 36
#define ORDER_MESSAGE_BODY_BYTES 29
#define HEARTBEAT_MESSAGE_BODY_BYTES 4
#define BUY_SELL_DEPTH 5
#define MAX_ORDER_OFFSET 1000000

int SELECTED_TOKEN;

typedef struct HeartBeatBody
{
    u32 lst_seq_num;
}HeartBeatBody;

typedef struct __attribute__((packed)) Header
{
    u16 msg_len;
    u16 stream_id;
    u32 seq_no;
    u8 msg_type;
} Header;

typedef struct __attribute__((packed)) OrderMessageBody
{
    u64 epoch;
    u64 day_unique_order_id;
    u32 token;
    u8 order_type;
    u32 price;
    u32 quantity;
} OrderMessageBody;

typedef struct __attribute__((packed)) TradeMessageBody
{
    u64 epoch;
    u64 buy_order_id;
    u64 sell_order_id;
    u32 token;
    u32 price;
    u32 quantity;
} TradeMessageBody;

struct __attribute__((packed)) PriceTable
{
    u32 price;
    u32 quantity;
    u32 participant;
    u32 volume;
};

typedef  struct PriceTable PriceTable;

struct __attribute__((packed)) OrderBookData
{
    u8 order_type;
    u32 price;
    u32 quantity;
};

typedef struct OrderBookData OrderBookData;

struct MinMaxPrice
{
  u32 min;
  u32 max;
} minMax = {1000000,0};

typedef struct MinMaxPrice MinMaxPrice;

    // Header message_header;
    // TradeMessageBody trade_message_body;
    // OrderMessageBody order_message_body;

    // void* parse_data(u8* data);

    // PriceTable buy_array[MAX_PRICE], sell_array[MAX_PRICE];

    // int buy_set[5], sell_set[5];

    // OrderBookData uniqueOrderIdData[MAX_ORDER_OFFSET];

int findMap(OrderBookData arr[],int value);
void* eraseMap(OrderBookData arr[], int value);

#define ASC 1
#define DSC -1

int find(int arr[], int *size, int val, int sign);
void* insert(int arr[],int* size, int val, int sign);
void* erase(int arr[], int* size, int val, int sign);

// int buy_size = 0;
// int sell_size = 0;
// u64 first_order_id = 0;

u16 msg_len, stream_id;
u32 seq_no;
// char msg_type;

u8 trade_executed = 0;
// u64 buy_order_id, sell_order_id;
// u32 buy_order_offset, sell_order_offset, day_order_offset;

u64 epoch;
// u64 day_unique_order_id;
// u32 token;
// char order_type;
// u32 price,quantity;

// // u32  counter_T = 0;
// // u32 counter_N = 0;


// void parse_data_x(u8* data)
// {
//     u8 msg_type;
//     memcpy((u8*)&message_header, data, MESSAGE_HEADER_BYTES);
//     msg_type = message_header.msg_type;

//     b_output(", data: ", 9);
//     b_output(&msg_type, 1);
//     b_output("\n", 1);
// }

// void parse_data_x(u8 *data)
// {
//     u8 msg_type;
//     memcpy((u8*)&message_header, data, MESSAGE_HEADER_BYTES);
//     msg_type = message_header.msg_type;

// #if defined(BAREMETAL)
//     b_output("Message Type : ", 16);
//     // b_output(&msg_type, 1);
//     b_output("\n", 1);
// #endif
// #if defined(LINUX)
//     printf("Message Type : %c\n", msg_type);
// #endif
    // // T
    // u64 epoch, buy_order_id, sell_order_id;
    // u32 token, price, quantity;

    // // N,M,X
    // u64 day_unique_order_id;
    // u8 order_type;

    // if (msg_type == 'T')
    // {
    //     memcpy((u8*)&trade_message_body, data + MESSAGE_HEADER_BYTES, TRADE_MESSAGE_BODY_BYTES);

    //     epoch = trade_message_body.epoch;
    //     buy_order_id = trade_message_body.buy_order_id;
    //     sell_order_id = trade_message_body.sell_order_id;
    //     token = trade_message_body.token;
    //     price = trade_message_body.price;
    //     quantity = trade_message_body.quantity;
    //     // printf("msg T, buy order id %d, sell order id %d\n", buy_order_id, sell_order_id);
    //     b_output("token : ", 9);
    //     itoa(token);
    //     b_output("\n", 1);
    // }
    // else if (msg_type == 'N' || msg_type == 'M' || msg_type == 'X')
    // {
    //     memcpy((u8 *)&order_message_body, data + MESSAGE_HEADER_BYTES, ORDER_MESSAGE_BODY_BYTES);

    //     day_unique_order_id = order_message_body.day_unique_order_id;
    //     // printf("msg %c, unique order id %d\n", msg_type, day_unique_order_id);
    //     token = order_message_body.token;
    //     order_type = order_message_body.order_type;
    //     price = order_message_body.price;
    //     quantity = order_message_body.quantity;
    //     // printf("price %d, quantity %d\n", price, quantity);
    //     b_output("token : ", 9);
    //     itoa(token);
    //     b_output("\n", 1);
    // }
    // else if (msg_type == 'Z')
    // {
    //     // enjoy life
    //     b_output("\n", 1);
    // }
// }