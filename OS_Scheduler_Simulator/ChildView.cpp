
// ChildView.cpp : CChildView 클래스의 구현
//

#include "stdafx.h"
#pragma warning(disable : 4995)
#pragma warning(disable : 4996)
#include <tchar.h>
#include <strsafe.h>
#include "OS_Scheduler_Simulator.h"
#include "ChildView.h"
#include "../include/OSType.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
	: m_keymode(KM_DATAFILE)
	, m_datafile_index(0)
{
	LoadAlgorithmFiles();
	LoadDataFiles();
	m_system = system_make();
}

CChildView::~CChildView()
{
	system_free(m_system);

	for (std::vector<algo_item_t>::iterator it = m_algo_item_list.begin(); it != m_algo_item_list.end(); ++it)
	{
		if (it->lib_handle)
		{
			FreeLibrary(it->lib_handle);
		}
	}
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

#define RGB_BLACK	RGB(0,0,0)
#define RGB_RED		RGB(255,0,0)
#define RGB_GREEN	RGB(0,255,0)
#define RGB_BLUE	RGB(0,0,255)

void CChildView::OnPaint() 
{
	int screen_start_x = 5;
	int screen_start_y = 5;
	int screen_line_height = 15;
	int status_start_x = 50;
	int status_unit_width = 3;
	int status_unit_top = 2;
	int status_unit_height = screen_line_height - 2*status_unit_top;

	int screen_cur_y = screen_start_y;
	int screen_cur_x;

	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	
	CPen blackPen(PS_SOLID, 1, RGB(0,0,0));
	CPen greenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen redPen(PS_SOLID, 1, RGB(255,0,0));
	CPen *oldPen = dc.SelectObject(&blackPen);

	CBrush redBrush(RGB_RED);
	CBrush greenBrush(RGB_GREEN);
	CBrush blackBrush(RGB_BLACK);
	CBrush *oldBrush = dc.SelectObject(&blackBrush);


	CFont l_font;
	l_font.CreateFont(screen_line_height-1, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, /*L"Courier New"*/ L"Verdana");
	CFont *oldFont = dc.SelectObject(&l_font);
	
	CString str;
	str.Format(L"Process Scheduler Simulator");
	dc.TextOut(screen_start_x, screen_cur_y, str);
	screen_cur_y += screen_line_height;
	
	// Data File
	screen_cur_x = screen_start_x;
	str = L"DataFile : ";
	dc.TextOut(screen_cur_x, screen_cur_y, str);
	screen_cur_x += dc.GetTextExtent(str).cx;

	for (int i = 0; i < (int)m_datafile_list.size(); ++i)
	{
		str = CString(m_datafile_list[i].c_str());
		if (i == m_datafile_index)
		{
			if (m_keymode == KM_DATAFILE)
				dc.SetTextColor(RGB_RED);
			else
				dc.SetTextColor(RGB_BLUE);
		}
		else
			dc.SetTextColor(RGB_BLACK);
		dc.TextOut(screen_cur_x, screen_cur_y, str);
		screen_cur_x += dc.GetTextExtent(str).cx + 10;
	}
	screen_cur_y += screen_line_height;

	// Algorithm
	screen_cur_x = screen_start_x;
	dc.SetTextColor(RGB_BLACK);
	str = L"Algorithm : ";
	dc.TextOut(screen_cur_x, screen_cur_y, str);
	screen_cur_x += dc.GetTextExtent(str).cx;

	for (std::vector<algo_item_t>::iterator it = m_algo_item_list.begin(); it != m_algo_item_list.end(); ++it)
	{
		char algo_name[32];
		it->fp_get_algorithm_name(algo_name);
		TCHAR walgo_name[32];
		memset(walgo_name, 0x00, sizeof walgo_name);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, algo_name, strlen(algo_name), walgo_name, 32);
		
		if (m_system->algorithm == it->algorithm)
			dc.SetTextColor( (m_keymode == KM_ALGO) ? RGB_RED : RGB_BLUE);
		else
			dc.SetTextColor(RGB_BLACK);

		str.Format(L"%d: %s", it->algorithm, walgo_name);
		dc.TextOut(screen_cur_x, screen_cur_y, str);
		screen_cur_x += dc.GetTextExtent(str).cx + 10;
	}
	screen_cur_y += screen_line_height;

	// Quantum
	screen_cur_x = screen_start_x;
	dc.SetTextColor(RGB_BLACK);
	str = L"Quantum : ";
	dc.TextOut(screen_cur_x, screen_cur_y, str);
	screen_cur_x += dc.GetTextExtent(str).cx;

	dc.SetTextColor( (m_keymode == KM_QUANTUM) ? RGB_RED : RGB_BLUE );
	str.Format(L"%d", m_system->quantum);
	dc.TextOut(screen_cur_x, screen_cur_y, str);

	screen_cur_y += screen_line_height;

	// Current Time
	screen_cur_x = screen_start_x;
	dc.SetTextColor(RGB_BLACK);
	str = L"Current Time : ";
	dc.TextOut(screen_cur_x, screen_cur_y, str);
	screen_cur_x += dc.GetTextExtent(str).cx;

	dc.SetTextColor( (m_keymode == KM_SIMUL) ? RGB_RED : RGB_BLUE );
	str.Format(L"%d", m_system->cur_time);
	dc.TextOut(screen_cur_x, screen_cur_y, str);

	screen_cur_y += screen_line_height;

	dc.SetTextColor(RGB(0,0,0));

	for (int pcb_index = 0; pcb_index < 3; ++pcb_index)
	{
		pcb_t* pcb;
		CString strTitle;

		switch (pcb_index)
		{
			case 0:
				pcb = m_system->terminate_queue->head;
				strTitle = L"[Terminated Queue]";
				break;
			case 1:
				pcb = m_system->ready_queue->head;
				strTitle = L"[Ready Queue]";
				break;
			case 2:
				pcb = m_system->work_queue->head;
				strTitle = L"[Work Queue]";
				break;
		}

		dc.TextOut(screen_start_x, screen_cur_y, strTitle);
		screen_cur_y += screen_line_height;

		while (pcb)
		{
			str.Format(L"%d", pcb->pid);
			dc.SelectObject(&blackPen);
			dc.TextOut(screen_start_x, screen_cur_y, str);

			for (int i = 0; i <= m_system->cur_time; ++i)
			{
				switch(pcb->time_status[i])
				{
				case 'W':
					{
						dc.SelectObject(&greenPen);
						dc.SelectObject(&greenBrush);
						dc.Rectangle(status_start_x + status_unit_width*i,
							screen_cur_y + status_unit_top,
							status_start_x + status_unit_width*(i+1) - 1,
							screen_cur_y + status_unit_top + status_unit_height);
					}
					break;
				case 'B':
					{
						dc.SelectObject(&redPen);
						dc.SelectObject(&redBrush);
						dc.Rectangle(status_start_x + status_unit_width*i,
							screen_cur_y + status_unit_top,
							status_start_x + status_unit_width*(i+1) - 1,
							screen_cur_y + status_unit_top + status_unit_height);
					}
					break;
				}
			}

			screen_cur_y += screen_line_height;
			pcb = pcb->next;
		}
	}

	dc.SelectObject(oldBrush);
	dc.SelectObject(oldPen);
	dc.SelectObject(oldFont);
	l_font.DeleteObject();
}



