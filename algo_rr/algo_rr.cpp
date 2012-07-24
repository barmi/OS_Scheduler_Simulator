// algo_rr.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#pragma warning(disable : 4996)
#include "algo_rr.h"
#include <stdio.h>
#include <string.h>

/*
// 내보낸 변수의 예제입니다.
ALGO_RR_API int nalgo_rr=0;

// 내보낸 함수의 예제입니다.
ALGO_RR_API int fnalgo_rr(void)
{
	return 42;
}

// 내보낸 클래스의 생성자입니다.
// 클래스 정의를 보려면 algo_rr.h를 참조하십시오.
Calgo_rr::Calgo_rr()
{
	return;
}
*/

ALGO_RR_API int lib_get_algorithm_name(char* str_name_buf)
{
	int size = sprintf(str_name_buf, "RR(선점)");
	return size;
}

static sys_util_t sys_util;

ALGO_RR_API int lib_init_sys_util(sys_util_t* sutil)
{
	memcpy(&sys_util, sutil, sizeof(sys_util_t));

	return 0;
}

/*
	스케줄링 핵심 함수
	system_t* system : 시스템 상태를 담고 있는 구조체
		system.cur_time : 현재 할당해야 할 시각 
				- "1"단위로 호출된다고 가정한다. 
				- 즉, 현재 시각이 0이면 시작하는 시점이고, 다음에는 1이 호출될 것이라 예상 할 수 있다.
		system.cpu_count : 동작 가능한 CPU개수, 현재에는 무조건 1이라 의미 없음.
		system.cpu_pid[0] : 0번 CPU에 할당된 프로세스 ID
				- "-1" 값이면, 할당되지 않고 IDLE상태임
				- CPU개수는 system.cpu_count에 있지만, 현재는 무조건 1이라, 0번 index만 사용
*/
ALGO_RR_API int lib_get_next_process(system_t* system)
{
	// 현재 시스템.CPU에 할당된 프로세스가 있으면, 그 프로세스의 burst_time과 consume_time을 비교한다.
	// burst_time > consume_time인 동안은 계속해서 현재 할당된 프로세스가 동작한다.
	// burst_time = consume_time이면, 다음 프로세스를 찾아서 할당해 준다.
	if (system->pcb_on_cpu[0] != NULL)
	{
		if (system->pcb_on_cpu[0]->consume_time == system->pcb_on_cpu[0]->burst_time)
		{
			pcb_t * pcb_next = system->pcb_on_cpu[0]->next;

			// 현재 cpu에 할당된 pcb가 모두 소진했다면, terminate큐로 옮긴다.
			sys_util.fp_system_switch_ready_to_terminate(system, system->pcb_on_cpu[0]);
			// 그 다음에는 ready큐에 있는 pcb를 CPU에 할당한다.
			system->pcb_on_cpu[0] = pcb_next;
			if (system->pcb_on_cpu[0] == NULL)
				system->pcb_on_cpu[0] = system->ready_queue->head;
		}
		else if (system->pcb_on_cpu[0]->consume_time_cur == system->quantum)
		{
			// 현재 할당된 pcb가 quantum 만큼을 소모했다면, 다음 pcb에게 CPU를 할당한다.
			if (system->pcb_on_cpu[0]->next)
			{
				system->pcb_on_cpu[0] = system->pcb_on_cpu[0]->next;
			}
			else
			{
				system->pcb_on_cpu[0] = system->ready_queue->head;
			}
		}
	}
	else
	{
		system->pcb_on_cpu[0] = system->ready_queue->head;
	}

	return 0;
}
