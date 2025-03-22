/*
 * interval_task_scheduler.h
 *
 *  Created on: 2024_12_11
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef INTERVAL_TASK_SCHEDULER_H_
#define INTERVAL_TASK_SCHEDULER_H_

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "singly_list.h"

typedef enum uint8_t{
    task_disable = 0,
    task_enable = 1
}TASK_STATUS_RRD;

typedef struct __TASK_RRD{
    TASK_STATUS_RRD status;
    uint32_t schedule_interval;
    uint32_t next_schedule_time;
    void (*task)(void);     // 补丁
}TASK_RRD;

typedef struct __INTERVAL_TASK_SCHEDULER{
    LIST_SINGLY_RRD tasks_list;

    void     (*del)(void **self);
    bool     (*schedule_task)(struct __INTERVAL_TASK_SCHEDULER *self);

    uint32_t (*get_time_stamp_ms)(void);

}INTERVAL_TASK_SCHEDULER;

INTERVAL_TASK_SCHEDULER *interval_task_scheduler_new(TASK_RRD *tasks_list,size_t size);

/******************************************************************************/
/*-----------------------------------DEBUG------------------------------------*/
/******************************************************************************/
#define OPEN_INTERVAL_TASK_SCHEDULE_TEST 0
#if OPEN_INTERVAL_TASK_SCHEDULE_TEST
void interval_task_scheduler_test_start(void);
#endif

#endif