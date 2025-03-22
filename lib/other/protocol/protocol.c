/*
 * protocol.h
 *	
 *  Created on: 2024_10_29
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "protocol.h"

/******************************************************************************/
/*----------------------------------PRIVATE-----------------------------------*/
/******************************************************************************/
/** \addtogroup 数据包解析/构建工具
 * \{ */
/**
 * @brief  校验数据头
 * @param  数据包
 * @return None
 */
static bool __PRIVATE__ check_packet_header(PROTOCOL_MANAGER_RRD *self,uint8_t data)
{
    // 1.判断下标位is_header_count的数据包头是不是对应的数据包头，是则加一，否则清零
    self->parse_packet_status.check_header_step 
        = self->packet_base_info.packet_header[self->parse_packet_status.check_header_step] == data ?
             ++self->parse_packet_status.check_header_step : 0;

    // 2.如果校验次数位数据包头数组长度，则返回TRUE
    if(self->parse_packet_status.check_header_step == self->packet_base_info.packet_header_len){
        self->parse_packet_status.check_header_step = 0;
        return true;
    }

    // 3.没有校验完成，返回FALSE
    return false;
}

/**
 * @brief  校验数据尾
 * @param  数据包
 * @return None
 */
static bool __PRIVATE__ check_packet_tail(PROTOCOL_MANAGER_RRD *self,uint8_t data)
{
    // 1.判断下标位is_header_count的数据包头是不是对应的数据包头，是则加一，否则清零
    self->parse_packet_status.check_tail_step 
        = self->packet_base_info.packet_tail[self->parse_packet_status.check_tail_step] == data ?
            ++self->parse_packet_status.check_tail_step : 0;
    // 2.如果校验次数位数据包头数组长度，则返回TRUE
    if(self->parse_packet_status.check_tail_step == self->packet_base_info.packet_tail_len)
    {
        self->parse_packet_status.check_tail_step = 0;
        return true;
    }

    // 3.没有校验完成，返回FALSE
    return false;
}

/**
 * @brief  校验数据长度
 * @param  数据包
 * @return None
 */
static bool __PRIVATE__ check_data_length(PROTOCOL_MANAGER_RRD *self)
{
//    printf("calcu_length:%d\r\n",self->parse_packet_status.pack_len_calcu);
//    printf("desire_length:%d\r\n",self->parse_packet_status.pack_len);
    return self->parse_packet_status.pack_len_calcu 
                == self->parse_packet_status.pack_len? 
                    true : false;
}

/**
 * @brief  计算数据包大小
 * @param  数据包
 * @return None
 */
static uint8_t __PRIVATE__ cacul_packet_size(PROTOCOL_MANAGER_RRD *self,uint8_t *data,size_t len)
{
    uint8_t calcu_size = 0;

    for(size_t index = 0;index < len;index++){
        calcu_size += data[index];
    }
    return calcu_size;
}

/**
 * @brief  校验数据包大小
 * @param  数据包
 * @return None
 * @note   用uint8计算长度
 */
static bool __PRIVATE__ check_packet_size(PROTOCOL_MANAGER_RRD *self)
{
    uint8_t calcu_size = cacul_packet_size(self,
                            self->parsed_packet_info.pack_body,
                            self->parse_packet_status.pack_len_calcu);
//    printf("calcu_size:%d\r\n",calcu_size);
//    printf("desire_size:%d\r\n",self->parse_packet_status.pack_size);

    // 2.判断计算结果是否等于预期大小
    return calcu_size == (uint8_t)self->parse_packet_status.pack_size? true : false;
}

/**
 * @brief  解析数据包
 * @param  data  一字节数据
 * @return None
 * @todo
 */
