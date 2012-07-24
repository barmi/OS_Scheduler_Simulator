
// OS_Scheduler_Simulator.h : OS_Scheduler_Simulator ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// COS_Scheduler_SimulatorApp:
// �� Ŭ������ ������ ���ؼ��� OS_Scheduler_Simulator.cpp�� �����Ͻʽÿ�.
//

class COS_Scheduler_SimulatorApp : public CWinAppEx
{
public:
	COS_Scheduler_SimulatorApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COS_Scheduler_SimulatorApp theApp;
