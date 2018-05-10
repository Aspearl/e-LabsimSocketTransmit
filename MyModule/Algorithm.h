#pragma once
#include "LtSocket.h"
#include "afxcmn.h"
#include "afxwin.h"

// CAlgorithm �Ի���
#define BUFFER_SIZE 4096


class CAlgorithm : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithm)

public:
	CAlgorithm(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAlgorithm();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void RunAlgorithm(const double *, double*);

	CLtSocket	m_DataServerSocket;	//��������
	CNewSocket	m_TargetSocket;		//Ŀ��Socket

private:
	unsigned int m_NetServerPort;	//���ݷ���˿�
	unsigned int m_DataWaitingTime; //���ݲ���

	CString m_TargetIP;
	unsigned int m_nTagetPort;

public:
	CIPAddressCtrl m_IPAddressCtrl;
	afx_msg void OnBnClickedCreateserverbtn();
	afx_msg void OnBnClickedCreateconnectbtn();

	float m_SendBuffer1[BUFFER_SIZE/4];	//������ռ��4�ֽڣ����������������char�����У���char���鳤��Ӧ��Ϊ��������4��
	char m_SendBuffer2[BUFFER_SIZE];

	float m_ReceiveBuffer[BUFFER_SIZE/4];

	LRESULT OnNewData(WPARAM wParam, LPARAM lParam);

private:
	BOOL m_bIsSend;
	unsigned int m_nSendBufferIndex;
	unsigned int m_nReceiveBufferIndex;
	BOOL m_bHasNewData;
	BOOL m_IsRemoteOK;
	BOOL m_NeedDp;
	BOOL m_IsSynchr;
	BOOL m_CanSendOK;
#define dpNum 100

	 float dpbuffer[dpNum];

	unsigned int m_WaittingTime;
public:
	afx_msg void OnBnClickedCancel();
	CEdit m_PortNumEdit;
	CEdit m_ServerPortNumEdit;
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedDpy();
	afx_msg void OnBnClickedDpn();
};