static bool __PRIVATE__ parse_packet_async(PROTOCOL_MANAGER_RRD *self, uint8_t data)
{   
    // 0. 校验数据包头
    if (0 == self->parse_packet_status.step){
        self->parse_packet_status.pack_len = 0;
        self->parse_packet_status.pack_size = 0;
        self->parse_packet_status.pack_len_calcu = 0;
        self->parse_packet_status.pack_size_calcu = 0;
        self->parse_packet_status.step = check_packet_header(self,data) ?
                                         ++self->parse_packet_status.step : 0;
        return false;
    }
    // 1. 保存数据包长度
    if (1 == self->parse_packet_status.step && self->config.parse_config.need_check_length)
    {
        ++self->parse_packet_status.step;
        self->parse_packet_status.pack_len = data;
        return false;
    }

    // 2. 保存数据包大小
    if (2 == self->parse_packet_status.step && self->config.parse_config.need_check_size)
    {
        ++self->parse_packet_status.step;
        self->parse_packet_status.pack_size = data;
        return false;
    }

    if (3 == self->parse_packet_status.step)
    {
        if (!check_packet_tail(self,data))
        {
            self->parsed_packet_info.pack_body[self->parse_packet_status.pack_len_calcu++] = data;
            return false;
        }
        ++self->parse_packet_status.step;
        for(uint8_t i = 0;i < self->packet_base_info.packet_tail_len;++i){
            self->parsed_packet_info.pack_body[self->parse_packet_status.pack_len_calcu - i] = 0;
        }
        self->parse_packet_status.pack_len_calcu -= 
                    self->packet_base_info.packet_tail_len - 1; // rx_data_buff_index会加一，所以这里需要减掉.
    }

    // 4.校验数据长度
    if(self->config.parse_config.need_check_length){
        if (4 == self->parse_packet_status.step)
        {
            if (!check_data_length(self))
            {
                self->parse_packet_status.step = 0;
                return false;
            }
            ++self->parse_packet_status.step;
        }
    }else{
        ++self->parse_packet_status.step;
    }


    // 5.校验数据包大小
    if(self->config.parse_config.need_check_size){
        if (5 == self->parse_packet_status.step)
        {
            if (!check_packet_size(self))
            {
                self->parse_packet_status.step = 0;
                return false;
            }
            ++self->parse_packet_status.step;
        }
    }else{
        ++self->parse_packet_status.step;
    }
    // 6.执行服务函数
    if (6 == self->parse_packet_status.step)
    {   
        self->parsed_packet_info.pack_body_len = self->parse_packet_status.pack_len_calcu;
        self->parse_packet_status.step = 0;
        return true;
    }
    return false;
}

static bool __PRIVATE__ parse_packet_sync(PROTOCOL_MANAGER_RRD *self,
                                             uint8_t* data,
                                             size_t start_index,
                                             size_t end_index)
{   
    bool ret = false;
    for(size_t index = start_index;index < end_index;++index){
        ret = parse_packet_async(self,data[index]);
    }
    return ret;
}

static bool __PUBLIC___ parse_packet(PROTOCOL_MANAGER_RRD *self,
                                        uint8_t* data,
                                        size_t start_index,
                                        size_t end_index)
{
    bool ret = false;
    switch (self->config.parse_packet_way){
        case PARSE_PACKET_DEFAULT_ASYNC:
            ret = parse_packet_async(self,*data);
            break;
        case PARSE_PACKET_DEFAULT_SYNC:
            ret = parse_packet_sync(self,data,start_index,end_index);
            break;
        default:
            ret = false;
            break;
    }
    if(ret){
        if(NULL != self->parase_success_callback){
            self->parase_success_callback(self->parsed_packet_info.pack_body,
                                            self->parsed_packet_info.pack_body_len);
        }
    }

    return ret;
}


/**
 * @brief  构建数据包
 * @return packet   完整的数据包
 * @todo
 */
static void __PRIVATE__ build_packet(PROTOCOL_MANAGER_RRD *self,uint8_t* data,size_t len)
{
    size_t index = 0;
    // 1.填充数据包头
    for(uint8_t i = 0; i < self->packet_base_info.packet_header_len; i++){
        self->built_packet_info.pack_body[index++] = *(self->packet_base_info.packet_header + i);
    }

    // 2.填充数据长度,加上功能号
    if(self->config.build_config.need_check_length){
        self->built_packet_info.pack_body[index++] = (uint8_t)len;
    }

    // 3.填充数据大小，加上功能号的大小
    if(self->config.build_config.need_check_size){
        self->built_packet_info.pack_body[index++] = (uint8_t)(cacul_packet_size(self,
                                                                    data,
                                                                    len));
    }

    // 5.填充数据体
    for(size_t i = 0;i < len;i++){
        self->built_packet_info.pack_body[index++] = data[i];
    }

    // 6.填充数据尾
    for(uint8_t i = 0; i<self->packet_base_info.packet_tail_len; i++){
        self->built_packet_info.pack_body[index++] = *(self->packet_base_info.packet_tail + i);
    }
    
    self->built_packet_info.pack_body_len = index;
}

// unuse
#if 0
static void __PRIVATE__ set_parse_config(PROTOCOL_MANAGER_RRD *self,
                                            bool need_check_size,
                                            bool need_check_length)
{
    self->config.parse_config.need_check_size = need_check_size;
    self->config.parse_config.need_check_length = need_check_length;
}

