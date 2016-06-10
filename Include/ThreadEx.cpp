#include "ThreadEx.h"
#include <boost\progress.hpp>
#include <boost\date_time.hpp>
using namespace boost;

DWORD ThreadEx::m_nThreadID = 0;//2.0
								//声明时创建资源
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
//析构时设置参数等待线程主函数返回,需要等待1秒
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
//创建线程
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
//继续执行挂起的线程
bool  ThreadEx::Resume(void)//中断1,2取消:释放
{
	if (p_thread != 0)
	{
		m_suspended = false;
		m_condition.notify_one();
		return true;
	}
	else return false;
}
//挂起线程
bool  ThreadEx::Suspend(void)//中断2:挂起
{
	if (p_thread != 0)
	{
		m_suspended = true;
		m_condition.notify_one();
		return true;
	}
	else return false;
}
//设置参数使主函数退出以销毁线程
void  ThreadEx::Destroy(void)//中断0:退出
{
	m_bExit = true;
	m_times = -1;
	m_timesBack = 0;
	this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_frameTime + 100));
	m_ThStatus = TH_EXIT;
	p_thread = 0;
}
//调用WindowsAPI强制结束当前线程
void ThreadEx::ForceEnd(void)
{
	Destroy();
	Resume();
}

//线程功能
//线程主函数 :线程创建后开始执行,处理任务Task和消息,主函数退出后线程销毁
void ThreadEx::ThreadMain(void* thisObj)
{
	ThreadEx* pThisObj = (ThreadEx*)thisObj;
	while (pThisObj->m_suspended) pThisObj->m_condition.wait(pThisObj->m_mutex);//中断2:挂起
	while (!pThisObj->m_bExit && pThisObj->m_times != 0)//中断0:退出
	{
		//任务处理部分
		pThisObj->m_taskTime = 0;//3.1
		timer arWatch;
		pThisObj->m_ThStatus = ThreadEx::TH_SUSPEND;
		while (pThisObj->m_suspended) pThisObj->m_condition.wait(pThisObj->m_mutex);//中断2:挂起
		if (pThisObj->m_bExit) break;//中断0:退出
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

		if (pThisObj->m_times > 0)//中断3计数退出
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
//任务功能
//虚任务函数,派生类重载此函数,静态ThreadExMain函数会调用派生类的Task()
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