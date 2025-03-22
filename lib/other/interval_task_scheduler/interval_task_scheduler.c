/*
 * interval_task_scheduler.c
 *	
 *  Created on: 2024_12_14
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "interval_task_scheduler.h"

static bool interval_task_schedule_task(INTERVAL_TASK_SCHEDULER *self){
    uint32_t time_stamp_ms = self->get_time_stamp_ms();
    TASK_RRD *task;
    list_singly_for_each(&self->tasks_list,task){
        if(task->status == task_disable){
            continue;
        }
        if(time_stamp_ms >= ((task)->next_schedule_time)){
            (task)->task();
            (task)->next_schedule_time = time_stamp_ms + (task)->schedule_interval;
        }
    }
    return true;
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static void interval_task_del(void **self){
    if (self != NULL && *self != NULL){
        free(*self);
        *self = NULL;
    }
}

INTERVAL_TASK_SCHEDULER *interval_task_scheduler_new(TASK_RRD *tasks_list,size_t size){
    INTERVAL_TASK_SCHEDULER *self;
    self = malloc(sizeof(*self));
    if(NULL == self){
        goto self_malloc_faild;
    }

    singly_list_init(&self->tasks_list,sizeof(TASK_RRD),false);

    for(size_t i = 0;i < size;++i){
        if(NULL == tasks_list || 
            NULL == tasks_list->task){
            goto list_append_failed;
        }
        if(!list_append_i(&self->tasks_list,tasks_list++)){
            goto list_append_failed;
        }
    }
    self->del = interval_task_del;
    self->schedule_task = interval_task_schedule_task;

    return self;

list_append_failed:
    free(self);
self_malloc_faild:
    return NULL;
}

/******************************************************************************/
/*-----------------------------------DEBUG------------------------------------*/
/******************************************************************************/
#if OPEN_INTERVAL_TASK_SCHEDULE_TEST

static uint32_t get_mock_time_stamp_ms(void) {
    static uint32_t timestamp = 0;
    timestamp += 100;
    return timestamp;
}

static bool task1(void) {
    printf("Task 1 executed\n");
    return true;
}

static bool task2(void) {
    printf("Task 2 executed\n");
    return true;
}

void interval_task_scheduler_test_start(void) {
    TASK_RRD tasks[] = {
        {.schedule_interval = 500, 0, task1},  // 任务1每500ms执行一次
        {.schedule_interval = 300, 0, task2}   // 任务2每300ms执行一次
    };

    // 创建任务调度器
    INTERVAL_TASK_SCHEDULER *scheduler = interval_task_scheduler_new(tasks, 2);
    if (scheduler == NULL) {
        printf("Scheduler creation failed\n");
        return;
    }

    // 设置调度器的时间戳获取函数
    scheduler->get_time_stamp_ms = get_mock_time_stamp_ms;

    // 模拟调度任务
    printf("Starting task scheduler test...\n");
    for (int i = 0; i < 10; i++) {  // 模拟10次调度周期
        printf("Cycle %d:\n", i + 1);
        scheduler->schedule_task(scheduler);
    }

    // 释放调度器
    scheduler->del((void**)&scheduler);
}
#endif


