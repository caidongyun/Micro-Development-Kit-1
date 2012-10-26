#ifndef MDK_NETHOST_H
#define MDK_NETHOST_H

#include "mdk/FixLengthInt.h"
#include <map>

namespace mdk
{
	
class NetConnect;
class Socket;
/**
	����������
	��ʾ�����е�һ�����ӹ���������
	ҵ���ͨ�Žӿڶ���
	�ṩ���ͽ��չرղ���
	�ṩ���ʰ�ȫ����Hold��Free

 	��hostIDʵ�ʾ�������������ӵ�SOCKET�����������ֱ��ʹ��socket���api��������socket��io��close��
 	��Ϊcloseʱ���ײ���Ҫ�������������ֱ��ʹ��socketclose()����ײ����û����ִ��������,������Ӳ�����
 	io�����ײ��Ѿ�ʹ��io�������ֱ��ʹ��api io������io��������һ���ײ�io���������������ݴ���
*/
class NetHost
{
	friend class NetConnect;
private:
	NetConnect* m_pConnect;//���Ӷ���ָ��,����NetConnect��ҵ���ӿڣ�����NetConnect��ͨ�Ų�ӿ�
	bool m_bIsServer;//�������ͷ�����
	std::map<int,int> m_groups;//��������
private://˽�й��죬����ֻ��NetConnect�ڲ����������ͻ�ֻ��ʹ��
	NetHost( bool bIsServer );

public:
	~NetHost();
	int ID();//����ID
	//ȡ���Ѿ��������ݳ���
	uint32 GetLength();
	//�ӽ��ջ����ж����ݣ����ݲ�����ֱ�ӷ���false��������ģʽ
	//bClearCacheΪfalse���������ݲ���ӽ��ջ���ɾ�����´λ��Ǵ���ͬλ�ö�ȡ
	bool Recv(unsigned char* pMsg, unsigned short uLength, bool bClearCache = true );
	/**
	 * ��������
	 * ��������Чʱ������false
	 */
	bool Send(const unsigned char* pMsg, unsigned short uLength);
	void Close();//�ر�����
	/**
	 * ���ֶ���
	 * ������Free()�ɶԵ���
	 * �ͻ��п�����Ҫ����������������Ȼ������Ҫ����ʹ��
	 * ��Ҫ���ֶ��󣬱�����ɾ������������
	 */
	void Hold();
	/**
	 * �ͷŶ���
	 * ������Hold()�ɶԵ���
	 * �ÿ�ܵõ����ԶԸ�����������л��տ���
	 */
	void Free();
	//������һ������
	bool IsServer();
	//////////////////////////////////////////////////////////////////////////
	//ҵ��ӿڣ���NetEngine::BroadcastMsgż��
	void InGroup( int groupID );//����ĳ���飬ͬһ�������ɶ�ε��ø÷��������������飬���̰߳�ȫ
	void OutGroup( int groupID );//��ĳ����ɾ�������̰߳�ȫ
private:
};

}  // namespace mdk
#endif//MDK_NETHOST_H
