/* \file BThread_base.h
  \brief Thread manager using Boost library
  \author Leon Contact: towanglei@163.com
  \copyright TMTeam
  \version 5.0
  \History:
	 2016/06/19 21:08 Add Click() for to one time in suspended status.\n
     4.1 : 2016/06/12 00:36 Add Click() for to one time in suspended status.\n
     4.0 : 2016/06/10 12:03 Use C++ 11 boost library.\n
     3.2 : 2016/05/12 12:03 Add more details of time management: m_actualWaitTime.\n
     3.1 : 2016/04/29 21:17 Fix comments.\n
     3.0 : Leon 2016/03/29 06:00 Add m_includeTaskTime = true/false means task time include in m_waitTime or not.\n
     2.0 : Leon 2016/03/29 06:00 Add m_hParent,m_nThreadID,m_waitTime.\n
     1.0 : Leon 2014/05/01 19:03 build.
*/
#pragma once
#ifndef BTHREAD_BASE
#define BTHREAD_BASE
//#include "..\Macros.hpp"
#include <boost\thread.hpp>
#include <boost\progress.hpp>
#include <boost\date_time.hpp>

#ifndef HANDLE
typedef void *HANDLE;
//#define HANDLE              void*
#define INVALID_HANDLE      (HANDLE)-1
#define NULL_HANDLE         (HANDLE)0
#endif

#ifndef NULL_POINTER
#define NULL_POINTER        (void*)0
#endif

#ifndef MIN
#define MIN(i,j)            i<j?i:j
#endif
#ifndef MAX
#define MAX(i,j)            i<j?j:i
#endif
#ifndef MOD
#define MOD(i,j)            i%j==j?0:i%j
#endif
#ifndef NULL
#define NULL                0
#endif

using namespace boost;

/* \class BThread : 
  \brief Thread manager using Boost library
  \note
  1:Create thread to run ThreadMain, destroy thread after ThreadMain return;\n
  2:The core function ThreadExMain has several interruption point, to implement:\n
    Suspend() vs Resume(),\n
    Destroy() or ForceEnd(),\n
    Create(long times = -1, long waitTime = 0, bool includeTaskTime = false),\n
           times: run times count, -1 means infinite,
           waitTime: sleep times(ms) between task,
           includeTaskTime: if task time included in wait time count.\n
  3:ThreadExMain run Task() function several times until encounter exit/suspend/sleep interruption,\n
           the Task() function is virtual and implimented by externded class.\n

  \author Leon Contact: towanglei@163.com
  \version 3.0
  \date 2016/04/29 22:34
*/
class BThread
{
//Thread tags
public:
	/// Class ID to distinct base and extend class.
	const int m_classID = 0;//3.0
	HANDLE  m_hThread = INVALID_HANDLE;///< BThread handle
	HANDLE  m_hParent = INVALID_HANDLE;;///< Parent handle

//Thread paras:
protected:
	bool m_bExit = false;///< Exit flag.
	long m_times = -1;///< Run times,-1 means infinite.
	long m_timesBack = 0;///< Run times,-1 means infinite.
	long m_waitTime = 0;///< Expected time(ms) from task end/start to next task start.
	double m_frameTime = 0;///< Actual frame time(ms) 4.0.
	double m_taskTime = 0;///< Actual task time(ms) from task start to end 3.1.
	bool m_includeTaskTime = false;///< If m_waitTime include task time, false/true is end/start to start.
	
