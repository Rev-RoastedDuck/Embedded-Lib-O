/*
 * list_interface.h
 *	
 *  Created on: 2024_12_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef ALGORITHM_DATA_STRUCTURE_LIST_INTERFACE_RRD_H_
#define ALGORITHM_DATA_STRUCTURE_LIST_INTERFACE_RRD_H_

#include "stdbool.h"
#include "stdint.h"

typedef void* (*list_get_fir_data_fn_t)(void *self);
typedef bool (*list_append_fn_t)(void *self,void* data);
typedef void* (*list_get_next_data_fn_t)(void *self,void* data);
typedef void* (*list_find_fn_t)(void *self,bool (*predicate)(void *data,void *context),void *context);
typedef void* (*list_remove_fn_t)(void *self,bool (*predicate)(void *data,void *context),void *context);

typedef struct __LIST_INTERFACE_RRD{
    list_find_fn_t find;
    list_append_fn_t append;
    list_remove_fn_t remove;
    list_get_fir_data_fn_t get_fir;
    list_get_next_data_fn_t get_next;
}LIST_INTERFACE_RRD;

void* list_get_fir_data_i(void *self);
bool list_append_i(void *self,void* data);
void* list_get_next_data_i(void *self,void* data);
void* list_find_i(void *self,bool (*predicate)(void *data,void *context),void *context);
void* list_remove_i(void *self,bool (*predicate)(void *data,void *context),void *context);

#endif
