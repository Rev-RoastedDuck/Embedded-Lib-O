/*
 * tools.h
 *	
 *  Created on: 2024_11_01
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef TOOLS_RRD_H_
#define TOOLS_RRD_H_

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include <string.h>
#include "stdarg.h"
#include "stdlib.h"
#include "stddef.h"

#define value_range_controller(value,min,max)  ((value) < (min)? (min) \
                                                               : ((value) > (max)? (max) \
                                                                                 : (value)))

#define array_length(array) (sizeof(array) / sizeof(array[0]))
#define container_of_offset(ptr, offset) ((void *)((char *)(ptr) - offset))
#define member_ptr(struct_ptr, offset) ((void *)((char *)(struct_ptr) + (offset)))
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

void* aligned_malloc(size_t size, size_t alignment);

#endif /* TOOLS_RRD_H_ */
