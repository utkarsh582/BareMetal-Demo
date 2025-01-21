int findMap(OrderBookData arr[],int value)
{
    return arr[value].order_type != '\0';
}

void* eraseMap(OrderBookData arr[], int value)
{
    arr[value].order_type = '\0';
    return 0;
}

int find(int arr[], int *size, int val, int sign)
{
    if (*size == 0)
        return -1;
    int i = (*size) >> 1, j;
    if (arr[i] == val)
        return i;
    if ((arr[i]*sign) > (val*sign))
    {
        for(j = i-1; j>=0; --j)
            if (arr[j] == val)
                return j;
        return -1;
    }
    for(j = i+1; j<*size; ++j)
        if (arr[j] == val)
            return j;
    return -1;
}

void* insert(int arr[],int* size, int val, int sign)
{
    if (find(arr, size, val, sign) != -1)
        return 0;
    int j;
    for(j = *size-1; j>-1 && ((arr[j]*sign) > (val*sign)); j--)
        arr[j+1] = arr[j];
    arr[j + 1] = val;
    (*size)++;
    return 0;
}

void* erase(int arr[], int* size, int val, int sign)
{
    int i = find(arr, size, val, sign);
    if (i == -1)
        return 0;
    int j;
    for(j = i + 1; j<*size; ++j)
        arr[j-1] = arr[j];
    (*size)--;
    return 0;
}


// void* parse_data(u8* buff)
// {
//     // u8 msg_type;
//     memcpy((u8*)&message_header, buff, MESSAGE_HEADER_BYTES);
//     msg_type = message_header.msg_type;
//     msg_len = message_header.msg_len;
//     stream_id = message_header.stream_id;
//     seq_no = message_header.seq_no;


//         if (msg_type == 'T')
//         {
//             if(first_order_id == 0)
//                 return 0;

//             memcpy((char *)&trade_message_body, buff + MESSAGE_HEADER_BYTES,
//                    TRADE_MESSAGE_BODY_BYTES);
//             epoch = (trade_message_body.epoch);
//             buy_order_id = (trade_message_body.buy_order_id);
//             sell_order_id = (trade_message_body.sell_order_id);
//             token = (trade_message_body.token);
//             price = (trade_message_body.price);
//             quantity = (trade_message_body.quantity);

//             buy_order_offset = buy_order_id - first_order_id;
//             sell_order_offset = sell_order_id - first_order_id;

//             u32 token_index = token;

//             // cout << "Token : " << token << "\n";

//             if (token_index != SELECTED_TOKEN)
//                 return 0;

//             b_output("counter : ", 11);
//             itoa(counter);

//             // cout << "T message Info "
//             //      << "epoch : " << epoch << " buy_order_id : " << buy_order_id << " sell_order_id : " << sell_order_id << " token : " << token << " price : " << price << " quantity : " << quantity << endl;

//             if (buy_order_id != 0 && findMap(uniqueOrderIdData, buy_order_offset))
//             {
//                 if (uniqueOrderIdData[buy_order_offset].quantity < quantity)
//                 {
//                     // cout<<"quantity being executed is larger than ordered. Denied"<<endl;
//                     return 0;
//                 }

//                 uniqueOrderIdData[buy_order_offset].quantity -= quantity; // buy execution

//                 int buy_price = uniqueOrderIdData[buy_order_offset].price;
//                 int buy_price_index = buy_price / 5;

//                 if (uniqueOrderIdData[buy_order_offset].quantity == 0)
//                 {
//                     eraseMap(uniqueOrderIdData, buy_order_offset);
//                 }

//                 if (buy_array[buy_price_index].price == 0)
//                 {
//                     //  Logical Error, check program
//                     // cout << "Logical error from buy trade , check program" << endl;
//                     return 0;
//                 }

//                 buy_array[buy_price_index].quantity -= quantity;
//                 buy_array[buy_price_index].volume += quantity;
//                 // tradeVolume[buy_price] += quantity;

//                 if (buy_array[buy_price_index].quantity == 0)
//                 {
//                     buy_array[buy_price_index].price = 0;
//                     if (find(buy_set,&buy_size,buy_price, ASC) != -1)
//                     {
//                         erase(buy_set,&buy_size,buy_price, ASC);
//                         if (buy_size == (BUY_SELL_DEPTH - 1))
//                         {
//                             for (u32 itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
//                             {
//                                 if (buy_array[itr / 5].price != 0)
//                                 {
//                                     insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
//                                     break;
//                                 }
//                             }
//                         }
//                     }
//                 }

