// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 _EXPORTS 기호로
// 컴파일되며, 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// ALGO_SRTF_API 함수를 DLL에서 가져오는 것으로 보고, 이 DLL은
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#ifdef ALGO_SRTF_EXPORTS
#define ALGO_SRTF_API __declspec(dllexport)
#else
#define ALGO_SRTF_API __declspec(dllimport)
#endif

#include "../include/OSType.h"

/*
// 이 클래스는 algo_srtf.dll에서 내보낸 것입니다.
class ALGO_SRTF_API Calgo_srtf {
public:
	Calgo_srtf(void);
	// TODO: 여기에 메서드를 추가합니다.
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
