/*
 * list_interface.c
 *	
 *  Created on: 2024_12_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
#include "list_interface.h"


inline void* list_get_fir_data_i(void *self){
    return (*(LIST_INTERFACE_RRD **)self)->get_fir(self);
}

inline bool list_append_i(void *self,void* data){
    return (*(LIST_INTERFACE_RRD **)self)->append(self,data);
}

inline void* list_get_next_data_i(void *self,void* data){
    return (*(LIST_INTERFACE_RRD **)self)->get_next(self,data);
}

inline void* list_find_i(void *self,bool (*predicate)(void *data,void *context),void *context){
    return (*(LIST_INTERFACE_RRD **)self)->find(self,predicate,context);
}

inline void* list_remove_i(void *self,bool (*predicate)(void *data,void *context),void *context){
    return (*(LIST_INTERFACE_RRD **)self)->remove(self,predicate,context);
}
