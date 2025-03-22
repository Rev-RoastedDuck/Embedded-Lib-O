/*
 * singly_list.c
 *	
 *  Created on: 2024_11_21
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "singly_list.h"

/** \addtogroup  LIST_ITEM
 * \{ */
/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static void item_del(LIST_SINGLY_NODE_RRD **self)
{
    if (self != NULL && *self != NULL){
        free((*self)->data);
        free(*self);
        *self = NULL;
    }
}

static LIST_SINGLY_NODE_RRD *item_new(LIST_SINGLY_RRD *self,void *data)
{
    if(NULL == data){
        return NULL;
    }
    LIST_SINGLY_NODE_RRD *item;
    item = malloc(sizeof(*item));
    if(NULL == item){
        return NULL;
    }
    item->next = NULL;

    if(self->is_deep_copy){
        item->data = malloc(self->unit_size);
        if(NULL == item->data){
            return NULL;
        }
        memcpy(item->data, data, self->unit_size);
    }else{
        item->data = data;
    }
    return item;
}
/** \} */

/** \addtogroup  LIST
 * \{ */
/******************************************************************************/
/*----------------------------------FUNCTION----------------------------------*/
/******************************************************************************/
static bool list_append(LIST_SINGLY_RRD *self,void *data)
{
    if(NULL == self || NULL == data){
        return false;
    }

    LIST_SINGLY_NODE_RRD *node = item_new(self,data);
    if(NULL == node){
        return false;
    }
    
    self->last_node->next = node;
    self->last_node = node;
    self->lenth++;

    return true;
}

static void* list_get_fir_data(LIST_SINGLY_RRD *self){
    return NULL == self->list.next ? NULL : self->list.next->data;
}

static void* list_get_next_data(LIST_SINGLY_RRD *list_ptr,void* data_ptr){
    // for(LIST_SINGLY_NODE_RRD* node = list_ptr->list.next;node->data;)

    return NULL;
}

static void* list_find(LIST_SINGLY_RRD *self,bool (*predicate)(void *data,void *context),void *context)
{
    LIST_SINGLY_NODE_RRD *current = self->list.next;
    while (NULL != current) {
        if (predicate(current->data,context)) {
            if(self->is_deep_copy){

            }
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}

static void* list_remove(LIST_SINGLY_RRD *self,bool (*predicate)(void *data,void *context),void *context){
    LIST_SINGLY_NODE_RRD *current = self->list.next;
    LIST_SINGLY_NODE_RRD *previous = NULL;
    while (NULL != current) {
        if (predicate(current->data,context)) {
            if (NULL == previous) {
                self->list.next = current->next;
            } else {
                previous->next = current->next;
            }
            if (current == self->last_node) {
                self->last_node = previous ? previous : &self->list;
            }
            self->lenth--;
            current->next = NULL;

            if(self->is_deep_copy){
                item_del((LIST_SINGLY_NODE_RRD**)&current);
                return NULL;
            }else{
                return current->data;
            }
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

static LIST_INTERFACE_RRD g_LIST_INTERFACE = {
    .find = (list_find_fn_t)list_find,
    .remove = (list_remove_fn_t)list_remove,
    .append = (list_append_fn_t)list_append,
    .get_fir = (list_get_fir_data_fn_t)list_get_fir_data,
    .get_next = (list_get_next_data_fn_t)list_get_next_data,
};

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
void singly_list_init(LIST_SINGLY_RRD* self,size_t unit_size,bool is_deep_copy)
{
    self->lenth = 0;
    self->unit_size = unit_size;
    self->last_node = &self->list;
    self->is_deep_copy = is_deep_copy;
    self->interface = &g_LIST_INTERFACE;
}
/** \} */

/******************************************************************************/
/*-----------------------------------DEBUG------------------------------------*/
/******************************************************************************/
#if OPEN_LIST_SINGLY_TEST
#include <stdio.h>

typedef struct {
    float data;
} Test_Item;

bool match(void *data,void *context){
    Test_Item* item = (Test_Item*)data;
    Test_Item* item_context = (Test_Item*)context;
    if(item->data > item_context->data){
        return true;
    }
    return false;
}

static void list_print(LIST_SINGLY_RRD *list) {
    LIST_SINGLY_NODE_RRD *current = list->list.next;
    while (current != NULL) {
        Test_Item *item = (Test_Item *)(current->data);
        printf("%f %p    ", item->data,item);
        current = current->next;
    }
    printf("\n");
}

void new_item(LIST_SINGLY_RRD *list,float data){
    Test_Item *item;
    item = malloc(sizeof(*item));
    item->data = data;
    printf("%p \r\n",item);
    list_append(list,item);
}

void list_singly_test_shallow_copy(void){
    // shallow_copy
    LIST_SINGLY_RRD list;
    singly_list_init(&list,0,false);

    // add 
    Test_Item item_1 = {.data = 1.1};
    Test_Item item_2 = {.data = 2.2};
    Test_Item item_3 = {.data = 3.3};
    Test_Item item_4 = {.data = 4.4};
    list_append(&list,&item_1);
    list_append(&list,&item_2);
    list_append(&list,&item_3);
    list_append(&list,&item_4);

    // If you don't want the parameters to be released when leaving the scope, 
    // you should use malloc to allocate memory and keep them.// Meanwhile, when you use `list_remove_i`, you should free the memory that was allocated.
    // new_item(&list,1.1);
    // new_item(&list,2.2);
    // new_item(&list,3.3);
    // new_item(&list,4.4);

    list_print(&list);

    // find
    Test_Item match_item = {
        .data = 4
    };
    Test_Item* item = list_find_i(&list,match,&match_item);
    printf("find: %f \r\n",item->data);

    // remove
    item = list_remove_i(&list,match,&match_item);
    // free(item); // free
    list_print(&list);
    
    // get_fir
    item = list_get_fir_data_i(&list);
    printf("get_fir: %f \r\n",item->data);

    // change data
    item->data = 999;
    printf("item_1_data: %f \r\n",item_1.data); // item_1.data changed


    // for_each
    list_singly_for_each(&list,item){
        printf("%f \r\n",item->data);
    }

    new_item(&list,4.4);

    list_print(&list);
}

void list_singly_test_deep_copy(void){
    // deep_copy
    LIST_SINGLY_RRD list;
    singly_list_init(&list,sizeof(Test_Item),true);

    // add 
    Test_Item item_1 = {.data = 1.1};
    Test_Item item_2 = {.data = 2.2};
    Test_Item item_3 = {.data = 3.3};
    Test_Item item_4 = {.data = 4.4};
    list_append(&list,&item_1);
    list_append(&list,&item_2);
    list_append(&list,&item_3);
    list_append(&list,&item_4);

    list_print(&list);

    // find
    Test_Item match_item = {
        .data = 4
    };
    Test_Item* item = list_find_i(&list,match,&match_item);
    printf("find: %f \r\n",item->data);

    // remove
    item = list_remove_i(&list,match,&match_item);
    free(item); // free
    list_print(&list);
    
    // get_fir
    item = list_get_fir_data_i(&list);
    printf("get_fir: %f \r\n",item->data);

    // change data
    item->data = 999;
    printf("item_1_data: %f \r\n",item_1.data); // item_1.data no change

    // for_each
    list_singly_for_each(&list,item){
        printf("%f \r\n",item->data);
    }

    new_item(&list,4.4);

    list_print(&list);
}

void list_singly_test_start(void){
    // list_singly_test_shallow_copy();
    list_singly_test_deep_copy();
}
#endif

