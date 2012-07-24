// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// ALGO_RR_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef ALGO_RR_EXPORTS
#define ALGO_RR_API __declspec(dllexport)
#else
#define ALGO_RR_API __declspec(dllimport)
#endif

#include "../include/OSType.h"

/*
// �� Ŭ������ algo_rr.dll���� ������ ���Դϴ�.
class ALGO_RR_API Calgo_rr {
public:
	Calgo_rr(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern ALGO_RR_API int nalgo_rr;

ALGO_RR_API int fnalgo_rr(void);
*/
#ifdef __cplusplus
extern "C" {
#endif

ALGO_RR_API int lib_init_sys_util(sys_util_t* sys_util);
ALGO_RR_API int lib_get_algorithm_name(char* str_name_buf);
ALGO_RR_API int lib_get_next_process(system_t* system);

#ifdef __cplusplus
}
#endif
