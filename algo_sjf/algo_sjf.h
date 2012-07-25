// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// ALGO_SJF_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef ALGO_SJF_EXPORTS
#define ALGO_SJF_API __declspec(dllexport)
#else
#define ALGO_SJF_API __declspec(dllimport)
#endif

#include "../include/OSType.h"

/*
// �� Ŭ������ algo_sjf.dll���� ������ ���Դϴ�.
class ALGO_SJF_API Calgo_sjf {
public:
	Calgo_sjf(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern ALGO_SJF_API int nalgo_sjf;

ALGO_SJF_API int fnalgo_sjf(void);
*/
#ifdef __cplusplus
extern "C" {
#endif

ALGO_SJF_API int lib_init_sys_util(sys_util_t* sys_util);
ALGO_SJF_API int lib_get_algorithm_name(char* str_name_buf);
ALGO_SJF_API int lib_get_next_process(system_t* system);

#ifdef __cplusplus
}
#endif