void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case 'A':
		{
			// system의 algorithm을 선택
			if (m_system->cur_time < 0)
			{
				if (++m_system->algorithm >= int(m_algo_item_list.size()))
					m_system->algorithm = 0;
				Invalidate();
			}
		}
		break;
	case 'L':
		{
			//LoadDataFile();
			m_system->quantum = 10;
			m_system->algorithm = 0;

			Invalidate();
		}
		break;
	case VK_LEFT:
		{
			switch (m_keymode)
			{
			case KM_DATAFILE:
				{
					if (--m_datafile_index < 0)
						m_datafile_index = (int)m_datafile_list.size()-1;
				}
				break;
			case KM_ALGO:
				{
					// system의 algorithm을 선택
					if (m_system->cur_time < 0)
					{
						if (--m_system->algorithm < 0)
							m_system->algorithm = (int)m_algo_item_list.size()-1;
					}
				}
				break;
			case KM_QUANTUM:
				{
					if (m_system->quantum > 0)
						--m_system->quantum;
				}
				break;
			case KM_SIMUL:
				{
				}
				break;
			}
			Invalidate();
		}
		break;
	case VK_RIGHT:
		{
			switch (m_keymode)
			{
			case KM_DATAFILE:
				{
					if (++m_datafile_index >= (int)m_datafile_list.size())
						m_datafile_index = 0;
				}
				break;
			case KM_ALGO:
				{
					// system의 algorithm을 선택
					if (m_system->cur_time < 0)
					{
						if (++m_system->algorithm >= int(m_algo_item_list.size()))
							m_system->algorithm = 0;
					}
				}
				break;
			case KM_QUANTUM:
				{
					++m_system->quantum;
				}
				break;
			case KM_SIMUL:
				{
					//=====================================================================
					// 시스템 시각을 증가 시킨다.
					++m_system->cur_time;
					// 알고리즘을 호출하기 전에, 도착한 PCB가 있으면 ready큐로 옮겨 놓는다.
					system_switch_work_to_ready(m_system);
					// 알고리즘을 수행한다.
					m_algo_item_list[m_system->algorithm].fp_get_next_process(m_system);
					// 알고리즘을 수행한 후, 후처리를 한다.
					system_update_after_algorithm(m_system);
					//=====================================================================
				}
				break;
			}

			Invalidate();
		}
		break;
	case VK_UP:
		{
			if (--m_keymode < KM_BEGIN)
				m_keymode = KM_LAST-1;
			Invalidate();
		}
		break;
	case VK_DOWN:
		{
			if (++m_keymode == KM_LAST)
				m_keymode = KM_BEGIN;
			Invalidate();
		}
		break;
	case VK_RETURN:
		{
			if (m_keymode == KM_DATAFILE)
			{
				LoadDataFile(m_datafile_index);
				Invalidate();
			}
		}
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


