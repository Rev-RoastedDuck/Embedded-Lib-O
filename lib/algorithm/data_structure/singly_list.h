/*
 * singly_list.h
 * 
 *	Note: This is a doubly linked list.
 *        get_next_data_ptr2(...) / 
 *          get_fir_data_ptr2(...): will return data ptr.
 *                                  This means that operations on this pointer will affect the data within the linked list.
 *                                  In other words,if you obtain this pointer and modify its value,
 *                                  the data of the corresponding node in the linked list will also be modified.                                    
 *          list_copy_to(...): If you want to modify its value but don't affect the data within the linked list,you should use this api.
 *                             !!!Note that when you no longer need this variable, remember to free its memory.
 *
 *  Created on: 2024_11_21
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef ALGORITHM_DATA_STRUCTURE_SINGLY_LIST_H_
#define ALGORITHM_DATA_STRUCTURE_SINGLY_LIST_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tools.h"
#include "list_interface.h"

typedef struct __LIST_SINGLY_NODE_RRD
{
    void                                *data;
    struct __LIST_SINGLY_NODE_RRD       *next;
}LIST_SINGLY_NODE_RRD;

typedef struct __LIST_SINGLY_RRD{
    LIST_INTERFACE_RRD *interface;
    
    size_t lenth;
    size_t unit_size;
    bool is_deep_copy;
    LIST_SINGLY_NODE_RRD list;
    LIST_SINGLY_NODE_RRD *last_node;
}LIST_SINGLY_RRD;

#define list_singly_for_each(_list_ptr, _entry_ptr)                                             \
    for(LIST_SINGLY_NODE_RRD* _node = ((_list_ptr) != (NULL) ? (_list_ptr)->list.next : (NULL));    \
        (_entry_ptr)=((_node)==(NULL)?(NULL):((_node)->data)),(_node)!=(NULL);                      \
        (_node)=(_node)->next)                                

#define list_singly_for_each_safe(_list_ptr, _entry_ptr)                        \
    for (LIST_SINGLY_NODE_RRD *_node = ((_list_ptr) != NULL ? (_list_ptr)->list.next : NULL), \
         *_node_next = (_node != NULL ? _node->next : NULL);                    \
         (_node) != NULL && ((_entry_ptr) = _node->data, true);                 \
         (_node) = _node_next,                                                  \
         _node_next = (_node != NULL ? _node->next : NULL))


void singly_list_init(LIST_SINGLY_RRD* self,size_t unit_size,bool is_deep_copy);

#define OPEN_LIST_SINGLY_TEST 1
#if OPEN_LIST_SINGLY_TEST
void list_singly_test_start(void);
#endif

#endif
