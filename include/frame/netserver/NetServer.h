#ifndef MDK_C_NET_SERVER_H
#define MDK_C_NET_SERVER_H

#include "mdk/Thread.h"

namespace mdk
{
	class NetEngine;
	class NetHost;
/**
 * �����������
 * ������Ϣ��ִ��ҵ����
 * 
 */
class NetServer
{
	friend class NetEngine;
private:
	/*
	 * ���������������������ִ���ض�������ͨ�Ų���(IOCP��EPoll��ͳ��select)
	 * Ҫ����������ԣ�ֻҪ������������༴��
	 * 
	 */
	NetEngine* m_pNetCard;
	//���߳�
	Thread m_mainThread;
	bool m_bStop;
	
protected:
	void* TMain(void* pParam);
	/*
	 *	�������ص����߳�
	 *	Start()��
	 *	�û����Ժ��Դ˷������Լ����ⴴ�����߳�
	 */
	virtual void* Main(void* pParam){ return 0; }
	
	/**
	 * �������¼��ص�����
	 * 
	 * ������ʵ�־�������ҵ����
	 * 
	 */
	virtual void OnConnect(NetHost* pClient){}
	/**
	 * ���ӹر��¼����ص�����
	 * 
	 * ������ʵ�־���Ͽ�����ҵ����
	 * 
	 */
	virtual void OnCloseConnect(NetHost* pClient){}
	/**
	 * ���ݵ���ص�����
	 * 
	 * ������ʵ�־���Ͽ�����ҵ����
	 * 
	*/
	virtual void OnMsg(NetHost* pClient){}

	//��������������true�����򷵻�false
	bool IsOk();
 
public:
	NetServer();
	virtual ~NetServer();
	/**
	 * ���з�����
	 * �ɹ�����NULL
	 * ʧ�ܷ���ʧ��ԭ��
	 */
	const char* Start();
	/**
	 * �رշ�����
	 */
	void Stop();
	/*
	 *	�ȴ�������ֹͣ
	 */
	void WaitStop();

	//���õ������������̿��ܳ��ص�ƽ����������Ĭ��5000
	void SetAverageConnectCount(int count);
	//��������ʱ�䣬�������򣬷��������������
	void SetHeartTime( int nSecond );
	//��������IO�߳�����
	void SetIOThreadCount(int nCount);
	//���ù����߳���
	void SetWorkThreadCount(int nCount);
	//����ĳ���˿ڣ��ɶ�ε��ü�������˿�
	bool Listen(int port);
	//�����ⲿ���������ɶ�ε������Ӷ���ⲿ������
	//����Ҫ��������������δ���˲��ԣ����ܳ���bug
	bool Connect(const char *ip, int port);
	/*
		�㲥��Ϣ
			���NetHost::InGroup(),NetHost::OutGroup()ʹ��
			������recvGroupIDs������һ�飬ͬʱ���˵�����filterGroupIDs������һ���������������Ϣ
		���磺
			A B C3������
			A���� 1 2 4
			B���� 2 3 5
			C���� 1 3 5
			D���� 2 3
			E���� 3 5
			
			BroadcastMsg( {1,3}, 2, msg, len, {5}, 1 );
			�����ڷ���1�����ڷ���3,ͬʱ�����ڷ���5������������Ϣ����AD�����յ���Ϣ��BCE������

		Ҳ���Բ������3���������Լ������������ӷ���
  
	 */
	void BroadcastMsg( int *recvGroupIDs, int recvCount, char *msg, int msgsize, int *filterGroupIDs, int filterCount );
	/*
		��ĳ����������Ϣ

		1.��NetHost::Send()������
			SendMsg()�ڲ����������б��У��������Ҷ���Ȼ���ǵ���NetHost::Send()������Ϣ
		���Ѿ��õ�NetHost���������£�ֱ��NetHost::Send()Ч����ߣ��Ҳ�������������
		
		2.ʲôʱ����SendMsg()ʲôʱ��ֱ��NetHost::Send()
			OnMsg() OnConnect()�ṩ�˵�ǰNetHost���󣬿���ֱ��NetHost::Send()

			�����Ҫ���������ӷ�����Ϣ����û�����ȱ���NetHost�����Ǿ�ֻ��SendMsg
			������ȱ�����NetHost�Ķ�����ο�NetHost::Hold()����
		
	 */
	void SendMsg( int hostID, char *msg, int msgsize );
	/*
	 	�ر�������������
	 */
	void CloseConnect( int hostID );
};

}  // namespace mdk
#endif //MDK_C_NET_SERVER_H
