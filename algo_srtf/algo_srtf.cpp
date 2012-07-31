// algo_srtf.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#pragma warning(disable : 4996)
#include "algo_srtf.h"
#include <stdio.h>
#include <string.h>

/*
// 내보낸 변수의 예제입니다.
ALGO_SRTF_API int nalgo_srtf=0;

// 내보낸 함수의 예제입니다.
ALGO_SRTF_API int fnalgo_srtf(void)
{
	return 42;
}

// 내보낸 클래스의 생성자입니다.
// 클래스 정의를 보려면 algo_srtf.h를 참조하십시오.
Calgo_srtf::Calgo_srtf()
{
	return;
}
*/


ALGO_SRTF_API int lib_get_algorithm_name(char* str_name_buf)
{
	int size = sprintf(str_name_buf, "SJF(선점-SRTF)");
	return size;
}

static sys_util_t sys_util;

ALGO_SRTF_API int lib_init_sys_util(sys_util_t* sutil)
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
		if ((pcb_sj->burst_time-pcb_sj->consume_time) > (pcb->burst_time-pcb->consume_time))
		{
			pcb_sj = pcb;
		}
		pcb = pcb->next;
	}
	return pcb_sj;
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
ALGO_SRTF_API int lib_get_next_process(system_t* system)
{
	// 현재 시스템.CPU에 할당된 프로세스가 있으면, 그 프로세스의 burst_time과 consume_time을 비교한다.
	// burst_time > consume_time인 동안은 계속해서 현재 할당된 프로세스가 동작한다.
	// burst_time = consume_time이면, 다음 프로세스를 찾아서 할당해 준다.
	if (system->pcb_on_cpu[0] != NULL)
	{
		if (system->pcb_on_cpu[0]->consume_time == system->pcb_on_cpu[0]->burst_time)
		{
			// 현재 cpu에 할당된 pcb가 모두 소진했다면, terminate큐로 옮긴다.
			sys_util.fp_system_switch_ready_to_terminate(system, system->pcb_on_cpu[0]);
			// 그 다음에는 ready큐에 있는 pcb를 CPU에 할당한다. (Shortest job을 찾아서 할당한다.)
			system->pcb_on_cpu[0] = get_shortest_job(system);
		}
		else if (system->pcb_on_cpu[0]->consume_time_cur == system->quantum)
		{
			// 현재 할당된 pcb가 quantum 만큼을 소모했다면, 다음으로 job이 적게 남은 pcb에게 CPU를 할당한다.
			system->pcb_on_cpu[0] = get_shortest_job(system);
			/* context switching이 일어나지 않을 수도 있다. 
			현재 진행 중이던 pcb가 다른 pcb보다 작업이 적게 남아 있으면 계속 수행하기 때문이다.
			*/
		}
	}
	else
	{
		system->pcb_on_cpu[0] = get_shortest_job(system);
	}

	return 0;
}
