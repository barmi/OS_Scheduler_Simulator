
// ChildView.h : CChildView 클래스의 인터페이스
//


#pragma once

#include <vector>
#include "../include/OSType.h"

// CChildView 창

#define KM_BEGIN	1
#define KM_DATAFILE	1
#define KM_ALGO		2
#define KM_QUANTUM	3
#define KM_SIMUL	4
#define KM_LAST		5

class CChildView : public CWnd
{
// 생성입니다.
public:
	CChildView();

// 특성입니다.
public:
	std::vector<algo_item_t> m_algo_item_list;
	std::vector<std::wstring> m_datafile_list;
	system_t*	m_system;
	int			m_keymode;	
	int			m_datafile_index;	// 현재 선택된 datafile의 인덱스, 실제 로드된 내용과는 다르며, 화면에서 선택된 내용

// 작업입니다.
public:

// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int LoadAlgorithmFiles(void);
	int LoadDataFiles(void);
	int LoadDataFile(int idx);
};

