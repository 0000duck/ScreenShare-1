
// ScreenShareClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CScreenShareClientApp: 
// �йش����ʵ�֣������ ScreenShareClient.cpp
//

class CScreenShareClientApp : public CWinApp
{
public:
	CScreenShareClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CScreenShareClientApp theApp;