#ifndef MDK_NET_ENGINE_H
#define MDK_NET_ENGINE_H

#include "mdk/Socket.h"
#include "mdk/Thread.h"
#include "mdk/Lock.h"
#include "mdk/ThreadPool.h"
#include "mdk/FixLengthInt.h"
#include "mdk/MemoryPool.h"

#include <map>
#include <vector>
#include <string>

namespace mdk
{
void XXSleep( long lSecond );
class Mutex;
class NetConnect;
class NetHost;
class NetEventMonitor;
class NetServer;
class MemoryPool;
typedef std::map<SOCKET,NetConnect*> ConnectList;
typedef std::vector<NetConnect*> ReleaseList;
	
/**
 * ������ͨ��������
 * ͨ�Ų��������
 * ʹ��һ��ͨ�Ų��ԣ�IOCP��EPoll��ͳ��select�ȣ�����ͨ�ſ���
 * 
 * ��Ա
 * �������������
 * �ͻ����������ӳ���
 * �ӿ�
 * ����
 * ֹͣ
 * 
 * ����
 * ����������
 * �Ͽ�����
 * ��Ϣ����
 * 
 */
enum connectState
{
	ok = 0,
	unconnect = 1,
	wait_recv = 2,
	wait_send = 3,
};
class NetEngine
{
	friend class NetServer;
protected:
	std::string m_startError;//����ʧ��ԭ��
	MemoryPool *m_pConnectPool;//NetConnect�����
	int m_averageConnectCount;//ƽ��������
	bool m_stop;//ֹͣ��־
	/**
		���ӱ�
		map<unsigned long,NetConnect*>
		��ʱ�����б����������з���������
		��û�������ģ��Ͽ����ӣ�����m_closedConnects�еȴ��ͷ�
	*/
	ConnectList m_connectList;
	/**
		�ѶϿ��������б�
		vector<NetConnect*>
		ʹ��NetConnect::GetOwner()����,ȷ�������Ƿ���ڷ��ʡ�
		��ʱ�����б��ж����Ƿ����˳������̣߳�ִ���ͷ�
	*/
	ReleaseList m_closedConnects;
	Mutex m_connectsMutex;//�����б���ʿ���
	Mutex m_closedConnectsMutex;//�ѹر������б���ʿ���
	int m_nHeartTime;//�������(S)
	int m_nReconnectTime;//�Զ�����ʱ��(S)
	Thread m_mainThread;
	NetEventMonitor *m_pNetMonitor;
	ThreadPool m_ioThreads;//io�̳߳�
	int m_ioThreadCount;//io�߳�����
	ThreadPool m_workThreads;//ҵ���̳߳�
	int m_workThreadCount;//ҵ���߳�����
	NetServer *m_pNetServer;
	std::map<int,SOCKET> m_serverPorts;//�ṩ����Ķ˿�,key�˿ڣ�value״̬��������˿ڵ��׽���
	Mutex m_listenMutex;//������������
	std::map<uint64,SOCKET> m_serIPList;//���ӵ��ⲿ�����ַ�б�
	Mutex m_serListMutex;//���ӵķ����ַ�б���
protected:
	//�����¼������߳�
	virtual void* NetMonitor( void* ) = 0;
	void* NetMonitorTask( void* );
	//��Ӧ�����¼�,sockΪ�����ӵ��׽���
	bool OnConnect( SOCKET sock, bool isConnectServer );
	void* ConnectWorker( NetConnect *pConnect );//ҵ��㴦������
	//��Ӧ�ر��¼���sockΪ�رյ��׽���
	void OnClose( SOCKET sock );
	void* CloseWorker( NetConnect *pConnect );//ҵ��㴦��ر�
	//��Ӧ���ݵ����¼���sockΪ�����ݵ�����׽���
	connectState OnData( SOCKET sock, char *pData, unsigned short uSize );
	/*
		��������
		��������״̬
		�������Ӧ�������Ҫ��������ʵ��
	*/
	virtual connectState RecvData( NetConnect *pConnect, char *pData, unsigned short uSize );
	void* MsgWorker( NetConnect *pConnect );//ҵ��㴦����Ϣ
	connectState OnSend( SOCKET sock, unsigned short uSize );//��Ӧ�����¼�
	virtual connectState SendData(NetConnect *pConnect, unsigned short uSize);//��������
	virtual bool MonitorConnect(NetConnect *pConnect);//��������
	virtual SOCKET ListenPort(int port);//����һ���˿�,���ش������׽���
	//��ĳ�����ӹ㲥��Ϣ(ҵ���ӿ�)
	void BroadcastMsg( int *recvGroupIDs, int recvCount, char *msg, int msgsize, int *filterGroupIDs, int filterCount );
	void SendMsg( int hostID, char *msg, int msgsize );//��ĳ����������Ϣ(ҵ���ӿ�)
private:
	//���߳�
	void* Main(void*);
	//�����߳�
	void HeartMonitor();
	//�ر�һ�����ӣ���socket�Ӽ�������ɾ��
	NetConnect* CloseConnect( ConnectList::iterator it );

	//////////////////////////////////////////////////////////////////////////
	//����˿�
	bool ListenAll();//��������ע��Ķ˿�
	//////////////////////////////////////////////////////////////////////////
	//����������������
	SOCKET ConnectOtherServer(const char* ip, int port);//����һ������,��������׽���
	bool ConnectAll();//��������ע��ķ��������ӵĻ��Զ�����
	void SetServerClose(NetConnect *pConnect);//���������ӵķ���Ϊ�ر�״̬
	const char* GetInitError();//ȡ������������Ϣ
public:
	/**
	 * ���캯��,�󶨷�������ͨ�Ų���
	 * 
	 */
	NetEngine();
	virtual ~NetEngine();

	//����ƽ��������
	void SetAverageConnectCount(int count);
	//��������ʱ��
	void SetHeartTime( int nSecond );
	//�����Զ�����ʱ��
	void SetReconnectTime( int nSecond );
	//��������IO�߳�����
	void SetIOThreadCount(int nCount);
	//���ù����߳���
	void SetWorkThreadCount(int nCount);
	/**
	 * ��ʼ
	 * �ɹ�����true��ʧ�ܷ���false
	 */
	bool Start();
	//ֹͣ
	void Stop();
	//�ȴ�ֹͣ
	void WaitStop();
	//�ر�һ���������,ͨ�Ų㷢���������ر�����ʱ����������ýӿ�
	void CloseConnect( SOCKET sock );
	//����һ���˿�
	bool Listen( int port );
	/*
		����һ������
		heartMsg�Ǹ÷�����Ҫ�������������
		����Է�û��Ҫ����������len����Ϊ0��
	*/
	bool Connect(const char* ip, int port);
};

}  // namespace mdk
#endif //MDK_NET_ENGINE_H
