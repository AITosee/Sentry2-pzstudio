/*
 * Copyright (C) 2022 Polygon Zone Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */



#include "stdio.h"

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "py/mphal.h"
#include "mpthreadport.h"
#include "los_task.h"
#include "los_hwi.h"

#if MICROPY_PY_THREAD


extern osThreadId_t *thread_list ;
extern int thread_cnt ;
extern int thread_list_len ;

#define MP_THREAD_MIN_STACK_SIZE                        (2 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 1024)
//#define MP_THREAD_PRIORITY                              (ESP_TASK_PRIO_MIN + 1)

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    osThreadId_t id;        // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    void *stack;            // pointer to the stack
    size_t stack_len;       // number of words in the stack
    struct _thread_t *next;
} thread_t;

// the mutex controls access to the linked list

MP_STATIC mp_thread_mutex_t thread_mutex;
MP_STATIC thread_t thread_entry0;
MP_STATIC thread_t *thread = NULL; // root pointer, handled by mp_thread_gc_others

void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_set_state(&mp_state_ctx.thread);
    // create the first entry in the linked list of all threads
    thread = &thread_entry0;
    thread->id = osThreadGetId();
    thread->ready = 1;
    thread->arg = NULL;
    thread->stack = stack;
    thread->stack_len = stack_len;
    thread->next = NULL;
    mp_thread_mutex_init(&thread_mutex);
	//MP_THREAD_GIL_EXIT();
}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1); // probably not needed
        if (th->id == osThreadGetId()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        gc_collect_root(th->stack, th->stack_len);
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    //return osGetThreadLocalStoragePointer(osThreadGetId());
#ifdef LITEOS_WIFI_IOT_VERSION
	osThreadId_t thread_id = osThreadGetId();
    void *pvReturn = NULL;
    LosTaskCB *pstTaskCB = NULL;
    if (OS_INT_ACTIVE || thread_id == NULL) {
        return NULL;
    }
    pstTaskCB = (LosTaskCB *)thread_id;
    pvReturn = pstTaskCB->args[3];
    return pvReturn;
#else
    return NULL;
#endif
}

void mp_thread_set_state(mp_state_thread_t *state) {
    //osSetThreadLocalStoragePointer(osThreadGetId(), state);
#ifdef LITEOS_WIFI_IOT_VERSION
	osThreadId_t thread_id = osThreadGetId();
    UINTPTR uvIntSave;
    LosTaskCB *pstTaskCB = NULL;

    if (OS_INT_ACTIVE || thread_id == NULL) {
        return ;
    }
    pstTaskCB = (LosTaskCB *)thread_id;
    uvIntSave = LOS_IntLock();

    pstTaskCB->args[3]= state;

    LOS_IntRestore(uvIntSave);
#else
#endif
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == osThreadGetId()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

MP_STATIC void *(*ext_thread_entry)(void *) = NULL;

MP_STATIC void freertos_entry(void *arg) {
	//printf("create task success \r\n");
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }
    /*
    while(1){
    	printf("create task success\r\n");
		sleep(1);
    }
    */
    osThreadTerminate(osThreadGetId());
    for (;;) {;
    }
}






void mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name) {
    // store thread entry function into a global variable so we can access it
    ext_thread_entry = entry;
	osThreadAttr_t attr = {0};
    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);
	/* 创建线程 */
    // create thread
    //BaseType_t result = xTaskCreatePinnedToCore(freertos_entry, name, *stack_size / sizeof(StackType_t), arg, priority, &th->id, MP_TASK_COREID);
    //if (result != pdPASS) {
    //    mp_thread_mutex_unlock(&thread_mutex);
    //    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
    //}

	attr.name = "mp_thread"; 
	attr.attr_bits = 0U; 
	attr.cb_mem = NULL; 
	attr.cb_size = 0U; 
	attr.stack_mem = NULL; 
	attr.stack_size = *stack_size;//堆栈大小 
	attr.priority = osPriorityNormal;//优先级 
	//osThreadId_t threadid = NULL;
	osThreadId_t threadid = osThreadNew((osThreadFunc_t)freertos_entry, arg, &attr);
	if ( threadid == NULL) {
	   	mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
	}else{
		#if 1
		thread_list[thread_cnt++] = threadid;
		if(thread_cnt == thread_list_len){
		    osThreadId_t *new_thread_list = (osThreadId_t *)malloc(sizeof(osThreadId_t)*(thread_list_len + 20));
		    for(int i=0;i<thread_list_len;i++){
		        new_thread_list[i] = thread_list[i];
		    }
		    free(thread_list);
		    thread_list = new_thread_list;
		    thread_list_len = thread_list_len + 20;
		}
		#endif
	}
