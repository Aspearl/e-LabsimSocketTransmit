// Algorithm.cpp : 实现文件
//

#include "stdafx.h"
#include "MyModule.h"
#include "Algorithm.h"


// CAlgorithm 对话框

IMPLEMENT_DYNAMIC(CAlgorithm, CDialog)

CAlgorithm::CAlgorithm(CWnd* pParent /*=NULL*/)
	: CDialog(CAlgorithm::IDD, pParent)
{
	

}

CAlgorithm::~CAlgorithm()
{
}

void CAlgorithm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddressCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_PortNumEdit);
	DDX_Control(pDX, IDC_EDIT2, m_ServerPortNumEdit);
}


BEGIN_MESSAGE_MAP(CAlgorithm, CDialog)
	ON_BN_CLICKED(IDOK, &CAlgorithm::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CreateServerBtn, &CAlgorithm::OnBnClickedCreateserverbtn)
	ON_BN_CLICKED(IDC_CreateConnectBtn, &CAlgorithm::OnBnClickedCreateconnectbtn)
	ON_BN_CLICKED(IDCANCEL, &CAlgorithm::OnBnClickedCancel)
	ON_MESSAGE(MSG_HAS_NEW_DATA, &CAlgorithm::OnNewData)
	ON_BN_CLICKED(IDC_BUTTON10, &CAlgorithm::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_DPY, &CAlgorithm::OnBnClickedDpy)
	ON_BN_CLICKED(IDC_DPN, &CAlgorithm::OnBnClickedDpn)
END_MESSAGE_MAP()


// CAlgorithm 消息处理程序
BOOL CAlgorithm::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_NetServerPort = 9000;	//数据服务端口
	m_nTagetPort = 9000;
	m_IPAddressCtrl.SetAddress(127, 0, 0, 1);
	m_PortNumEdit.SetWindowText(_T("9000"));		//目标端口
	m_ServerPortNumEdit.SetWindowText(_T("9000"));	//服务端口
	m_bIsSend = TRUE;		//默认为发送方
	CButton* pSendRadio = (CButton*)GetDlgItem(IDC_RADIO1);
	pSendRadio->SetCheck(1);
	CButton* pReceiveRadio = (CButton*)GetDlgItem(IDC_RADIO2);
	pReceiveRadio->SetCheck(0);

	m_IsRemoteOK = FALSE;
	m_bHasNewData = FALSE;
	m_CanSendOK = FALSE;
	m_NeedDp = FALSE;
	m_IsSynchr = FALSE;

	m_nSendBufferIndex = 0;
	m_nReceiveBufferIndex = 0;
	m_WaittingTime = 0;
	m_DataWaitingTime = 0;
	return TRUE;
}

