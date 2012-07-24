// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// ALGO_FCFS_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef ALGO_FCFS_EXPORTS
#define ALGO_FCFS_API __declspec(dllexport)
#else
#define ALGO_FCFS_API __declspec(dllimport)
#endif

#include "../include/OSType.h"

/*
// �� Ŭ������ algo_fcfs.dll���� ������ ���Դϴ�.
class ALGO_FCFS_API Calgo_fcfs {
public:
	Calgo_fcfs(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern ALGO_FCFS_API int nalgo_fcfs;
*/
#ifdef __cplusplus
extern "C" {
#endif

ALGO_FCFS_API int lib_init_sys_util(sys_util_t* sys_util);
ALGO_FCFS_API int lib_get_algorithm_name(char* str_name_buf);
ALGO_FCFS_API int lib_get_next_process(system_t* system);

#ifdef __cplusplus
}
#endif
