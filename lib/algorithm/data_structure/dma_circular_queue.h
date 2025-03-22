/*
 * dma_circular_queue.h
 *
 * Note: This queue based on contiguous memory can store variables of any type.
 *       You should specify the size of queue before creating queue.
 *
 *  Created on: 2024_10_24
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef RRD_CODE_DEVICES_DMA_CIRCULAR_QUEUE_RRD_H_
#define RRD_CODE_DEVICES_DMA_CIRCULAR_QUEUE_RRD_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"

#include "tools.h"

typedef bool    (*circular_queue_is_empty_fn_t)(void *self);
typedef size_t  (*circular_queue_get_lenth_fn_t)(void *self);
typedef void    (*circular_queue_enqueue_fn_t)(void *self,void *value);
typedef void    (*circular_queue_dequeue_fn_t)(void *self,void *value);
typedef size_t  (*circular_queue_batch_dequeue_fn_t)(void *self, void *buff, uint32_t max_size);
typedef void    (*circular_queue_batch_enqueue_fn_t)(void *self, void *buff, uint32_t size);

typedef struct __CIRCULAR_QUEUE_INTERFACE_RRD{
    circular_queue_enqueue_fn_t enqueue;
    circular_queue_dequeue_fn_t dequeue;
    circular_queue_is_empty_fn_t is_empty;
    circular_queue_get_lenth_fn_t get_lenth;
    circular_queue_batch_dequeue_fn_t batch_dequeue;
    circular_queue_batch_enqueue_fn_t batch_enqueue;
}CIRCULAR_QUEUE_INTERFACE_RRD;

typedef struct __DMA_CIRCULAR_QUEUE_RRD{
    CIRCULAR_QUEUE_INTERFACE_RRD *interface;
    void (*del)     	(void **self);

    void *data;             // Data storage container
    uint32_t front;         // Head node (no data stored here)
    uint32_t rear;          // Tail node (no data stored here)
    bool is_full;           // is_full
    uint32_t dma_wrap_count;//
    uint32_t capacity;      // Actual queue length (including head and tail nodes)
    uint8_t unit_size;      // Size of a single data unit
}DMA_CIRCULAR_QUEUE_RRD;

#define dma_circular_queue_for_each(_queue_ptr,_entry_ptr) \
        for(size_t _i = 0,_max_length = (_queue_ptr)->interface->get_lenth((_queue_ptr)); \
            (_i) < (_max_length) \
                            && ((_entry_ptr) = (((char *)_queue_ptr->data + _queue_ptr->front * _queue_ptr->unit_size)),true) \
                            && ((_queue_ptr)->front = ((_queue_ptr)->front + 1) % (_queue_ptr)->capacity, true) \
                            && ((_queue_ptr)->is_full = false, true); \
            (++(_i)))
    
void dma_circular_queue_init(DMA_CIRCULAR_QUEUE_RRD *self,uint32_t buff_lenth,uint8_t unit_size,size_t buff_alignment);

#endif