#if 1
    // add thread to linked list of all threads
    th->id = threadid;
	LosTaskCB *pstTaskCB = (LosTaskCB *)(threadid);
    th->ready = 0;
    th->arg = arg;
    th->stack =  pstTaskCB->topOfStack;//获取栈指针,参考esp32 的做法
    th->stack_len = *stack_size / sizeof(uintptr_t);
    th->next = thread;
    thread = th;
    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;
#endif
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    mp_thread_create_ex(entry, arg, stack_size, osPriorityNormal, "mp_thread");
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == osThreadGetId()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

// This is called from the FreeRTOS idle task and is not within Python context,
// so MP_STATE_THREAD is not valid and it does not have the GIL.
void vPortCleanUpTCB(void *tcb) {
    if (thread == NULL) {
        // threading not yet initialised
        return;
    }
    thread_t *prev = NULL;
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; prev = th, th = th->next) {
        // unlink the node from the list
        if ((void *)th->id == tcb) {
            if (prev != NULL) {
                prev->next = th->next;
            } else {
                // move the start pointer
                thread = th->next;
            }
            // The "th" memory will eventually be reclaimed by the GC.
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    // Need a binary semaphore so a lock can be acquired on one Python thread
    // and then released on another.
    if(mutex->flag != 0xa5){
		
	    if(LOS_MuxCreate(&mutex->handle)==0){
	    //mutex->handle = xSemaphoreCreateBinaryStatic(&mutex->buffer);
	    	LOS_MuxPost(mutex->handle);
			mutex->flag = 0xa5;
		}
    }
	
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {

	if(mutex->flag == 0xa5){
		return LOS_OK == LOS_MuxPend(mutex->handle, wait ? LOS_WAIT_FOREVER : 0);
	}else{
		return LOS_OK == LOS_NOK;
	}
	
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
	if(mutex->flag == 0xa5){
    	LOS_MuxPost(mutex->handle);
	}
}



void mp_thread_sem_init(mp_thread_sem_t *sem,uint32_t max_count,uint32_t initial_count) {
    // Need a binary semaphore so a lock can be acquired on one Python thread
    // and then released on another.
    if(sem->sem_handle == NULL){
		sem->sem_handle = osSemaphoreNew(max_count,initial_count,NULL);
    }
	
}

int mp_thread_sem_acquire(mp_thread_sem_t *sem, int wait) {

	if(sem->sem_handle != NULL){
		return LOS_OK == osSemaphoreAcquire(sem->sem_handle, wait ? LOS_WAIT_FOREVER : 0);
	}else{
		return LOS_OK == LOS_NOK;
	}
	
}

void mp_thread_sem_release(mp_thread_sem_t *sem) {
	if(sem->sem_handle != NULL){
    	osSemaphoreRelease(sem->sem_handle);
	}
}

uint32_t mp_thread_sem_get_count(mp_thread_sem_t *sem){
	if(sem->sem_handle != NULL){
		return  osSemaphoreGetCount(sem->sem_handle);
	}else{
		return 0;
	}
}

void mp_thread_deinit(void) {
    for (;;) {
        // Find a task to delete
        osThreadId_t id = NULL;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (thread_t *th = thread; th != NULL; th = th->next) {
            // Don't delete the current task
            if (th->id != osThreadGetId()) {
                id = th->id;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);

        if (id == NULL) {
            // No tasks left to delete
            break;
        } else {
            // Call FreeRTOS to delete the task (it will call vPortCleanUpTCB)
            osThreadTerminate(id);
        }
    }
}

#else

void vPortCleanUpTCB(void *tcb) {
}

#endif // MICROPY_PY_THREAD