//                 trade_executed = 1;
//             }

//             if (sell_order_id != 0 && findMap(uniqueOrderIdData, sell_order_offset))
//             {
//                 if (uniqueOrderIdData[sell_order_offset].quantity < quantity)
//                 {
//                     // quantity being executed is larger than ordered. Denied
//                     return 0;
//                 }

//                 uniqueOrderIdData[sell_order_offset].quantity -= quantity;  //sell execution

//                 int sell_price = uniqueOrderIdData[sell_order_offset].price;
//                 int sell_price_index = sell_price / 5;

//                 if (uniqueOrderIdData[sell_order_offset].quantity == 0)
//                 {
//                     eraseMap(uniqueOrderIdData, sell_order_offset);
//                 }

//                 if (sell_array[sell_price_index].price == 0)
//                 {
//                     // cout << "[orderbook][error]: Logical Error, check program";
//                     // cout << endl
//                     //      << "[orderbook][debug]: "
//                     //      << sell_price << " :"
//                     //      << sell_array[sell_price_index].price;
//                     return 0;                }

//                 sell_array[sell_price_index].quantity -= quantity;
//                 sell_array[sell_price_index].volume += quantity;
//                 // tradeVolume[sell_price] += quantity;

//                 if (sell_array[sell_price_index].quantity == 0)
//                 {
//                     sell_array[sell_price_index].price = 0;
//                     if (find(sell_set, &sell_size, sell_price, DSC) != -1)
//                     {
//                         erase(sell_set, &sell_size, sell_price, DSC);
//                         if (sell_size == (BUY_SELL_DEPTH - 1))
//                         {
//                             for (u32 itr = sell_set[0] + 5; itr <= minMax.max; itr = itr + 5)
//                             {
//                                 if (sell_array[itr / 5].price != 0)
//                                 {
//                                     insert(sell_set, &sell_size, sell_array[itr / 5].price, DSC);
//                                     break;
//                                 }
//                             }
//                         }
//                     }
//                 }

//                 trade_executed = 1;
//             }

//             // if(trade_executed)
//             // {
//             //     curr_trade_info.price = price;
//             //     curr_trade_info.quantity = quantity;
//             //     curr_trade_info.time = epoch;

//             //     // cout << "LTP : " << curr_trade_info.price << " LTQ : " << curr_trade_info.quantity << " LTT : " << curr_trade_info.time << endl;
//             // }
//         }

//         else if(msg_type == 'N' || msg_type == 'M' || msg_type == 'X' )
//         {
//             memcpy((char *)&order_message_body, buff + MESSAGE_HEADER_BYTES,
//                    ORDER_MESSAGE_BODY_BYTES);

//             token = order_message_body.token;
//             // cout << "Token : " << token << "\n";

//             if (token != SELECTED_TOKEN)
//                 return 0;
            
//             b_output("counter : ", 11);
//             itoa(counter);

//             epoch = order_message_body.epoch;
//             day_unique_order_id = order_message_body.day_unique_order_id;

//             if (first_order_id == 0)
//                 first_order_id = day_unique_order_id;

//             day_order_offset = day_unique_order_id - first_order_id;

//             order_type = order_message_body.order_type;
//             price = order_message_body.price;
//             quantity = order_message_body.quantity;

//             u32 token_index = token;


//             // cout << "N,M,X message info "
//             //      << "epoch : " << epoch << " msg type : " << msg_type << ", day_unique_order_id : " << day_unique_order_id << " token : " << token << " order_type : " << order_type << " price : " << price << " quantity : " << quantity << endl;

//             u32 price_index = price / 5;
            

//             if (price !=0)
//                 if (minMax.min > price)
//                     minMax.min = price;

//             if (minMax.max < price)
//                 minMax.max = price;

//             OrderBookData entry;
//             entry.order_type = order_type;
//             entry.price = price;
//             entry.quantity = quantity;

//             if (msg_type == 'N')
//             {
//                 uniqueOrderIdData[day_order_offset] = entry;

//                 if (order_type == 'B')
//                 {
//                     if (buy_array[price_index].price == 0)
//                     {
//                         buy_array[price_index].price = price;
//                         buy_array[price_index].quantity = quantity;
//                         buy_array[price_index].participant = 1;

