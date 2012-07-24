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
	queue������ pcb�� prev�� ã�´�.
	1. queue�� ����ų�, queueu���� ������ NULL ����
	2. queue�� head�� �ٷ� pcb�̸�, prev�� �ƴ� pcb�� ����
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

// pcb_on_cpu�� �����Ҵ簪�� ���������� ������ �ִ´�. context switch�� �����ߴ� ���� �Ǵ�.
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
	workť���� ���� �ý��۽ð��� ��ġ�ϴ� PCB�� readyť�� �̵��Ѵ�.
*/
void system_switch_work_to_ready(system_t* system)
{
	pcb_t* pcb = system->work_queue->head;

	// work queue�� ��������� �׳� �����Ѵ�.
	if (pcb == NULL)
		return;

	while (pcb && (pcb->arrival_time == system->cur_time))
	{
		// �ý����� ����ð� �������� ������ pcb�� ������ readyť�� �ű��.

		// readyť head, tail ����, readyť�� pcb�� �߰�
		if (system->ready_queue->head == NULL)
		{
			system->ready_queue->head = pcb;
		}
		else
		{
			system->ready_queue->tail->next = pcb;
		}
		system->ready_queue->tail = pcb;

		// work ť head, tail ����, workť�� head�� ����
		system->work_queue->head = pcb->next;
		if (pcb->next == NULL)
		{
			system->work_queue->tail = NULL;
		}

		// ���������� readyť�� �������� �߰��� pcb�� next�� ����, ������ ���� pcb�� �̵��� ��
		// �Ʒ��� �� ������ �߿�!!
		pcb = pcb->next;
		system->ready_queue->tail->next = NULL;
	}
}

/*
	readyť�� �ִ� pcb�� terminateť�� �̵��Ѵ�.
*/
void system_switch_ready_to_terminate(system_t* system, pcb_t* pcb)
{
	// readyť���� pcb�� ã�´�. (next���������� �ʿ�)
	pcb_t* prev = queue_find_prev_pcb(system->ready_queue, pcb);

	if (prev == NULL)
		return;

	// terminateť head, tail ����, terminateť�� pcb�߰�
	if (system->terminate_queue->head == NULL)
	{
		system->terminate_queue->head = pcb;
	}
	else
	{
		system->terminate_queue->tail->next = pcb;
	}
	system->terminate_queue->tail = pcb;

	// readyť head, tail ����, readyť���� pcb����
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

	// ���������� pcb->next ����
	pcb->next = NULL;

	// terminateť�� ��ó��
	pcb->finish_time = system->cur_time-1;
}

/*
	CPU�� ��� �����ٸ��� ������, PCB������ time������ ó���Ѵ�.
*/
void system_update_after_algorithm(system_t* system)
{
	// readyť���� 
	//		�������� pcb�� consume_time�� ����, (start_time�� update)
	//		��� ���� pcb�� wait_time�� ����,
	pcb_t* pcb = system->ready_queue->head;
	
	// cpu�� �Ҵ�� pcb�� ��������� context switch�߻�
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