static void __PRIVATE__ set_build_config(PROTOCOL_MANAGER_RRD *self,
                                            bool need_check_size,
                                            bool need_check_length)
{
    self->config.build_config.need_check_size = need_check_size;
    self->config.build_config.need_check_length = need_check_length;
}
#endif

static bool __PRIVATE__ set_parse_packet_way(PROTOCOL_MANAGER_RRD *self,
                                            uint8_t way)
{
    if(!is_parse_packet_way(way)){
        return false;
    }
    self->config.parse_packet_way = way;
    return true;
}
/** \endgroup 数据包解析/构建工具
 * \} */

/******************************************************************************/
/*-----------------------------------PUBLIC-----------------------------------*/
/******************************************************************************/
/** \addtogroup PROTOCOL_MANAGER_RRD
 * \{ */
static bool __PUBLIC___ set_packet_headers(PROTOCOL_MANAGER_RRD *self,
                                            uint8_t *headers,
                                            uint8_t headers_len)
{
    if(headers_len != self->packet_base_info.packet_header_len){
        if(NULL != self->packet_base_info.packet_header){
            free(self->packet_base_info.packet_header);
        }
        self->packet_base_info.packet_header = malloc(sizeof(uint8_t) * headers_len);
        if(NULL == self->packet_base_info.packet_header){
            printf("packet_header malloc failed");
            return false;
        }
        self->packet_base_info.packet_header_len = headers_len;
    }
    memcpy(self->packet_base_info.packet_header, headers, headers_len);

    return true;
}

static bool __PUBLIC___ set_packet_tails(PROTOCOL_MANAGER_RRD *self,
                                            uint8_t *tails,
                                            uint8_t tails_len)
{
    if(tails_len != self->packet_base_info.packet_tail_len){
        if(NULL != self->packet_base_info.packet_tail){
            free(self->packet_base_info.packet_tail);
        }
        self->packet_base_info.packet_tail = malloc(sizeof(uint8_t) * tails_len);
        if(NULL == self->packet_base_info.packet_tail){
            printf("packet_header malloc failed");
            return false;
        }
        self->packet_base_info.packet_tail_len = tails_len;
    }
    memcpy(self->packet_base_info.packet_tail, tails, tails_len);
    
    return true;
}

static bool __PUBLIC___ transmit_data(PROTOCOL_MANAGER_RRD *self,uint8_t* data,size_t size){
    build_packet(self,data,size);
    if(NULL != self->transmit_data_interface){
        self->transmit_data_interface(self->built_packet_info.pack_body,
                                        self->built_packet_info.pack_body_len);
    }
    return true;
}

static void __PUBLIC___ protocol_manager_del(void **self){
    if (self != NULL && *self != NULL) {
        free(((PROTOCOL_MANAGER_RRD*)*self)->packet_base_info.packet_header);
        free(((PROTOCOL_MANAGER_RRD*)*self)->packet_base_info.packet_tail);
        free(*self);
        *self = NULL;
    }
}

/** \endgroup PROTOCOL_MANAGER_RRD/构建工具
 * \} */

/******************************************************************************/
/*---------------------------------INTERFACE----------------------------------*/
/******************************************************************************/
static PROTOCOL_MANAGER_INTERFACE_RRD g_PROTOCOL_MANAGER_INTERFACE = {
    .check_packet_header = (protocol_manager_check_packet_header_fn_t)check_packet_header,
    .check_packet_tail = (protocol_manager_check_packet_tail_fn_t)check_packet_tail,
    .parse_packet = (protocol_manager_parse_packet_fn_t)parse_packet,
    .pack_data = (protocol_manager_pack_data_fn_t)transmit_data
};