void CAlgorithm::RunAlgorithm(const double* pdInput, double * pdOutput)
{
	m_DataWaitingTime++;
	if (m_DataWaitingTime > 1000)
		m_DataWaitingTime = 0;

	if (m_NeedDp == TRUE && m_IsSynchr == FALSE) {
		if (m_bIsSend == TRUE) {
			if (m_DataWaitingTime == 1000)
			{
				m_SendBuffer1[m_nSendBufferIndex] = float(pdInput[1]);	//从Input-2获取数据来发送，将double类型的数据强制转换为float类型，降低的精度对于我们的算法来讲不会有影响
				m_nSendBufferIndex++;
			}
			if (m_nSendBufferIndex >= dpNum / 4)
			{	//发送25个导频信号
 				memcpy(m_SendBuffer2, m_SendBuffer1, dpNum * sizeof(char));
				while (m_IsRemoteOK == FALSE)	//等待接收方返回接收回执，这里可以考虑设置一个等待的时间门限，如果超时，则认为接收方出现故障，终止远程发送，断开SOCKET连接
				{	//如果采用非同步传输方式，可不用等待，仿真效率会提高，但数据在接收方可能会有丢失
					Sleep(1);
					m_WaittingTime++;
					if (m_WaittingTime > 1000)	//等待时间达到1秒，重发一次数据，应对对方反应慢的问题
					{
						m_WaittingTime = 0;
						m_TargetSocket.Send(m_SendBuffer2, dpNum);
					}
				}
				m_IsRemoteOK = FALSE;
				m_nSendBufferIndex = 0;
				m_IsSynchr = TRUE;
			}
		}
		else {
				
				//数据接收
				while (m_bHasNewData == FALSE)	//等待发送方发送新的数据，可以设置一个超时时间，如果超时则断开SOCKET连接，终止接收数据
				{	//如果采用非同步传输方式，可不用等待，仿真效率会提高，但是数据可能会有重复
					Sleep(1);
				}
				if (m_DataWaitingTime == 1000)
				{
					dpbuffer[m_nReceiveBufferIndex++] = m_ReceiveBuffer[m_nReceiveBufferIndex];			
				}
				if (m_nReceiveBufferIndex >= dpNum / 4)
				{
					m_nReceiveBufferIndex = 0;
					m_bHasNewData = FALSE;
				}
				for (int i = 0; i < dpNum/4; i++) {
				FLAG:if (dpbuffer[i] - float(pdInput[1]) <= 0.01) {
					pdOutput[1] = dpbuffer[i];
						continue;
					}
					else {
						goto FLAG;
					}
				}
				m_CanSendOK = TRUE;
				m_IsSynchr = TRUE;
				m_bHasNewData = FALSE;
			}
		}
	
	
	if (m_NeedDp == FALSE || m_IsSynchr == TRUE) {

		if (m_bIsSend == TRUE)	//发送方
		{
			//数据发送
			if (m_DataWaitingTime == 1000)
			{
				m_SendBuffer1[m_nSendBufferIndex] = float(pdInput[0]);	//从Input-1获取数据来发送，将double类型的数据强制转换为float类型，降低的精度对于我们的算法来讲不会有影响
				m_nSendBufferIndex++;
			}
			if (m_nSendBufferIndex >= BUFFER_SIZE / 4)
			{	//发送缓存已满，将数据发送出去
				m_nSendBufferIndex = 0;
				memcpy(m_SendBuffer2, m_SendBuffer1, BUFFER_SIZE * sizeof(char));
				while (m_IsRemoteOK == FALSE)	//等待接收方返回接收回执，这里可以考虑设置一个等待的时间门限，如果超时，则认为接收方出现故障，终止远程发送，断开SOCKET连接
				{	//如果采用非同步传输方式，可不用等待，仿真效率会提高，但数据在接收方可能会有丢失
					Sleep(1);
					m_WaittingTime++;
					if (m_WaittingTime>1000)	//等待时间达到1秒，重发一次数据，应对对方反应慢的问题
					{
						m_WaittingTime = 0;
						m_TargetSocket.Send(m_SendBuffer2, BUFFER_SIZE);
					}
				}
				m_IsRemoteOK = FALSE;
				m_TargetSocket.Send(m_SendBuffer2, BUFFER_SIZE);
			}
		}
		else	//接收方
		{
			//数据接收
			while (m_bHasNewData == FALSE)	//等待发送方发送新的数据，可以设置一个超时时间，如果超时则断开SOCKET连接，终止接收数据
			{	//如果采用非同步传输方式，可不用等待，仿真效率会提高，但是数据可能会有重复
				Sleep(1);
			}
			if (m_DataWaitingTime == 1000)
			{
				pdOutput[0] = m_ReceiveBuffer[m_nReceiveBufferIndex];
				m_nReceiveBufferIndex++;
			}
			if (m_nReceiveBufferIndex >= BUFFER_SIZE / 4)
			{
				m_nReceiveBufferIndex = 0;
				m_bHasNewData = FALSE;
			}
		}
	}
	
}

void CAlgorithm::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
//	OnOK();	//防止误关闭对话框，需要将本行代码注释掉
}

