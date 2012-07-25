// algo_sjf.cpp : DLL ���� ���α׷��� ���� ������ �Լ��� �����մϴ�.
//

#include "stdafx.h"
#pragma warning(disable : 4996)
#include "algo_sjf.h"
#include <stdio.h>
#include <string.h>

/*
// ������ ������ �����Դϴ�.
ALGO_SJF_API int nalgo_sjf=0;

// ������ �Լ��� �����Դϴ�.
ALGO_SJF_API int fnalgo_sjf(void)
{
	return 42;
}

// ������ Ŭ������ �������Դϴ�.
// Ŭ���� ���Ǹ� ������ algo_sjf.h�� �����Ͻʽÿ�.
Calgo_sjf::Calgo_sjf()
{
	return;
}
*/

ALGO_SJF_API int lib_get_algorithm_name(char* str_name_buf)
{
	int size = sprintf(str_name_buf, "SJF(����)");
	return size;
}

static sys_util_t sys_util;

ALGO_SJF_API int lib_init_sys_util(sys_util_t* sutil)
{
	memcpy(&sys_util, sutil, sizeof(sys_util_t));

	return 0;
}

static pcb_t* get_shortest_job(system_t* system)
{
	pcb_t* pcb = system->ready_queue->head;
	pcb_t* pcb_sj = pcb;

	while(pcb)
	{
		if (pcb_sj->burst_time > pcb->burst_time)
		{
			pcb_sj = pcb;
		}
		pcb = pcb->next;
	}
	return pcb_sj;
}

/*
	�����ٸ� �ٽ� �Լ�
	system_t* system : �ý��� ���¸� ��� �ִ� ����ü
		system.cur_time : ���� �Ҵ��ؾ� �� �ð� 
				- "1"������ ȣ��ȴٰ� �����Ѵ�. 
				- ��, ���� �ð��� 0�̸� �����ϴ� �����̰�, �������� 1�� ȣ��� ���̶� ���� �� �� �ִ�.
		system.cpu_count : ���� ������ CPU����, ���翡�� ������ 1�̶� �ǹ� ����.
		system.cpu_pid[0] : 0�� CPU�� �Ҵ�� ���μ��� ID
				- "-1" ���̸�, �Ҵ���� �ʰ� IDLE������
				- CPU������ system.cpu_count�� ������, ����� ������ 1�̶�, 0�� index�� ���
*/
ALGO_SJF_API int lib_get_next_process(system_t* system)
{
	// ���� �ý���.CPU�� �Ҵ�� ���μ����� ������, �� ���μ����� burst_time�� consume_time�� ���Ѵ�.
	// burst_time > consume_time�� ������ ����ؼ� ���� �Ҵ�� ���μ����� �����Ѵ�.
	// burst_time = consume_time�̸�, ���� ���μ����� ã�Ƽ� �Ҵ��� �ش�.
	if (system->pcb_on_cpu[0] != NULL)
	{
		if (system->pcb_on_cpu[0]->consume_time == system->pcb_on_cpu[0]->burst_time)
		{
			// ���� cpu�� �Ҵ�� pcb�� ��� �����ߴٸ�, terminateť�� �ű��.
			sys_util.fp_system_switch_ready_to_terminate(system, system->pcb_on_cpu[0]);
			// �� �������� readyť�� �ִ� pcb�� CPU�� �Ҵ��Ѵ�. (Shortest job�� ã�Ƽ� �Ҵ��Ѵ�.)
			system->pcb_on_cpu[0] = get_shortest_job(system);
		}
		/*
		else if (system->pcb_on_cpu[0]->consume_time_cur == system->quantum)
		{
			// ���� �Ҵ�� pcb�� quantum ��ŭ�� �Ҹ��ߴٸ�, ���� pcb���� CPU�� �Ҵ��Ѵ�.
			if (system->pcb_on_cpu[0]->next)
			{
				system->pcb_on_cpu[0] = system->pcb_on_cpu[0]->next;
			}
			else
			{
				system->pcb_on_cpu[0] = system->ready_queue->head;
			}
		}
		*/
	}
	else
	{
		system->pcb_on_cpu[0] = get_shortest_job(system);
	}

	return 0;
}
