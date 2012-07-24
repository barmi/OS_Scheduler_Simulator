#pragma once

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#define MAX_CPU	8	// �ִ� CPU ����
#define MAX_TIME_SLICE	1000

typedef struct tag_pcb
{
	struct tag_pcb *next;

	int arrival_time;	// �۾� ��û�� ���� �ð�
	int burst_time;		// ���� �۾��� �ʿ��� �ð�
	int priority;		// �켱����

	int pid;				// ���μ��� ID
	int start_time;			// ó���� ���۵� �ð�
	int consume_time;		// ������� ����� ó�� �ð�
	int consume_time_cur;	// - �̹� quantum���� ����� ó�� �ð�
	int finish_time;		// ������ �Ϸ�� �ð�
	int turnarround_time;	// ���� ���۵� �ð�(finish_time - start_time)
	int wait_time;			// ��ü ��� �ð�
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
	int cur_time;	// ���� �ð�
	int cpu_count;	// ���� ������� CPU ����
	pcb_t* pcb_on_cpu[MAX_CPU];	// ���� ó������ ���μ��� ID
	int quantum;
	int algorithm;
	queue_t* work_queue;		// ��� ���μ������� ���ĵ� ������ �ִ�.
	queue_t* ready_queue;		// CPU�� ó���� ��ٸ��� ����
	queue_t* terminate_queue;		// CPU�� ó���� ��ģ ���μ������� ����
} system_t;

// ���� util �Լ�
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

// �˰��� DLL������ �ε��ϱ� ���� ����ü �� Function Point Type
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