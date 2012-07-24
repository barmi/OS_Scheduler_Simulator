
// ChildView.h : CChildView Ŭ������ �������̽�
//


#pragma once

#include <vector>
#include "../include/OSType.h"

// CChildView â

#define KM_BEGIN	1
#define KM_DATAFILE	1
#define KM_ALGO		2
#define KM_QUANTUM	3
#define KM_SIMUL	4
#define KM_LAST		5

class CChildView : public CWnd
{
// �����Դϴ�.
public:
	CChildView();

// Ư���Դϴ�.
public:
	std::vector<algo_item_t> m_algo_item_list;
	std::vector<std::wstring> m_datafile_list;
	system_t*	m_system;
	int			m_keymode;	
	int			m_datafile_index;	// ���� ���õ� datafile�� �ε���, ���� �ε�� ������� �ٸ���, ȭ�鿡�� ���õ� ����

// �۾��Դϴ�.
public:

// �������Դϴ�.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CChildView();

	// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int LoadAlgorithmFiles(void);
	int LoadDataFiles(void);
	int LoadDataFile(int idx);
};