PROTOCOL_MANAGER_RRD* protocol_manager_new(size_t buff_size,
                                    uint8_t *pack_body, uint8_t *built_packet,
                                    uint8_t *headers,uint8_t packet_header_len,
                                    uint8_t *tails,  uint8_t packet_tail_len,
                                    uint8_t parse_packet_way)
{
    PROTOCOL_MANAGER_RRD* self;
    self = calloc(1,sizeof(*self));
    if(NULL == self){
        printf("self malloc failed");
        goto protocol_manager_malloc_failed;
    }

    self->config.parse_packet_way = 1;

    self->config.build_config.need_check_size = false;
    self->config.build_config.need_check_length = false;

    self->config.parse_config.need_check_size = false;
    self->config.parse_config.need_check_length = false;

    
    if(!buff_size && (NULL == built_packet || NULL == pack_body)){
        printf("buff_size is zero");
        goto set_buff_size_failed;
    }

     if(NULL != built_packet){
         self->built_packet_info.pack_body = built_packet;
     }else{
         self->built_packet_info.pack_body = malloc(sizeof(uint8_t) * buff_size);
         if(NULL == self->built_packet_info.pack_body){
             goto set_built_packet_size_failed;
         }
     }

     if(NULL != pack_body){
         self->parsed_packet_info.pack_body = pack_body;
     }else{
         self->parsed_packet_info.pack_body = malloc(sizeof(uint8_t) * buff_size);
         if(NULL == self->parsed_packet_info.pack_body){
             goto set_parsed_packet_size_failed;
         }
     }

    if(packet_header_len){
        set_packet_headers(self,headers,packet_header_len);
        if(NULL == self->packet_base_info.packet_header){
            goto packet_header_malloc_failed;
        }
    }else{
        self->packet_base_info.packet_header = NULL;
    }

    if(packet_tail_len){
        set_packet_tails(self,tails,packet_tail_len);
        if(NULL == self->packet_base_info.packet_tail){
            goto packet_tail_malloc_failed;
        }
    }else{
        self->packet_base_info.packet_tail = NULL;
    }

    set_parse_packet_way(self,parse_packet_way);

    self->del = protocol_manager_del;
    self->interface = &g_PROTOCOL_MANAGER_INTERFACE;

    return self;

packet_tail_malloc_failed:
    if (self->packet_base_info.packet_tail) {
        free(self->packet_base_info.packet_tail);
    }
packet_header_malloc_failed:
    if (self->packet_base_info.packet_header) {
        free(self->packet_base_info.packet_header);
    }
set_parsed_packet_size_failed:
    if (self->parsed_packet_info.pack_body && NULL == pack_body) {
        free(self->parsed_packet_info.pack_body);
    }
set_built_packet_size_failed:
    if (self->built_packet_info.pack_body && NULL == built_packet) {
        free(self->built_packet_info.pack_body);
    }
set_buff_size_failed:
    free(self);
protocol_manager_malloc_failed:
    return NULL;
}

/******************************************************************************/
/*-----------------------------------DEBUG------------------------------------*/
/******************************************************************************/

#if OPEN_PROTOCOL_MANAGER_TEST

#include <stdio.h>

void transmit_data_interface(uint8_t *packet,
                            size_t length)
{
    printf("build success! \r\n");
    printf("length: %d \r\n",length);
    for(uint8_t i = 0;i < length;++i){
        printf("%x  ",packet[i]);
    }
    printf("\r\n");
}

void parase_success_callback(uint8_t *packet,
                            size_t length)
{
    printf("parase success! \r\n");
    printf("length: %d \r\n",length);
    for(uint8_t i = 0;i <length;++i){
        printf("%x  ",packet[i]);
    }
    printf("\r\n");

}

uint8_t tails[] = {0xEE,0xEE};
uint8_t headers[] = {0xAA,0xAA};
uint8_t data_body[] = {0x01,0x02,0x03,0x04,0x05,0x06};
uint8_t rev_data[] = {0xaa,0xaa,0x07,0x15,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0xee,0xee};
void protocol_manager_test_start(void){
    PROTOCOL_MANAGER_RRD *protocol_manager;
    protocol_manager = protocol_manager_new(128,NULL,NULL,
                                            headers,sizeof(headers),
                                            tails,sizeof(tails),
                                            PARSE_PACKET_DEFAULT_ASYNC);
    protocol_manager->parase_success_callback = parase_success_callback;
    protocol_manager->transmit_data_interface = transmit_data_interface;

    protocol_manager->config.parse_config.need_check_size = true;
    protocol_manager->config.parse_config.need_check_length = true;

    protocol_manager->config.build_config.need_check_size = true;
    protocol_manager->config.build_config.need_check_length = true;

    // transmit_data
    protocol_manager->interface->pack_data(protocol_manager,data_body,sizeof(data_body));

    // parse_packet async
    protocol_manager->config.parse_packet_way = PARSE_PACKET_DEFAULT_ASYNC;
    for(uint8_t index = 0;index < sizeof(rev_data);index++){
        protocol_manager->interface->parse_packet(protocol_manager,rev_data+index,0,0);
    }

    // parse_packet sync
    protocol_manager->config.parse_packet_way = PARSE_PACKET_DEFAULT_SYNC;
    protocol_manager->interface->parse_packet(protocol_manager,rev_data,0,sizeof(rev_data));

    // free
    protocol_manager->del((void**)&protocol_manager);
}
#endif


