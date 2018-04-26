// NewSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "NewSocket.h"

// CNewSocket

CNewSocket::CNewSocket()
{
	m_nMsgLength = 0;
	m_nChannel = 0;	//默认未分配通道
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
	case 1:	//数据通道
		m_pViewWnd->SendMessage( MSG_HAS_NEW_DATA, (WPARAM)this, 0 );
		break;
	case 2:	//消息通道
		m_pViewWnd->SendMessage( MSG_HAS_NEW_MSG, (WPARAM)this, 0 );
		break;
	}
	//清空接收缓存
	memset( m_szBuffer, 0, sizeof(m_szBuffer) );
	CAsyncSocket::OnReceive( nErrorCode );
}

void CNewSocket::OnSend(int nErrorCode)
{
	//继续提请一个“读”的网络事件，接收Server消息
	AsyncSelect( FD_READ );
	CAsyncSocket::OnSend( nErrorCode );
}

void CNewSocket::OnClose(int nErrorCode)
{
	m_hSocket = INVALID_SOCKET;
	m_pViewWnd->SendMessage( MSG_STOP_PLAYING, 0, 0 );
}
// CNewSocket 成员函数
