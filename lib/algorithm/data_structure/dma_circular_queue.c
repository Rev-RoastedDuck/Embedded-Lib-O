/*
 * circular_queue.c
 *	
 *  Created on: 2024_10_24
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "dma_circular_queue.h"

/******************************************************************************/
/*----------------------------------FUNCTION----------------------------------*/
/******************************************************************************/
static size_t circular_queue_get_length(DMA_CIRCULAR_QUEUE_RRD *self){
    return self->is_full? self->capacity : ((self->capacity + self->rear - self->front) % self->capacity);
}

static bool circular_queue_is_empty(DMA_CIRCULAR_QUEUE_RRD *self){
    return self->rear == self->front && !self->is_full;
}

static void circular_queue_enqueue(DMA_CIRCULAR_QUEUE_RRD *self,void *value){
    void *data_pos = (void *)((char *)self->data + self->unit_size * self->rear);
    memcpy(data_pos, value, self->unit_size);

    self->rear = (self->rear + 1) % self->capacity;

    if(self->rear == self->front || self->is_full){
        self->is_full = true;
        self->front = self->rear;
    }
}

static void circular_queue_dequeue(DMA_CIRCULAR_QUEUE_RRD *self,void *value){
    if(circular_queue_is_empty(self)){
        return;
    }

    void *data_pos = (char *)self->data + (self->front * self->unit_size);
    memcpy(value, data_pos, self->unit_size);

    self->is_full = false;
    self->front = (self->front + 1) % self->capacity;
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static void circular_queue_del(void **self){
    if (self != NULL && *self != NULL) {
        free(((DMA_CIRCULAR_QUEUE_RRD*)*self)->data);
        free(*self);
        *self = NULL;
    }
}

static CIRCULAR_QUEUE_INTERFACE_RRD g_DAM_CIRCULAR_QUEUE_INTERFACE = {
    .dequeue = (circular_queue_dequeue_fn_t)circular_queue_dequeue,
    .enqueue = (circular_queue_enqueue_fn_t)circular_queue_enqueue,
    .is_empty = (circular_queue_is_empty_fn_t)circular_queue_is_empty,
    .get_lenth = (circular_queue_get_lenth_fn_t)circular_queue_get_length,
};

void dma_circular_queue_init(DMA_CIRCULAR_QUEUE_RRD * self,uint32_t buff_lenth,uint8_t unit_size,size_t buff_alignment)
{
    self->data = buff_alignment? aligned_malloc((buff_lenth) * unit_size, buff_alignment)
                                : (uint8_t*)malloc((buff_lenth) * unit_size);
    if(NULL == self->data){
        return;
    }

    self->rear = 0;
    self->front = 0;
    self->unit_size = unit_size;
    self->capacity = buff_lenth;

    self->del = circular_queue_del;
    self->interface = &g_DAM_CIRCULAR_QUEUE_INTERFACE;
}