//                         if (buy_size < BUY_SELL_DEPTH)
//                         {
//                             insert(buy_set, &buy_size, price, ASC);
//                         }
//                         else
//                         {
//                             if (buy_set[0] < price)
//                             {
//                                 erase(buy_set, &buy_size, buy_set[0], ASC);
//                                 insert(buy_set, &buy_size, price, ASC);
//                             }
//                         }
//                     }
//                     else
//                     {
//                         buy_array[price_index].quantity =  buy_array[price_index].quantity + quantity;
//                         buy_array[price_index].participant++;
//                     }
//                 }

//                 else if (order_type == 'S')
//                 {
//                     if (sell_array[price_index].price == 0)
//                     {
//                         sell_array[price_index].price = price;
//                         sell_array[price_index].quantity = quantity;
//                         sell_array[price_index].participant = 1;

//                         if (sell_size < BUY_SELL_DEPTH)
//                         {
//                             insert(sell_set, &sell_size, price, DSC);
//                         }
//                         else
//                         {
//                             if (sell_set[0] > price)
//                             {
//                                 erase(sell_set, &sell_size, sell_set[0], DSC);
//                                 insert(sell_set, &sell_size, price, DSC);
//                             }
//                         }
//                     }
//                     else
//                     {
//                         sell_array[price_index].quantity = sell_array[price_index].quantity + quantity;
//                         sell_array[price_index].participant++;
//                     }
//                 }
//             }

//             else if (msg_type == 'M')
//             {
//                 u32 old_price, old_quantity;
//                 u32 old_price_index;

//                 // if the record of the order exists with us
//                 if(findMap(uniqueOrderIdData, day_order_offset))
//                 {
//                     old_price = uniqueOrderIdData[day_order_offset].price;
//                     old_price_index = old_price / 5;

//                     old_quantity = uniqueOrderIdData[day_order_offset].quantity;

//                     uniqueOrderIdData[day_order_offset] = entry;

//                     if (order_type == 'B')
//                     {
//                         buy_array[old_price_index].quantity -= old_quantity;
//                         buy_array[old_price_index].participant--;

//                         // *****
//                         if (buy_array[old_price_index].quantity == 0)
//                         {
//                             buy_array[old_price_index].price = 0;
//                             if (find(buy_set,&buy_size,old_price, ASC) != -1)
//                             {
//                                 erase(buy_set,&buy_size,old_price, ASC);
//                                 if (buy_size == (BUY_SELL_DEPTH - 1))
//                                 {
//                                     for (u32 itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
//                                     {
//                                         if (buy_array[itr / 5].price != 0)
//                                         {
//                                             insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
//                                             break;
//                                         }
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                     // if (order_type == 'S')
//                     else 
//                     {
//                         sell_array[old_price_index].quantity -= old_quantity;
//                         sell_array[old_price_index].participant--;

//                         if (sell_array[old_price_index].quantity == 0)
//                         {
//                             sell_array[old_price_index].price = 0;
//                             if (find(sell_set, &sell_size, old_price, DSC) != -1)
//                             {
//                                 erase(sell_set, &sell_size, old_price, DSC);
//                                 if (sell_size == (BUY_SELL_DEPTH - 1))
//                                 {
//                                     for (u32 itr = sell_set[0] + 5; itr <= minMax.max; itr = itr + 5)
//                                     {
//                                         if (sell_array[itr / 5].price != 0)
//                                         {
//                                             insert(sell_set, &sell_size, sell_array[itr / 5].price, DSC);
//                                             break;
//                                         }
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }

//                 // anyways, place the new order
//                 if (order_type == 'B')
//                 {
//                     if (buy_array[price_index].price == 0)
//                     {
//                         buy_array[price_index].price = price;
//                         buy_array[price_index].quantity = quantity;
//                         buy_array[price_index].participant = 1;

//                         if (buy_size < BUY_SELL_DEPTH)
//                         {
//                             insert(buy_set, &buy_size, price, ASC);
//                         }
//                         else
//                         {
//                             if (buy_set[0] < price)
//                             {
//                                 erase(buy_set, &buy_size, buy_set[0], ASC);
//                                 insert(buy_set, &buy_size, price, ASC);
//                             }
//                         }
//                     }
//                     else
//                     {
//                         buy_array[price_index].quantity += quantity;
//                         buy_array[price_index].participant++;
//                     }
//                 }
//                 // if (order_type == 'S')
//                 else 
//                 {
//                     if (sell_array[price_index].price == 0)
//                     {
//                         sell_array[price_index].price = price;
//                         sell_array[price_index].quantity = quantity;
//                         sell_array[price_index].participant = 1;

//                         if (sell_size < BUY_SELL_DEPTH)
//                         {
//                             insert(sell_set, &sell_size, price, DSC);
//                         }
//                         else
//                         {
//                             if (sell_set[0] > price)
//                             {
//                                 erase(sell_set, &sell_size, sell_set[0], DSC);
//                                 insert(sell_set, &sell_size, price, DSC);
//                             }
//                         }
//                     }
//                     else
//                     {
//                         sell_array[price_index].quantity += quantity;
//                         sell_array[price_index].participant++;
//                     }
//                 }
//             }

//             else if (msg_type == 'X')
//             {
//                 if(findMap(uniqueOrderIdData, day_order_offset) == 0)
//                     return 0; 

//                 u32 cancel_price = uniqueOrderIdData[day_order_offset].price;
//                 u32 cancel_price_index = cancel_price / 5;

//                 u32 cancel_quantity = uniqueOrderIdData[day_order_offset].quantity;

//                 eraseMap(uniqueOrderIdData, day_order_offset);

//                 if (order_type == 'B')
//                 {
//                     buy_array[cancel_price_index].quantity -= cancel_quantity;
//                     buy_array[cancel_price_index].participant--;

//                     // *****
//                     if (buy_array[cancel_price_index].quantity == 0)
//                     {
//                         buy_array[cancel_price_index].price = 0;
//                         if (find(buy_set,&buy_size,cancel_price, ASC) != -1)
//                         {
//                             erase(buy_set,&buy_size,cancel_price, ASC);
//                             if (buy_size == (BUY_SELL_DEPTH - 1))
//                             {
//                                 for (int itr = buy_set[0] - 5; itr >= minMax.min; itr = itr - 5)
//                                 {
//                                     if (buy_array[itr / 5].price != 0)
//                                     {
//                                         insert(buy_set, &buy_size, buy_array[itr / 5].price, ASC);
//                                         break;
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }

//                 else if (order_type == 'S')
//                 {
//                     sell_array[cancel_price_index].quantity -= cancel_quantity;
//                     sell_array[cancel_price_index].participant--;

//                     if (sell_array[cancel_price_index].quantity == 0)
//                     {
//                         sell_array[cancel_price_index].price = 0;
//                         if (find(sell_set, &sell_size, cancel_price, DSC) != -1)
//                         {
//                             erase(sell_set, &sell_size, cancel_price, DSC);
//                             if (sell_size == (BUY_SELL_DEPTH - 1))
//                             {
//                                 for (int i = sell_set[0] + 5; i <= minMax.max; i = i + 5)
//                                 {
//                                     if (sell_array[i / 5].price != 0)
//                                     {
//                                         insert(sell_set, &sell_size, sell_array[i / 5].price, DSC);
//                                         break;
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }

//         else if (msg_type == 'Z')
//         {
//             HeartBeatBody last_sequence;

//             memcpy((char *)&last_sequence, buff + MESSAGE_HEADER_BYTES,
//                    HEARTBEAT_MESSAGE_BODY_BYTES);

//             int lst_seq_num = last_sequence.lst_seq_num;

//             // cout << "Z [info]: last sequence number: " << lst_seq_num << endl;

//             return 0;
//         }

//         else
//         {
//             // cout << "[orderbook][debug]: " << msg_type << endl;
//             // cout << "[orderbook][info]: Unhandeled message type, Exiting\n";
//             return 0;
//         }

//     b_output("\nData : B", 10);
//     for(int i = 0; i<buy_size; ++i)
//     {
//         itoa(buy_set[i]);
//         b_output(" ", 1);
//     }
//     b_output("S ", 2);
//     for(int i = 0; i<sell_size; ++i)
//     {
//         itoa(sell_set[i]);
//         b_output(" ", 1);
//     }
//     b_output("\n", 1);
//     // b_output(", data: ", 9);
//     // b_output((const char*)&msg_type, 1);
//     // if (msg_type == 'T')
//     // {
//     //     b_output(", token: ", 10);
//     //     itoa(*(int *)(data + 33));
//     // }
//     // else if (msg_type == 'N' || msg_type == 'M' || msg_type == 'X')
//     // {
//     //     b_output(", token: ", 10);
//     //     itoa(*(int *)(data + 25));
//     // }
//     // b_output("\n", 1);

// 	return buff;
// }
