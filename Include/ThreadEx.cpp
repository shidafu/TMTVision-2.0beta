#include "ThreadEx.h"
#include <boost\progress.hpp>
#include <boost\date_time.hpp>
using namespace boost;

DWORD ThreadEx::m_nThreadID = 0;//2.0
								//����ʱ������Դ
ThreadEx::ThreadEx()//2.0
{
	m_mutex.unlock();
	m_suspended = true;
	m_section.lock();
	m_bExit = false;
	//m_hEvt = 0;
	p_thread = 0;
	m_times = -1;
	m_timesBack = 0;
	//m_hParent = hParent;//2.0
	m_nThreadID = m_nThreadID + 1;//2.0
	m_waitTime = 0;
	m_frameTime = 0;
	m_taskTime = 0;
	m_includeTaskTime = false;//3.1
	m_ThStatus = TH_EXIT;
	m_section.unlock();
}
//����ʱ���ò����ȴ��߳�����������,��Ҫ�ȴ�1��
ThreadEx::~ThreadEx(void)
{
	m_mutex.unlock();
	m_section.lock();
	//Destroy();
	ForceEnd();
	this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_frameTime + 100));
	m_ThStatus = TH_EXIT;
	m_section.unlock();
}
//�����߳�
bool  ThreadEx::Create(long times, long waiteTime, bool includeTaskTime)//2.0
{
	if (p_thread == 0)
	{
		m_times = times;
		m_timesBack = 0;
		m_bExit = false;
		m_waitTime = MAX(waiteTime, 0);
		m_includeTaskTime = includeTaskTime;//3.1
		Suspend();
		//Resume();
		p_thread = new thread(ThreadEx::ThreadMain,(void*)this);
		//p_thread = (HANDLE)_beginThreadEx(ThreadExMain, 0, this);
		if (p_thread != 0) m_ThStatus = TH_AVIALABLE;
		return true;
	}
	else return false;
}
//����ִ�й�����߳�
bool  ThreadEx::Resume(void)//�ж�1,2ȡ��:�ͷ�
{
	if (p_thread != 0)
	{
		m_suspended = false;
		m_condition.notify_one();
		return true;
	}
	else return false;
}
//�����߳�
bool  ThreadEx::Suspend(void)//�ж�2:����
{
	if (p_thread != 0)
	{
		m_suspended = true;
		m_condition.notify_one();
		return true;
	}
	else return false;
}
//���ò���ʹ�������˳��������߳�
void  ThreadEx::Destroy(void)//�ж�0:�˳�
{
	m_bExit = true;
	m_times = -1;
	m_timesBack = 0;
	this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_frameTime + 100));
	m_ThStatus = TH_EXIT;
	p_thread = 0;
}
//����WindowsAPIǿ�ƽ�����ǰ�߳�
void ThreadEx::ForceEnd(void)
{
	Destroy();
	Resume();
}

//�̹߳���
//�߳������� :�̴߳�����ʼִ��,��������Task����Ϣ,�������˳����߳�����
void ThreadEx::ThreadMain(void* thisObj)
{
	ThreadEx* pThisObj = (ThreadEx*)thisObj;
	while (pThisObj->m_suspended) pThisObj->m_condition.wait(pThisObj->m_mutex);//�ж�2:����
	while (!pThisObj->m_bExit && pThisObj->m_times != 0)//�ж�0:�˳�
	{
		//��������
		pThisObj->m_taskTime = 0;//3.1
		timer arWatch;
		pThisObj->m_ThStatus = ThreadEx::TH_SUSPEND;
		while (pThisObj->m_suspended) pThisObj->m_condition.wait(pThisObj->m_mutex);//�ж�2:����
		if (pThisObj->m_bExit) break;//�ж�0:�˳�
		pThisObj->m_ThStatus = ThreadEx::TH_RUNNING;
		if (pThisObj->m_includeTaskTime)//3.1
		{
			try
			{
				pThisObj->Task();//3.0
			}
			catch (...)
			{
				pThisObj->m_section.unlock();
			}

		}
		else
		{
			try
			{
				pThisObj->Task();//3.0
			}
			catch (...)
			{
				pThisObj->m_section.unlock();
			}
		}

		if (pThisObj->m_times > 0)//�ж�3�����˳�
		{
			pThisObj->m_times--;
		}
		pThisObj->m_timesBack++;
		pThisObj->m_taskTime = arWatch.elapsed()*1000;
		arWatch.restart();
		if (pThisObj->m_includeTaskTime)
		{
			pThisObj->p_thread->sleep(boost::get_system_time() + posix_time::milliseconds(MAX(pThisObj->m_waitTime - pThisObj->m_taskTime, 0)));
		} 
		else
		{
			pThisObj->p_thread->sleep(boost::get_system_time() + posix_time::milliseconds(MAX(pThisObj->m_waitTime, 0)));
		}
		pThisObj->m_frameTime = pThisObj->m_taskTime + arWatch.elapsed() * 1000;
		std::cout << "times:"<< pThisObj->m_timesBack << ",taskTime: " << pThisObj->m_taskTime << ",frameTime: " << pThisObj->m_frameTime << std::endl;
	}
	pThisObj->m_ThStatus = ThreadEx::TH_EXIT;
}
//������
//��������,���������ش˺���,��̬ThreadExMain����������������Task()
int a = 0;
void  ThreadEx::Task(void)
{
	//m_section.lock();
	{
		//progress_timer t;
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(100));
	}
	//m_section.unlock();
}
//};
//==============================================================================
///</class_info>