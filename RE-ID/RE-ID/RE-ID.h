
// RE-ID.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CREIDApp: 
// �йش����ʵ�֣������ RE-ID.cpp
//

class CREIDApp : public CWinApp
{
public:
	CREIDApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CREIDApp theApp;