//创建服务
void CAlgorithm::OnBnClickedCreateserverbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strPortNum;
	m_ServerPortNumEdit.GetWindowText(strPortNum);	//从对话框获取服务端口
	m_NetServerPort = _wtoi(strPortNum);
	BOOL bFlag = FALSE;
	bFlag = m_DataServerSocket.Create(m_NetServerPort, SOCK_STREAM, FD_CLOSE|FD_ACCEPT);	//创建Socket服务
	if(!bFlag)
	{
		m_DataServerSocket.Close();
		AfxMessageBox(_T("创建数据服务套接字失败!"));
		return;
	}
	if(!m_DataServerSocket.Listen(1))	//开启监听
	{
		int nErrorCode = m_DataServerSocket.GetLastError();
		if(nErrorCode != WSAEWOULDBLOCK)
		{
			m_DataServerSocket.Close();
			AfxMessageBox(_T("开启数据服务失败!"));
			return;
		}
	}
	m_DataServerSocket.SetViewWnd(this);
	m_DataServerSocket.SetDataChannel(1);
}

//接收数据事件
LRESULT CAlgorithm::OnNewData(WPARAM wParam, LPARAM lParam)
{
	CNewSocket *pSocket = reinterpret_cast<CNewSocket*>( wParam );
	ASSERT( pSocket != NULL && !::IsBadReadPtr( (void*)pSocket, sizeof( CNewSocket ) ) );

	if(m_bIsSend == FALSE)	//作为接收方，接收的数据为仿真数据
	{
		if(m_bHasNewData == FALSE)
		{
			memcpy(m_ReceiveBuffer, pSocket->GetRevBuffer(), BUFFER_SIZE*sizeof(char));
			m_bHasNewData = TRUE;
			//应答发送方，本地已收到数据
			if (m_NeedDp == TRUE && m_CanSendOK == TRUE) {
				pSocket->Send("R_OK", 5 * sizeof(char));
			}
			if (m_NeedDp == FALSE) {
				pSocket->Send("R_OK", 5 * sizeof(char));
			}
			
		}
	}
	else	//作为发送方接收到的数据为对方返回的回执信息
	{
		//这里要先判断对方发过来的数据是否为"R_OK"如果是，则认为对方已经收到上一帧数据，可以继续发送数据了，否则需要等待对方响应才能发送新的数据
		m_IsRemoteOK = TRUE;
	}
	return 0;
}

//连接服务
void CAlgorithm::OnBnClickedCreateconnectbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	
	DWORD dwIP;
	if( m_IPAddressCtrl.GetAddress( dwIP ) < 4 )	//获取目标IP地址
	{
		AfxMessageBox( _T("注意!\nIP地址填写不完整!"), MB_ICONWARNING );
		return;
	}
	unsigned char *pIP;
	pIP = (unsigned char*)&dwIP;
	unsigned char *IP1 = pIP+3;
	unsigned char *IP2 = pIP+2;
	unsigned char *IP3 = pIP+1;
	unsigned char *IP4 = pIP;
	m_TargetIP.Format( _T("%u.%u.%u.%u"), *IP1, *IP2, *IP3, *IP4 );
	CString strPortNum;
	m_PortNumEdit.GetWindowText(strPortNum);	//获取目标端口号
	m_nTagetPort = _wtoi(strPortNum);
	m_TargetSocket.Create();
	m_TargetSocket.Connect(m_TargetIP, m_nTagetPort);	//连接目标
	m_TargetSocket.AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE);	//设置传输模式
	m_TargetSocket.SetViewWnd(this);	//设置消息返回事件的句柄
	m_TargetSocket.SetChannelNum(1);	//设置当前Socket传输通道为数据通道
}

void CAlgorithm::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
//	OnCancel();
	ShowWindow(SW_MINIMIZE);
}

void CAlgorithm::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	int nState = 0;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO1);
	nState = pButton->GetCheck();
	if(nState==0)	//接收模式
		m_bIsSend = FALSE;
	else
		m_bIsSend = TRUE;
}


void CAlgorithm::OnBnClickedDpy()
{
	// TODO: 在此添加控件通知处理程序代码
	m_NeedDp = true;
	this->GetDlgItem(IDC_DPY)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_DPN)->EnableWindow(TRUE);

}


void CAlgorithm::OnBnClickedDpn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_NeedDp = false;
	this->GetDlgItem(IDC_DPY)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_DPN)->EnableWindow(FALSE);
}
