
// common_util_ui.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Ccommon_util_uiApp: 
// �йش����ʵ�֣������ common_util_ui.cpp
//

class Ccommon_util_uiApp : public CWinApp
{
public:
	Ccommon_util_uiApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ccommon_util_uiApp theApp;