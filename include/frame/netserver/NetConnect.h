// NetConnect.h: interface for the NetConnect class.
//
//////////////////////////////////////////////////////////////////////
/*
	����������
	ͨ�Ų���󣬶�ҵ��㲻�ɼ�
 */
#ifndef MDK_NETCONNECT_H
#define MDK_NETCONNECT_H

#include <time.h>
#include "NetHost.h"
#include "mdk/Lock.h"
#include "mdk/IOBuffer.h"
#include "mdk/Socket.h"

namespace mdk
{
class NetEventMonitor;
class Socket;
class NetEngine;
class NetConnect  
{
	friend class NetEngine;
	friend class IOCPFrame;
	friend class EpollFrame;
public:
	NetConnect(SOCKET sock, bool bIsConnectServer, NetEventMonitor *pNetMonitor, NetEngine *pEngine);
	virtual ~NetConnect();

public:
	/**
	 * ͨ�Ų�ӿ�
	 * ȡ��ӵ����
	 * �ͷ��̷߳���ȷ���ɷ��ͷ�
	 */
	bool IsFree();
	/**
	  ҵ��㿪ʼ����
	  m_uWorkAccessCount++
	 */
	void WorkAccess();
	/**
	  ҵ�����ɷ���
	  m_uWorkAccessCount--
	 */
	void WorkFinished();
	/*
	* ׼��Buffer
	* Ϊд��uLength���ȵ�����׼�����壬
	* д�����ʱ�������WriteFinished()��ǿɶ����ݳ���
	*/
	unsigned char* PrepareBuffer(unsigned short uRecvSize);
	/**
	 * д�����
	 * ���д�����д�����ݵĳ���
	 * ������PrepareBuffer()�ɶԵ���
	 */
	void WriteFinished(unsigned short uLength);
	/*
	 *	���ṩbool WriteData( char *data, int nSize );�ӿ�
	 *	��д���ݣ���Ϊ�˱���COPY���������Ч��
	 *	����Ҳͳһ��IOCP��EPOLL������д�뷽ʽ
	 */

	int GetID();//ȡ��ID
	Socket* GetSocket();//ȡ���׽���
	bool IsReadAble();//�ɶ�
	uint32 GetLength();//ȡ�����ݳ���
	//�ӽ��ջ����ж����ݣ����ݲ�����ֱ�ӷ���false��������ģʽ
	//bClearCacheΪfalse���������ݲ���ӽ��ջ���ɾ�����´λ��Ǵ���ͬλ�ö�ȡ
	bool ReadData(unsigned char* pMsg, unsigned short uLength, bool bClearCache = true );
	bool SendData( const unsigned char* pMsg, unsigned short uLength );
	bool SendStart();//��ʼ��������
	void SendEnd();//������������
	void Close();//�ر�����
		
	//ˢ������ʱ��
	void RefreshHeart();
	//ȡ���ϴ�����ʱ��
	time_t GetLastHeart();
	bool IsInGroups( int *groups, int count );//����ĳЩ����
	
private:
	/**
	 * �ö���ǰӵ����
	 * enum Owner����
	 * 0��ӵ���߿����ͷ�
	 * 1ͨ�Ų�
	 * 2ҵ���
	 * 
	 * ����ӱ�������ʼ����Ӧ������Ϊ1
	 * ����ҵ��㱣�浽�ڴ�ʱ��Ӧ������Ϊ2
	 * ����ҵ�����ڴ�ɾ��ʱ��Ӧ������Ϊ1
	 * 
	 * �޷�����ӵ����Ϊ�����ʱ����������Ϊ0
	 * ����ÿ���˳�ҵ������͹ر���������ʱ��Ӧ�ü����ʼ�����ӵ���ߣ������Ƿ�Ӧ�ý�ӵ��������Ϊ0��
	 * �ر���������ʱ�����2�ַ��ʼ�����Ϊ0����ӵ����ȴ��ҵ��㣬��һ����ҵ����߼������ͷ�ӵ�����ԣ�������ҵ�����̣����
	 * ��Ϊ������ʼ���Ϊ0�ˣ���һ������������ҵ����ʣ���ҵ�����Ϊ0�ˣ����һ��ҵ�����Ӧ���ǶϿ�����ҵ��ҵ�������ͷ�ӵ������
	 * 
	 * ���ؼӷ�����
	 * 1.��֤OnCloseConnect()һ����OnConnect()��ɺ�ſ���������
	 * ��˾ͱ�֤�ˣ������ڳ���ҵ����ͷ�ռ��Ȩ�޺��ֻ�ȡռ��Ȩ�ޣ����ͻ����벻�ù��ģ���������������
	 * 
	 * 2.��ҵ���ϱ�֤ҵ����ͷŶ���󣬾Ͳ�����ռ�ж���
	 * �����֤�ˣ����ʼ���Ϊ0ʱ�����԰�ȫ���ͷŶ���
	 */
	int m_owner;
	unsigned int m_uWorkAccessCount;//ҵ�����ʼ���
	Mutex m_lockWorkAccessCount;//ҵ�����ʼ�����
	IOBuffer m_recvBuffer;//���ջ���
	int m_nReadCount;//���ڽ��ж����ջ�����߳���
	bool m_bReadAble;//io�����������ݿɶ�
	bool m_bConnect;//��������

	IOBuffer m_sendBuffer;//���ͻ���
	int m_nSendCount;//���ڽ��з��͵��߳���
	bool m_bSendAble;//io��������������Ҫ����
	Mutex m_sendMutex;//���Ͳ���������
	
	Socket m_socket;//socketָ�룬���ڵ����������
	NetEventMonitor *m_pNetMonitor;//�ײ�Ͷ�ݲ����ӿ�
	NetEngine *m_pEngine;//���ڹر�����
	int m_id;
	NetHost m_host;
	time_t m_tLastHeart;//���һ���յ�����ʱ��

};

}//namespace mdk

#endif // MDK_NETCONNECT_H
