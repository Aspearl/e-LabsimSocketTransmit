// NewSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NewSocket.h"

// CNewSocket

CNewSocket::CNewSocket()
{
	m_nMsgLength = 0;
	m_nChannel = 0;	//Ĭ��δ����ͨ��
	memset( m_szBuffer, 0, sizeof(m_szBuffer) );
}

CNewSocket::~CNewSocket()
{
}

void CNewSocket::OnReceive(int nErrorCode)
{
	m_nMsgLength = Receive( m_szBuffer, sizeof(m_szBuffer), 0 );
	switch( m_nChannel )
	{
	case 1:	//����ͨ��
		m_pViewWnd->SendMessage( MSG_HAS_NEW_DATA, (WPARAM)this, 0 );
		break;
	case 2:	//��Ϣͨ��
		m_pViewWnd->SendMessage( MSG_HAS_NEW_MSG, (WPARAM)this, 0 );
		break;
	}
	//��ս��ջ���
	memset( m_szBuffer, 0, sizeof(m_szBuffer) );
	CAsyncSocket::OnReceive( nErrorCode );
}

void CNewSocket::OnSend(int nErrorCode)
{
	//��������һ���������������¼�������Server��Ϣ
	AsyncSelect( FD_READ );
	CAsyncSocket::OnSend( nErrorCode );
}

void CNewSocket::OnClose(int nErrorCode)
{
	m_hSocket = INVALID_SOCKET;
	m_pViewWnd->SendMessage( MSG_STOP_PLAYING, 0, 0 );
}
// CNewSocket ��Ա����
