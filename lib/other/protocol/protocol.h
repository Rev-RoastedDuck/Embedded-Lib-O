/*
 * protocol.h
 *	
 *  Note: 在c++编程中，不建议把缓存变量的类型改成vector，
 *          一般而言，待发送的数据结构体绑定了一个union，union包含vector是一个糟糕的选择
 *  Created on: 2024_10_29
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef PROTOCOL_MANAGER_RRD_H_
#define PROTOCOL_MANAGER_RRD_H_

/******************************************************************************/
/*---------------------------------INCLUDES-----------------------------------*/
/******************************************************************************/
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include <string.h>
#include "stdarg.h"
#include "stdlib.h"
#include "common_definition_rrd.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*-------------------------------DECLARAYIONS---------------------------------*/
/******************************************************************************/

typedef bool (*protocol_manager_pack_data_fn_t)           (void *manager,uint8_t* data,size_t size);
typedef bool (*protocol_manager_parse_packet_fn_t)        (void *manager,uint8_t* data,size_t start_index,size_t end_index);
typedef bool (*protocol_manager_check_packet_tail_fn_t)   (void *manager,uint8_t data);
typedef bool (*protocol_manager_check_packet_header_fn_t) (void *manager,uint8_t data);

typedef struct __PROTOCOL_MANAGER_INTERFACE_RRD{
    protocol_manager_pack_data_fn_t pack_data;
    protocol_manager_parse_packet_fn_t parse_packet;
    protocol_manager_check_packet_tail_fn_t check_packet_tail;
    protocol_manager_check_packet_header_fn_t check_packet_header;
}PROTOCOL_MANAGER_INTERFACE_RRD;

typedef struct __PACKET_INFO
{
    uint8_t *pack_body;         // 解析出来的数据体
    size_t pack_body_len;      // 解析出来的数据体长度
}PACKET_INFO;


typedef struct __PROTOCOL_MANAGER_RRD{

    __PUBLIC___
        PROTOCOL_MANAGER_INTERFACE_RRD* interface;

        PACKET_INFO parsed_packet_info; // 解析出来的数据体
        PACKET_INFO built_packet_info;  // 已构建的数据包

        struct {
            #define PARSE_PACKET_DEFAULT_ASYNC    1
            #define PARSE_PACKET_DEFAULT_SYNC     2
            #define is_parse_packet_way(way)                            \
                                ((way) == PARSE_PACKET_DEFAULT_ASYNC || \
                                 (way) == PARSE_PACKET_DEFAULT_SYNC)
            uint8_t parse_packet_way;       // 数据包解析方式

            struct {
                bool need_check_size;
                bool need_check_length;
            }parse_config;

            struct {
                bool need_check_size;
                bool need_check_length;
            }build_config;
        }config;

        void (*del)                         (void **self);

    __PRIVATE__
        struct {
            uint8_t *packet_header;     // 数据包头部
            uint8_t packet_header_len;  // 数据包头部长度

            uint8_t *packet_tail;       // 数据包尾部
            uint8_t packet_tail_len;    // 数据包尾部长度
        }packet_base_info;

        struct{
            uint8_t step;

            // body
            uint8_t pack_len;
            uint8_t pack_size;
            uint8_t pack_len_calcu;
            uint8_t pack_size_calcu;

            uint8_t check_tail_step;
            uint8_t check_header_step;
        }parse_packet_status;

    ____IF_____
        void (*parase_success_callback)     (uint8_t *packet,size_t length);
        void (*transmit_data_interface)     (uint8_t *packet,size_t length);

}PROTOCOL_MANAGER_RRD;

/**
 * @param pack_body: 用于存放解析出的数据体,用于关联外部数据缓存器,当pack_body = NULL时,内部根据 buff_size 分配内存;否则,使用外部内存
 * @param built_packet: 用于存放构建的的数据协议包,用于关联外部数据缓存器,built_packet = NULL时,内部根据 buff_size 分配内存;否则,使用外部内存
*/
PROTOCOL_MANAGER_RRD* protocol_manager_new(size_t buff_size,
                                            uint8_t *pack_body, uint8_t *built_packet,  // 补丁
                                            uint8_t *headers,uint8_t packet_header_len,
                                            uint8_t *tails,  uint8_t packet_tail_len,
                                            uint8_t parse_packet_way);

#define OPEN_PROTOCOL_MANAGER_TEST 0
#if OPEN_PROTOCOL_MANAGER_TEST
void protocol_manager_test_start(void);
#endif /* OPEN_PROTOCOL_MANAGER_TEST */

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_RRD_H_ */
