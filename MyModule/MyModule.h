// MyModule.h : MyModule DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyModuleApp
// �йش���ʵ�ֵ���Ϣ������� MyModule.cpp
//

class CMyModuleApp : public CWinApp
{
public:
	CMyModuleApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