#define BUFSIZE MAX_PATH

int CChildView::LoadAlgorithmFiles(void)
{
	TCHAR buf[BUFSIZE];
	DWORD dwRet;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	dwRet = GetModuleFileName(AfxGetApp()->m_hInstance, buf, BUFSIZE);
	TRACE(L"%s\n", buf);

	PathRemoveFileSpec(buf);
	TRACE(L"%s\n", buf);

	StringCchCat(buf, BUFSIZE, TEXT("\\algo_*.dll"));
	hFind = FindFirstFile(buf, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	static sys_util_t sys_util = {
		queue_make,
		queue_init,
		queue_clear,
		queue_find_prev_pcb,

		system_make,
		system_free,
		system_init,
		system_clear,
		system_add_process_to_work,
		system_switch_work_to_ready,
		system_switch_ready_to_terminate,
		system_update_after_algorithm,
		system_debug_status
	};

	int algo_count = 0;
	do
	{
		HINSTANCE hLib;
		algo_item_t algo_item;

		if ((hLib = LoadLibrary(ffd.cFileName)) != NULL)
		{
			FARPROC* lpf = NULL;
			algo_item.fp_init_sys_util = (lib_init_sys_util_t)GetProcAddress(hLib, "lib_init_sys_util");
			algo_item.fp_get_algorithm_name = (lib_get_algorithm_name_t)GetProcAddress(hLib, "lib_get_algorithm_name");
			algo_item.fp_get_next_process = (lib_get_next_process_t)GetProcAddress(hLib, "lib_get_next_process");
			if (algo_item.fp_init_sys_util && algo_item.fp_get_algorithm_name && algo_item.fp_get_next_process)
			{
				algo_item.fp_init_sys_util(&sys_util);

				algo_item.lib_handle = hLib;
				algo_item.algorithm = algo_count++;
				m_algo_item_list.push_back(algo_item);
			}
			else
			{
				FreeLibrary(hLib);
			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	return algo_count;
}

int CChildView::LoadDataFiles(void)
{
	TCHAR buf[BUFSIZE];
	DWORD dwRet;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	dwRet = GetModuleFileName(AfxGetApp()->m_hInstance, buf, BUFSIZE);
	TRACE(L"%s\n", buf);

	PathRemoveFileSpec(buf);
	TRACE(L"%s\n", buf);

	StringCchCat(buf, BUFSIZE, TEXT("\\*.dat"));
	hFind = FindFirstFile(buf, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	do
	{
		std::wstring filename(ffd.cFileName);
		m_datafile_list.push_back(filename);
	}
	while (FindNextFile(hFind, &ffd) != 0);

	return m_datafile_list.size();
}


int CChildView::LoadDataFile(int idx)
{
	TCHAR buf[BUFSIZE];
	DWORD dwRet;

	system_clear(m_system);

	dwRet = GetModuleFileName(AfxGetApp()->m_hInstance, buf, BUFSIZE);
	PathRemoveFileSpec(buf);

	StringCchCat(buf, BUFSIZE, L"\\");
	StringCchCat(buf, BUFSIZE, m_datafile_list[m_datafile_index].c_str());

	FILE* fp;
	fp = _wfopen(buf, L"rt");
	if (fp == NULL)
		return 0;

	int nBurst, nDelay, nPriority;
	int arrival_time = 0;
	while (fwscanf(fp, L"%d %d %d", &nBurst, &nDelay, &nPriority) > 0)
	{
		arrival_time += nDelay;
		system_add_process_to_work(m_system, arrival_time, nBurst, nPriority);
	}
	fclose(fp);

	return 0;
}
