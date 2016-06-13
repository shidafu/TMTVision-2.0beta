#include "BThread.h"
#include <boost\progress.hpp>
#include <boost\date_time.hpp>
using namespace boost;

DWORD BThread::m_nThreadID = 0;//2.0

BThread::BThread(HANDLE hParent)//2.0
{
	m_suspendMutex.unlock();
	m_suspended = true;
	m_section.lock();
	m_bExit = false;
	p_thread = 0;
	m_times = -1;
	m_timesBack = 0;
	m_nThreadID = m_nThreadID + 1;//2.0
	m_waitTime = 0;
	m_frameTime = 0;
	m_taskTime = 0;
	m_includeTaskTime = false;//3.1
	m_ThStatus = TH_EXIT;
	m_hParent = hParent;
	m_section.unlock();
}

BThread::~BThread(void)
{
	m_suspendMutex.unlock();
	m_section.lock();
	ForceEnd();
	this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
	while (m_ThStatus != BThread::TH_EXIT)
	{
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
	}
	m_section.unlock();
}

bool  BThread::Create(long times, long waitTime, bool includeTaskTime)//2.0
{
	if (p_thread == 0)
	{
		m_times = times;
		m_timesBack = 0;
		m_bExit = false;
		m_waitTime = MAX(waitTime, 0);
		m_includeTaskTime = includeTaskTime;//3.1
		Suspend();
		p_thread = new thread(BThread::ThreadMain,(void*)this);
		if (p_thread != 0) m_ThStatus = TH_AVIALABLE;
		m_hThread = p_thread->native_handle();
		return true;
	}
	else return false;
}

bool  BThread::Resume(void)
{
	if (p_thread != 0)
	{
		m_suspended = false;
		m_suspendCondition.notify_one();
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
		return true;
	}
	else return false;
}

bool  BThread::Suspend(void)
{
	if (p_thread != 0)
	{
		m_suspended = true;
		m_suspendCondition.notify_one();
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
		return true;
	}
	else return false;
}

bool  BThread::Destroy(void)
{
	if (p_thread == 0) return true;
	m_bExit = true;
	for (int i = 0; i < 8; i++)
	{
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
		if (m_ThStatus == BThread::TH_EXIT) break;
	}
	//while (m_ThStatus != BThread::TH_EXIT)
	//{
	//	this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
	//}
	if (m_ThStatus == BThread::TH_EXIT)
	{	
		m_times = -1;
		m_timesBack = 0;
		p_thread = 0;
		m_hThread = INVALID_HANDLE;
		return true;
	}
	return false;
}

bool BThread::ForceEnd(void)
{
	if (!Destroy())
	{
		p_thread->interrupt();
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
		m_ThStatus = BThread::TH_EXIT;
		m_times = -1;
		m_timesBack = 0;
		p_thread = 0;
		m_hThread = INVALID_HANDLE;
	}
	return true;
}

bool BThread::Click(bool wait)
{
	if (m_ThStatus != BThread::TH_SUSPEND)
	{
		return false;
	}
	if (p_thread != 0)
	{
		//m_suspended = false;
		m_suspendCondition.notify_one();
		//m_suspended = true;
		if (wait)
		{
			while (m_ThStatus != BThread::TH_RUNNING)
			{
				this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(10));
				//std::cout << "m_ThStatus:" << m_ThStatus<< std::endl;
			}
		}
		return true;
	}
	else return false;
}

void BThread::ThreadMain(void* thisObj)
{
	BThread* pThisObj = (BThread*)thisObj;
	if (pThisObj->m_suspended) pThisObj->m_suspendCondition.wait(pThisObj->m_suspendMutex);//interruption:2 Suspend after create
	while (!pThisObj->m_bExit && pThisObj->m_times != 0)//interruption:0 Exit
	{
		//Prepare, interruption and Task
		pThisObj->m_taskTime = 0;//3.1
		timer arWatch;
		pThisObj->m_ThStatus = BThread::TH_SUSPEND;
		if (pThisObj->m_suspended) pThisObj->m_suspendCondition.wait(pThisObj->m_suspendMutex);//interruption:2 Suspend
		//pThisObj->m_suspendMutex.unlock();
		if (pThisObj->m_bExit) break;//interruption:0 Exit after suspend before task
		pThisObj->m_ThStatus = BThread::TH_RUNNING;
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

		if (pThisObj->m_times > 0)//interruption:3 Time counter
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
	pThisObj->m_ThStatus = BThread::TH_EXIT;
}

void  BThread::Task(void)
{
	//m_section.lock();
	{
		//progress_timer t;
		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(100));
	}
	//m_section.unlock();
}