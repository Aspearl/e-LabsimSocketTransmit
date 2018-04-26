// MyModule.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "MyModule.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMyModuleApp

BEGIN_MESSAGE_MAP(CMyModuleApp, CWinApp)
END_MESSAGE_MAP()


// CMyModuleApp ����

CMyModuleApp::CMyModuleApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMyModuleApp ����

CMyModuleApp theApp;


// CMyModuleApp ��ʼ��

BOOL CMyModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return TRUE;
}

//����һ���㷨���󣬲������㷨����ָ��
void *LtCreateObject()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());	//������ڹ��� DLL ��ʹ�� MFC������������һ��
	CAlgorithm * pAlgorithm = new CAlgorithm();
	pAlgorithm->Create(IDD_DIALOG1,NULL);	//����㷨�����Ǽ̳���CDialog��,������ô��д���
	pAlgorithm->ShowWindow(SW_SHOW);
	return static_cast<void*>( pAlgorithm );//����һ���µ��㷨����
}

//ɾ��һ���㷨���󣬴˺����ڴ��Ĳ�����pObject������LtCreateObject()�ķ���ֵ
void LtDestroyObject( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->DestroyWindow();	//����㷨�����Ǽ̳���CDialog��,������ô��д���
	delete pAlgorithm;	//ɾ��һ���㷨����
}

//�㷨������
//�˺�����һ��������pObject����LtCreateObject()�ķ���ֵ
//�˺����ڶ���������bIsPortUsing����ָ������˿��Ƿ�ʹ�õ�ָ��
//�˺����ڶ���������pdInput����ָ�����������ָ��
//�˺���������������pdOutput����ָ����������ָ��
void LtDLLMain(  void * pObject, const bool * pbIsPortUsing, const double * pdInput, double * pdOutput )
{
	ASSERT( pObject != NULL );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->RunAlgorithm(pdInput, pdOutput);	//�����㷨������RunAlgrithm()����);
}

//��ʾ�㷨��������(�����㷨�����Ǽ̳���CDialog��,�����д�˺���)
void LtShowWindow( void * pObject )
{
	ASSERT( pObject != NULL );
	ASSERT( !IsBadWritePtr( pObject, sizeof(CAlgorithm) ) );
	CAlgorithm * pAlgorithm = static_cast<CAlgorithm *>( pObject );
	pAlgorithm->ShowWindow( SW_SHOW );	//����㷨�����Ǽ̳���CDialog��,������ô��д���
}