	/// Actual frame time(ms), decided by driver or link, -1 means unused.
	double GetFrameTime()
	{
		return m_frameTime;
	}
	/// Actual sample time(ms), decided by driver or link, -1 means unused.
	double GetSampleTime()
	{
		return m_taskTime;
	}

//Thread status:
	/* \enum THSTATUS
	 \brief BThread status
	*/
	enum THSTATUS
	{
		TH_EXIT = 0, ///< Not initialed BThread.
		TH_AVIALABLE =1, ///< Initialed BThread but not run.
		TH_RUNNING = 2, ///< Running BThread.
		TH_SUSPEND = 3 ///< Suspend BThread.
	};
	/// BThread status.
	THSTATUS m_ThStatus = TH_EXIT;

//Thread methods:
public:
	/// Create source when construct.
	BThread(HANDLE hParent = INVALID_HANDLE)
	{
		m_suspendMutex.unlock();
		m_suspended = true;
		m_section.lock();
		m_bExit = false;
		p_thread = 0;
		m_times = -1;
		m_timesBack = 0;
		//m_nThreadID = m_nThreadID + 1;//2.0
		m_waitTime = 0;
		m_frameTime = 0;
		m_taskTime = 0;
		m_includeTaskTime = false;//3.1
		m_ThStatus = TH_EXIT;
		m_hParent = hParent;
		m_section.unlock();
	}
	/// Destruct use ForceEnd().
	~BThread(void)
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
	/// Create thread.
	bool  Create(long times = -1, long waitTime = 0, bool includeTaskTime = false) 
	{
		if (p_thread == 0)
		{
			m_times = times;
			m_timesBack = 0;
			m_bExit = false;
			m_waitTime = MAX(waitTime, 0);
			m_includeTaskTime = includeTaskTime;//3.1
			Suspend();
			p_thread = new thread(BThread::ThreadMain, (void*)this);
			if (p_thread != 0) m_ThStatus = TH_AVIALABLE;
			m_hThread = p_thread->native_handle();
			return true;
		}
		else return false;
	}
	/// Resume suspended thread.
	bool  Resume(void)
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
	/// Suspend thread
	bool  Suspend(void)
	{
		if (p_thread != 0)
		{
			if (m_ThStatus != BThread::TH_SUSPEND)
			{
				m_suspended = true;
				m_suspendCondition.notify_one();
				this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(m_waitTime + 10));
			} 
			else
			{
			}
			return true;
		}
		else return false;
	}
	/// Stop thread and destroy source, invalid when suspended,
	bool  Destroy(void)
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
	/// Force Stop thread and destroy source, valid when suspended.
	bool  ForceEnd(void)
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
	/// Click to run one time.
	bool  Click(bool isSync = false)
	{
		if (m_suspended != true && m_ThStatus != BThread::TH_SUSPEND)
		{
			return false;
		}
		if (p_thread != 0)
		{
			if (isSync)
			{
				Task();
			} 
			else
			{
				//m_suspended = false;
				m_suspendCondition.notify_one();
				//m_suspended = true;
			}
		    ////m_suspended = false;
			//m_suspendCondition.notify_one();
			////m_suspended = true;
			//if (isSync)
			//{
			//	while (m_ThStatus != BThread::TH_RUNNING)
			//	{
			//		this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(10));
			//		//std::cout << "m_ThStatus:" << m_ThStatus<< std::endl;
			//	}
			//}
			return true;
		}
		else return false;
	}

//Thread functions:
private:
	/// Mutex for suspend function.
	mutex m_suspendMutex;
	/// Flag for suspend function.
	bool m_suspended = true;
	/// Condition for suspend function.
	condition_variable_any m_suspendCondition;
	/// Mutex to avoid multi-thread conflicts.
	shared_mutex m_section;
	/// Thread Main function.
	static void ThreadMain(void* thisObj)
	{
		BThread* pThisObj = (BThread*)thisObj;
		pThisObj->m_suspended = true;
		//if (pThisObj->m_suspended) pThisObj->m_suspendCondition.wait(pThisObj->m_suspendMutex);//interruption:2 Suspend after create
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
			pThisObj->m_taskTime = arWatch.elapsed() * 1000;
			arWatch.restart();
			if (pThisObj->m_includeTaskTime)
			{
				pThisObj->p_thread->sleep(boost::get_system_time() + posix_time::milliseconds(MAX(pThisObj->m_waitTime - (long)pThisObj->m_taskTime, 0)));
			}
			else
			{
				pThisObj->p_thread->sleep(boost::get_system_time() + posix_time::milliseconds(MAX(pThisObj->m_waitTime, 0)));
			}
			pThisObj->m_frameTime = pThisObj->m_taskTime + arWatch.elapsed() * 1000;
			std::cout << "times:" << pThisObj->m_timesBack << ",taskTime: " << pThisObj->m_taskTime << ",frameTime: " << pThisObj->m_frameTime << std::endl;
		}
		pThisObj->m_ThStatus = BThread::TH_EXIT;
	}
	/// Thread Task, implement this function in extend class.
	virtual void  Task(void)
	{
		//m_section.lock();
		{
			progress_timer t;
			this_thread::sleep(boost::get_system_time() + posix_time::milliseconds(100));
		}
		//m_section.unlock();
	}
	/// Thread object.
	thread* p_thread=0;
};
#endif