#pragma once

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#define MAX_CPU	8	// 최대 CPU 개수
#define MAX_TIME_SLICE	1000

typedef struct tag_pcb
{
	struct tag_pcb *next;

	int arrival_time;	// 작업 요청이 들어온 시각
	int burst_time;		// 실제 작업에 필요한 시간
	int priority;		// 우선순위

	int pid;				// 프로세스 ID
	int start_time;			// 처리가 시작된 시각
	int consume_time;		// 현재까지 실행된 처리 시간
	int consume_time_cur;	// - 이번 quantum에서 실행된 처리 시간
	int finish_time;		// 실행이 완료된 시각
	int turnarround_time;	// 실제 동작된 시간(finish_time - start_time)
	int wait_time;			// 전체 대기 시간
	char time_status[MAX_TIME_SLICE];
} pcb_t;

typedef struct
{
	int quantum;
	int algorithm;
	int count;
	pcb_t* head;
	pcb_t* tail;
} queue_t;

/*
	running_queue
	waiting_queue
	ready_queue
	1. running -> waiting
	2. running -> ready
	3. waiting -> ready
	4. terminate
*/
typedef struct
{
	int cur_time;	// 현재 시각
	int cpu_count;	// 현재 사용중인 CPU 개수
	pcb_t* pcb_on_cpu[MAX_CPU];	// 현재 처리중인 프로세스 ID
	int quantum;
	int algorithm;
	queue_t* work_queue;		// 모든 프로세스들이 정렬된 순서로 있다.
	queue_t* ready_queue;		// CPU의 처리를 기다리는 집합
	queue_t* terminate_queue;		// CPU의 처리를 마친 프로세스들의 집합
} system_t;

// 관련 util 함수
typedef queue_t*	(*fp_queue_make_t)(void);
typedef void		(*fp_queue_init_t)(queue_t* queue);
typedef void		(*fp_queue_clear_t)(queue_t* queue);
typedef pcb_t*		(*fp_queue_find_prev_pcb_t)(queue_t* queue, pcb_t* pcb);

typedef system_t*	(*fp_system_make_t)(void);
typedef void		(*fp_system_free_t)(system_t* system);
typedef void		(*fp_system_init_t)(system_t* system);
typedef void		(*fp_system_clear_t)(system_t* system);
typedef void		(*fp_system_add_process_to_work_t)(system_t* system, int arrival_time, int burst_time, int priority);
typedef void		(*fp_system_switch_work_to_ready_t)(system_t* system);
typedef void		(*fp_system_switch_ready_to_terminate_t)(system_t* system, pcb_t* pcb);
typedef void		(*fp_system_update_after_algorithm_t)(system_t* system);
typedef void		(*fp_system_debug_status_t)(system_t* system);

typedef struct tag_sys_util_t {
	fp_queue_make_t							fp_queue_make;
	fp_queue_init_t							fp_queue_init;				
	fp_queue_clear_t						fp_queue_clear;			
	fp_queue_find_prev_pcb_t				fp_queue_find_prev_pcb;	

	fp_system_make_t						fp_system_make;			
	fp_system_free_t						fp_system_free;			
	fp_system_init_t						fp_system_init;			
	fp_system_clear_t						fp_system_clear;			
	fp_system_add_process_to_work_t			fp_system_add_process_to_work;			
	fp_system_switch_work_to_ready_t		fp_system_switch_work_to_ready;		
	fp_system_switch_ready_to_terminate_t	fp_system_switch_ready_to_terminate;	
	fp_system_update_after_algorithm_t		fp_system_update_after_algorithm;		
	fp_system_debug_status_t				fp_system_debug_status;				
} sys_util_t;

queue_t* queue_make(void);
void queue_init(queue_t* queue);
void queue_clear(queue_t* queue);
pcb_t*  queue_find_prev_pcb(queue_t* queue, pcb_t* pcb);

system_t* system_make(void);
void system_free(system_t* system);
void system_init(system_t* system);
void system_clear(system_t* system);
void system_add_process_to_work(system_t* system, int arrival_time, int burst_time, int priority);
void system_switch_work_to_ready(system_t* system);
void system_switch_ready_to_terminate(system_t* system, pcb_t* pcb);
void system_update_after_algorithm(system_t* system);
void system_debug_status(system_t* system);

// 알고리즘 DLL파일을 로드하기 위한 구조체 및 Function Point Type
typedef int (*lib_init_sys_util_t)(sys_util_t*);
typedef int (*lib_get_algorithm_name_t)(char *);
typedef int (*lib_get_next_process_t)(system_t*);

typedef struct
{
	HINSTANCE lib_handle;
	int algorithm;
	lib_init_sys_util_t fp_init_sys_util;
	lib_get_algorithm_name_t fp_get_algorithm_name;
	lib_get_next_process_t fp_get_next_process;
} algo_item_t;

/*
#ifdef __cplusplus
}
#endif
*/