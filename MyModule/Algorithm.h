#pragma once
#include "LtSocket.h"
#include "afxcmn.h"
#include "afxwin.h"

// CAlgorithm 对话框
#define BUFFER_SIZE 4096


class CAlgorithm : public CDialog
{
	DECLARE_DYNAMIC(CAlgorithm)

public:
	CAlgorithm(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAlgorithm();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void RunAlgorithm(const double *, double*);

	CLtSocket	m_DataServerSocket;	//数据链接
	CNewSocket	m_TargetSocket;		//目标Socket

private:
	unsigned int m_NetServerPort;	//数据服务端口
	unsigned int m_DataWaitingTime; //数据采样

	CString m_TargetIP;
	unsigned int m_nTagetPort;

public:
	CIPAddressCtrl m_IPAddressCtrl;
	afx_msg void OnBnClickedCreateserverbtn();
	afx_msg void OnBnClickedCreateconnectbtn();

	float m_SendBuffer1[BUFFER_SIZE/4];	//浮点数占用4字节，如果将浮点数存入char数组中，则char数组长度应该为浮点数的4倍
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
