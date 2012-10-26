// NetConnect.cpp: implementation of the NetConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "mdk/atom.h"
#include "frame/netserver/NetConnect.h"
#include "frame/netserver/NetEventMonitor.h"
#include "frame/netserver/NetEngine.h"

namespace mdk
{

NetConnect::NetConnect(SOCKET sock, bool bIsServer, NetEventMonitor *pNetMonitor, NetEngine *pEngine)
:m_socket(sock,Socket::tcp),m_host( bIsServer )
{
	m_pEngine = pEngine;
	m_pNetMonitor = pNetMonitor;
	m_id = m_socket.GetSocket();
	m_uWorkAccessCount = 0;
	m_host.m_pConnect = this;
	m_nReadCount = 0;
	m_bReadAble = false;

	m_nSendCount = 0;//���ڽ��з��͵��߳���
	m_bSendAble = false;//io��������������Ҫ����
	m_bConnect = true;//ֻ�з������ӲŴ����������Զ��󴴽�����һ��������״̬
#ifdef WIN32
	Socket::InitForIOCP(sock);	
#endif
	m_socket.InitWanAddress();
	m_socket.InitLocalAddress();
}

NetConnect::~NetConnect()
{

}

bool NetConnect::IsFree()
{
	if ( this->m_uWorkAccessCount > 0 ) return false;
	return true;
}

void NetConnect::WorkAccess()
{
	AutoLock lock( &m_lockWorkAccessCount );
	this->m_uWorkAccessCount++;
}

void NetConnect::WorkFinished()
{
	AutoLock lock( &m_lockWorkAccessCount );
	this->m_uWorkAccessCount--;
}

void NetConnect::RefreshHeart()
{
	m_tLastHeart = time( NULL );
}

time_t NetConnect::GetLastHeart()
{
	return m_tLastHeart;
}

unsigned char* NetConnect::PrepareBuffer(unsigned short uRecvSize)
{
	return m_recvBuffer.PrepareBuffer( uRecvSize );
}

void NetConnect::WriteFinished(unsigned short uLength)
{
	m_recvBuffer.WriteFinished( uLength );
}

bool NetConnect::IsReadAble()
{
	return m_bReadAble;
}

uint32 NetConnect::GetLength()
{
	return m_recvBuffer.GetLength();
}

bool NetConnect::ReadData( unsigned char* pMsg, unsigned short uLength, bool bClearCache )
{
	m_bReadAble = m_recvBuffer.ReadData( pMsg, uLength, bClearCache );
	if ( !m_bReadAble ) uLength = 0;
	
	return m_bReadAble;
}

bool NetConnect::SendData( const unsigned char* pMsg, unsigned short uLength )
{
	try
	{
		unsigned char *ioBuf = NULL;
		int nSendSize = 0;
		AutoLock lock(&m_sendMutex);//�ظ�������֪ͨ���ڲ���send
		if ( 0 >= m_sendBuffer.GetLength() )//û�еȴ����͵����ݣ���ֱ�ӷ���
		{
			nSendSize = m_socket.Send( pMsg, uLength );
		}
		if ( -1 == nSendSize ) return false;//�����������ӿ����ѶϿ�
		if ( uLength == nSendSize ) return true;//���������ѷ��ͣ����سɹ�
		
		//���ݼ��뷢�ͻ��壬�����ײ�ȥ����
		uLength -= nSendSize;
		while ( true )
		{
			if ( uLength > 256 )
			{
				ioBuf = m_sendBuffer.PrepareBuffer( 256 );
				memcpy( ioBuf, &pMsg[nSendSize], 256 );
				m_sendBuffer.WriteFinished( 256 );
				nSendSize += 256;
				uLength -= 256;
			}
			else
			{
				ioBuf = m_sendBuffer.PrepareBuffer( uLength );
				memcpy( ioBuf, &pMsg[nSendSize], uLength );
				m_sendBuffer.WriteFinished( uLength );
				break;
			}
		}
		if ( !SendStart() ) return true;//�Ѿ��ڷ���
		//�������̿�ʼ
		m_pNetMonitor->AddSend( m_socket.GetSocket(), NULL, 0 );
	}
	catch(...){}
	return true;
}

Socket* NetConnect::GetSocket()
{
	return &m_socket;
}

int NetConnect::GetID()
{
	return m_id;
}

//��ʼ��������
bool NetConnect::SendStart()
{
	if ( 0 != AtomAdd(&m_nSendCount,1) ) return false;//ֻ�������һ����������
	return true;
}

//������������
void NetConnect::SendEnd()
{
	m_nSendCount = 0;
}

void NetConnect::Close()
{
	m_pEngine->CloseConnect(m_id);
}

bool NetConnect::IsInGroups( int *groups, int count )
{
	int i = 0;
	for ( i = 0; i < count; i++ )
	{
		if ( m_host.m_groups.end() != m_host.m_groups.find(groups[i]) ) return true;
	}
	
	return false;
}

}//namespace mdk

