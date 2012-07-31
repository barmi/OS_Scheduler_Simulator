// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// ALGO_SRTF_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef ALGO_SRTF_EXPORTS
#define ALGO_SRTF_API __declspec(dllexport)
#else
#define ALGO_SRTF_API __declspec(dllimport)
#endif

#include "../include/OSType.h"

/*
// �� Ŭ������ algo_srtf.dll���� ������ ���Դϴ�.
class ALGO_SRTF_API Calgo_srtf {
public:
	Calgo_srtf(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern ALGO_SRTF_API int nalgo_srtf;

ALGO_SRTF_API int fnalgo_srtf(void);
*/
#ifdef __cplusplus
extern "C" {
#endif

ALGO_SRTF_API int lib_init_sys_util(sys_util_t* sys_util);
ALGO_SRTF_API int lib_get_algorithm_name(char* str_name_buf);
ALGO_SRTF_API int lib_get_next_process(system_t* system);

#ifdef __cplusplus
}
#endif
