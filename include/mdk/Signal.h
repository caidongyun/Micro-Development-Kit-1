// Signal.h: interface for the Signal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDK_SINGLE_H
#define MDK_SINGLE_H

/*
	�ź���
	Ч��
	���û���߳��ڵȴ�
	NotifyAll�ᶪʧ
	Notify���ᶪʧ

	windows linux���
	���NotifyAllʱ������һ���߳����ڵ�Wait��
	���NotifyAll��windows�¿��ܻᶪʧ
	(��Ϊwindows�µ�Wait�Ὣ�ź�����Ϊ�ޣ�Ȼ���ٵȴ���
	���NotifyAll֮�󣬱���̼߳���֮ǰ��
	���ڵ���Wait���߳����ȵõ�cpuʱ��Ƭ�����ź������ˣ���Notifyall��ʧ)

	��linux���ᶪʧ
	

*/
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif


namespace mdk
{
	
class Signal  
{
public:
	Signal();
	virtual ~Signal();

	bool Wait( unsigned long nMillSecond = (unsigned long)-1 );
	bool Notify();
	bool NotifyAll();
	
private:
#ifdef WIN32
	HANDLE m_oneEvent;
	HANDLE m_allEvent;
#else
	int m_nSignal;
	bool m_bNotifyAll;
	pthread_cond_t m_cond;
	pthread_mutex_t m_condMutex;
#endif
};

}//namespace mdk

#endif // MDK_SINGLE_H
