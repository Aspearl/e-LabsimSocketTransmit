// Algorithm.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyModule.h"
#include "Algorithm.h"


// CAlgorithm �Ի���

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


// CAlgorithm ��Ϣ�������
BOOL CAlgorithm::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_NetServerPort = 9000;	//���ݷ���˿�
	m_nTagetPort = 9000;
	m_IPAddressCtrl.SetAddress(127, 0, 0, 1);
	m_PortNumEdit.SetWindowText(_T("9000"));		//Ŀ��˿�
	m_ServerPortNumEdit.SetWindowText(_T("9000"));	//����˿�
	m_bIsSend = TRUE;		//Ĭ��Ϊ���ͷ�
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
				m_SendBuffer1[m_nSendBufferIndex] = float(pdInput[1]);	//��Input-2��ȡ���������ͣ���double���͵�����ǿ��ת��Ϊfloat���ͣ����͵ľ��ȶ������ǵ��㷨����������Ӱ��
				m_nSendBufferIndex++;
			}
			if (m_nSendBufferIndex >= dpNum / 4)
			{	//����25����Ƶ�ź�
 				memcpy(m_SendBuffer2, m_SendBuffer1, dpNum * sizeof(char));
				while (m_IsRemoteOK == FALSE)	//�ȴ����շ����ؽ��ջ�ִ��������Կ�������һ���ȴ���ʱ�����ޣ������ʱ������Ϊ���շ����ֹ��ϣ���ֹԶ�̷��ͣ��Ͽ�SOCKET����
				{	//������÷�ͬ�����䷽ʽ���ɲ��õȴ�������Ч�ʻ���ߣ��������ڽ��շ����ܻ��ж�ʧ
					Sleep(1);
					m_WaittingTime++;
					if (m_WaittingTime > 1000)	//�ȴ�ʱ��ﵽ1�룬�ط�һ�����ݣ�Ӧ�ԶԷ���Ӧ��������
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
				
				//���ݽ���
				while (m_bHasNewData == FALSE)	//�ȴ����ͷ������µ����ݣ���������һ����ʱʱ�䣬�����ʱ��Ͽ�SOCKET���ӣ���ֹ��������
				{	//������÷�ͬ�����䷽ʽ���ɲ��õȴ�������Ч�ʻ���ߣ��������ݿ��ܻ����ظ�
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

		if (m_bIsSend == TRUE)	//���ͷ�
		{
			//���ݷ���
			if (m_DataWaitingTime == 1000)
			{
				m_SendBuffer1[m_nSendBufferIndex] = float(pdInput[0]);	//��Input-1��ȡ���������ͣ���double���͵�����ǿ��ת��Ϊfloat���ͣ����͵ľ��ȶ������ǵ��㷨����������Ӱ��
				m_nSendBufferIndex++;
			}
			if (m_nSendBufferIndex >= BUFFER_SIZE / 4)
			{	//���ͻ��������������ݷ��ͳ�ȥ
				m_nSendBufferIndex = 0;
				memcpy(m_SendBuffer2, m_SendBuffer1, BUFFER_SIZE * sizeof(char));
				while (m_IsRemoteOK == FALSE)	//�ȴ����շ����ؽ��ջ�ִ��������Կ�������һ���ȴ���ʱ�����ޣ������ʱ������Ϊ���շ����ֹ��ϣ���ֹԶ�̷��ͣ��Ͽ�SOCKET����
				{	//������÷�ͬ�����䷽ʽ���ɲ��õȴ�������Ч�ʻ���ߣ��������ڽ��շ����ܻ��ж�ʧ
					Sleep(1);
					m_WaittingTime++;
					if (m_WaittingTime>1000)	//�ȴ�ʱ��ﵽ1�룬�ط�һ�����ݣ�Ӧ�ԶԷ���Ӧ��������
					{
						m_WaittingTime = 0;
						m_TargetSocket.Send(m_SendBuffer2, BUFFER_SIZE);
					}
				}
				m_IsRemoteOK = FALSE;
				m_TargetSocket.Send(m_SendBuffer2, BUFFER_SIZE);
			}
		}
		else	//���շ�
		{
			//���ݽ���
			while (m_bHasNewData == FALSE)	//�ȴ����ͷ������µ����ݣ���������һ����ʱʱ�䣬�����ʱ��Ͽ�SOCKET���ӣ���ֹ��������
			{	//������÷�ͬ�����䷽ʽ���ɲ��õȴ�������Ч�ʻ���ߣ��������ݿ��ܻ����ظ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	OnOK();	//��ֹ��رնԻ�����Ҫ�����д���ע�͵�
}

//��������
void CAlgorithm::OnBnClickedCreateserverbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strPortNum;
	m_ServerPortNumEdit.GetWindowText(strPortNum);	//�ӶԻ����ȡ����˿�
	m_NetServerPort = _wtoi(strPortNum);
	BOOL bFlag = FALSE;
	bFlag = m_DataServerSocket.Create(m_NetServerPort, SOCK_STREAM, FD_CLOSE|FD_ACCEPT);	//����Socket����
	if(!bFlag)
	{
		m_DataServerSocket.Close();
		AfxMessageBox(_T("�������ݷ����׽���ʧ��!"));
		return;
	}
	if(!m_DataServerSocket.Listen(1))	//��������
	{
		int nErrorCode = m_DataServerSocket.GetLastError();
		if(nErrorCode != WSAEWOULDBLOCK)
		{
			m_DataServerSocket.Close();
			AfxMessageBox(_T("�������ݷ���ʧ��!"));
			return;
		}
	}
	m_DataServerSocket.SetViewWnd(this);
	m_DataServerSocket.SetDataChannel(1);
}

//���������¼�
LRESULT CAlgorithm::OnNewData(WPARAM wParam, LPARAM lParam)
{
	CNewSocket *pSocket = reinterpret_cast<CNewSocket*>( wParam );
	ASSERT( pSocket != NULL && !::IsBadReadPtr( (void*)pSocket, sizeof( CNewSocket ) ) );

	if(m_bIsSend == FALSE)	//��Ϊ���շ������յ�����Ϊ��������
	{
		if(m_bHasNewData == FALSE)
		{
			memcpy(m_ReceiveBuffer, pSocket->GetRevBuffer(), BUFFER_SIZE*sizeof(char));
			m_bHasNewData = TRUE;
			//Ӧ���ͷ����������յ�����
			if (m_NeedDp == TRUE && m_CanSendOK == TRUE) {
				pSocket->Send("R_OK", 5 * sizeof(char));
			}
			if (m_NeedDp == FALSE) {
				pSocket->Send("R_OK", 5 * sizeof(char));
			}
			
		}
	}
	else	//��Ϊ���ͷ����յ�������Ϊ�Է����صĻ�ִ��Ϣ
	{
		//����Ҫ���ж϶Է��������������Ƿ�Ϊ"R_OK"����ǣ�����Ϊ�Է��Ѿ��յ���һ֡���ݣ����Լ������������ˣ�������Ҫ�ȴ��Է���Ӧ���ܷ����µ�����
		m_IsRemoteOK = TRUE;
	}
	return 0;
}

//���ӷ���
void CAlgorithm::OnBnClickedCreateconnectbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	DWORD dwIP;
	if( m_IPAddressCtrl.GetAddress( dwIP ) < 4 )	//��ȡĿ��IP��ַ
	{
		AfxMessageBox( _T("ע��!\nIP��ַ��д������!"), MB_ICONWARNING );
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
	m_PortNumEdit.GetWindowText(strPortNum);	//��ȡĿ��˿ں�
	m_nTagetPort = _wtoi(strPortNum);
	m_TargetSocket.Create();
	m_TargetSocket.Connect(m_TargetIP, m_nTagetPort);	//����Ŀ��
	m_TargetSocket.AsyncSelect(FD_READ|FD_WRITE|FD_CLOSE);	//���ô���ģʽ
	m_TargetSocket.SetViewWnd(this);	//������Ϣ�����¼��ľ��
	m_TargetSocket.SetChannelNum(1);	//���õ�ǰSocket����ͨ��Ϊ����ͨ��
}

void CAlgorithm::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	OnCancel();
	ShowWindow(SW_MINIMIZE);
}

void CAlgorithm::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nState = 0;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO1);
	nState = pButton->GetCheck();
	if(nState==0)	//����ģʽ
		m_bIsSend = FALSE;
	else
		m_bIsSend = TRUE;
}


void CAlgorithm::OnBnClickedDpy()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_NeedDp = true;
	this->GetDlgItem(IDC_DPY)->EnableWindow(FALSE);
	this->GetDlgItem(IDC_DPN)->EnableWindow(TRUE);

}


void CAlgorithm::OnBnClickedDpn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_NeedDp = false;
	this->GetDlgItem(IDC_DPY)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_DPN)->EnableWindow(FALSE);
}
