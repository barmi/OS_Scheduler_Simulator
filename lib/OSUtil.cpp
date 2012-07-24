#include "stdafx.h"
#include "../include/OSType.h"

queue_t* queue_make(void)
{
	queue_t* queue = new queue_t;
	memset(queue, 0x00, sizeof(queue_t));
	queue_init(queue);

	return queue;
}

void queue_init(queue_t* queue)
{
	queue->quantum = -1;
	queue->algorithm = -1;
	queue->count = 0;
	queue->head = queue->tail = NULL;
}

void queue_clear(queue_t* queue)
{
	queue->quantum = -1;
	queue->algorithm = -1;
	queue->count = 0;
	if (queue->head)
	{
		pcb_t* pcb = queue->head;
		do
		{
			pcb_t* temp = pcb;
			pcb = pcb->next;
			delete temp;
		}
		while(pcb);

		queue->head = queue->tail = NULL;
	}
}

/*
	queue내에서 pcb의 prev를 찾는다.
	1. queue가 비었거나, queueu내에 없으면 NULL 리턴
	2. queue의 head가 바로 pcb이면, prev가 아닌 pcb를 리턴
*/
pcb_t* queue_find_prev_pcb(queue_t* queue, pcb_t* pcb)
{
	pcb_t* p = queue->head;
	if (p == NULL)
		return NULL;
	if (p == pcb)
		return p;

	while(p->next)
	{
		if (p->next == pcb)
		{
			return p;
		}
		p = p->next;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

// pcb_on_cpu의 이전할당값을 내부적으로 가지고 있는다. context switch가 발행했는 지를 판단.
static pcb_t* pcb_on_cpu_prev[MAX_CPU];

system_t* system_make(void)
{
	system_t* system = new system_t;
	memset(system, 0x00, sizeof(system_t));
	system_init(system);

	return system;
}

void system_free(system_t* system)
{
	system_clear(system);
	delete system->work_queue;
	delete system->ready_queue;
	delete system->terminate_queue;
	delete system;
}

void system_init(system_t* system)
{
	system->cur_time = -1;
	system->cpu_count = 1;

	system->work_queue = queue_make();
	system->ready_queue = queue_make();
	system->terminate_queue = queue_make();

	memset(system->pcb_on_cpu, 0x00, sizeof(system->pcb_on_cpu));
	memset(pcb_on_cpu_prev, 0x00, sizeof(pcb_on_cpu_prev));
}

void system_clear(system_t* system)
{
	system->cur_time = -1;
	queue_clear(system->work_queue);
	queue_clear(system->ready_queue);
	queue_clear(system->terminate_queue);

	memset(system->pcb_on_cpu, 0x00, sizeof(system->pcb_on_cpu));
	memset(pcb_on_cpu_prev, 0x00, sizeof(pcb_on_cpu_prev));
}

void system_add_process_to_work(system_t* system, int arrival_time, int burst_time, int priority)
{
	pcb_t* pcb = new pcb_t;
	memset(pcb, 0x00, sizeof(pcb_t));

	pcb->arrival_time = arrival_time;
	pcb->burst_time = burst_time;
	pcb->priority = priority;
	pcb->pid = system->work_queue->count;

	if (system->work_queue->head == NULL)
	{
		system->work_queue->head = pcb;
	}
	else
	{
		system->work_queue->tail->next = pcb;
	}
	system->work_queue->tail = pcb;
	++(system->work_queue->count);
}

/*
	work큐에서 현재 시스템시각과 일치하는 PCB를 ready큐로 이동한다.
*/
void system_switch_work_to_ready(system_t* system)
{
	pcb_t* pcb = system->work_queue->head;

	// work queue가 비어있으면 그냥 종료한다.
	if (pcb == NULL)
		return;

	while (pcb && (pcb->arrival_time == system->cur_time))
	{
		// 시스템의 현재시각 기준으로 도착한 pcb가 있으면 ready큐로 옮긴다.

		// ready큐 head, tail 정리, ready큐에 pcb를 추가
		if (system->ready_queue->head == NULL)
		{
			system->ready_queue->head = pcb;
		}
		else
		{
			system->ready_queue->tail->next = pcb;
		}
		system->ready_queue->tail = pcb;

		// work 큐 head, tail 정리, work큐의 head를 삭제
		system->work_queue->head = pcb->next;
		if (pcb->next == NULL)
		{
			system->work_queue->tail = NULL;
		}

		// 마지막으로 ready큐의 마지막에 추가된 pcb의 next를 정리, 그전에 다음 pcb로 이동해 둠
		// 아래의 두 순서가 중요!!
		pcb = pcb->next;
		system->ready_queue->tail->next = NULL;
	}
}

/*
	ready큐에 있는 pcb를 terminate큐로 이동한다.
*/
void system_switch_ready_to_terminate(system_t* system, pcb_t* pcb)
{
	// ready큐에서 pcb를 찾는다. (next조정때문에 필요)
	pcb_t* prev = queue_find_prev_pcb(system->ready_queue, pcb);

	if (prev == NULL)
		return;

	// terminate큐 head, tail 정리, terminate큐에 pcb추가
	if (system->terminate_queue->head == NULL)
	{
		system->terminate_queue->head = pcb;
	}
	else
	{
		system->terminate_queue->tail->next = pcb;
	}
	system->terminate_queue->tail = pcb;

	// ready큐 head, tail 정리, ready큐에서 pcb삭제
	if (system->ready_queue->head == pcb)
	{
		system->ready_queue->head = pcb->next;
		if (system->ready_queue->head == NULL)
			system->ready_queue->tail = NULL;
	}
	else
	{
		prev->next = pcb->next;
		if (prev->next == NULL)
			system->ready_queue->tail = prev;
	}

	// 마지막으로 pcb->next 정리
	pcb->next = NULL;

	// terminate큐의 후처리
	pcb->finish_time = system->cur_time-1;
}

/*
	CPU의 모든 스케줄링이 끝나고, PCB내부의 time값들을 처리한다.
*/
void system_update_after_algorithm(system_t* system)
{
	// ready큐에서 
	//		동작중인 pcb는 consume_time을 증가, (start_time도 update)
	//		대기 중인 pcb는 wait_time을 증가,
	pcb_t* pcb = system->ready_queue->head;
	
	// cpu에 할당된 pcb가 변경됐으면 context switch발생
	if (system->pcb_on_cpu[0] != pcb_on_cpu_prev[0])
	{
		if (system->pcb_on_cpu[0])
			system->pcb_on_cpu[0]->consume_time_cur = 0;
		pcb_on_cpu_prev[0] = system->pcb_on_cpu[0];
	}

	while (pcb)
	{
		if (system->pcb_on_cpu[0] == pcb)
		{
			if (pcb->consume_time == 0)
				pcb->start_time = system->cur_time;
			++pcb->consume_time;
			++pcb->consume_time_cur;
			if (system->cur_time < MAX_TIME_SLICE)
			{
				pcb->time_status[system->cur_time] = 'B';	// burst
			}
		}
		else
		{
			++pcb->wait_time;
			if (system->cur_time < MAX_TIME_SLICE)
			{
				pcb->time_status[system->cur_time] = 'W';	// wait
			}
		}
		pcb = pcb->next;
	}

	pcb = system->work_queue->head;
	while (pcb)
	{
		if (system->cur_time < MAX_TIME_SLICE)
		{
			pcb->time_status[system->cur_time] = '-';	// not arrived
		}
		pcb = pcb->next;
	}

	pcb = system->terminate_queue->head;
	while (pcb)
	{
		if (system->cur_time < MAX_TIME_SLICE)
		{
			pcb->time_status[system->cur_time] = '.';	// terminated
		}
		pcb = pcb->next;
	}
}

void system_debug_status(system_t* system)
{
	OutputDebugString(L"===============================================\n");
	TCHAR str[256];
	wsprintf(str, L"time : %d\n", system->cur_time);
	OutputDebugString(str);
	OutputDebugString(L"Ready Queue\n--------------------------------\n");

	pcb_t* pcb = system->ready_queue->head;
	while(pcb)
	{
		wsprintf(str, L"PID : %d%s\n", pcb->pid, (system->pcb_on_cpu[0] == pcb) ? L"[*]" : L"[ ]");
		OutputDebugString(str);
		wsprintf(str, L"start : %d\n", pcb->start_time);
		OutputDebugString(str);
		wsprintf(str, L"consume : %d\n", pcb->consume_time);
		OutputDebugString(str);
		wsprintf(str, L"wait : %d\n", pcb->wait_time);
		OutputDebugString(str);
		OutputDebugString(L"-----\n");

		memset(str, 0x00, sizeof(str));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pcb->time_status, strlen(pcb->time_status), str, 256);
		OutputDebugString(L"GANT : ");
		OutputDebugString(str);
		OutputDebugString(L"\n");

		pcb = pcb->next;
	}
}