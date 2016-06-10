///////////////////////////////////////////////////
/** \file ThreadEx.h
 *  \brief ThreadEx Class and ThreadExEx Class
 *
 *  \note
 *
 ** Status:
 *
 *            ThreadEx()━▶ ┏━━━━━━━┓ ◀━~ThreadEx()
 *                          ┃    TH_EXIT   ┃
 *                          ┗━━━━━━━┛ ◀━ForceEnd()
 *                            ┃          ▲
 *                            ▼          ┃
 *             Create()━▶ m_bExit==1  m_bExit==0 ◀━ Destroy()
 *                            ┃          ▲
 *                            ▼          ┃
 *                          ┏━━━━━━━┓
 *                          ┃ TH_AVIALABLE ┃
 *                          ┗━━━━━━━┛
 *                            ┃          ▲
 *                            ▼          ┃
 *                          p_Task      p_Task
 *             RegTask()━▶ !=NUll      ==NUllL ◀━ FreeTask()
 *                            ┃          ▲
 *                            ▼          ┃
 *                          ┏━━━━━━━┓
 *                          ┃  TH_RUNNING  ┃
 *                          ┗━━━━━━━┛
 *                            ┃          ▲
 *                            ▼          ┃
 *                          m_hEvt       m_hEvt
 *             Suspend()━▶ Unexist      Exist  ◀━ Resume()
 *          Destroy()invalide ┃          ▲
 *          ForceEnd() valide ▼          ┃
 *                          ┏━━━━━━━┓
 *                          ┃  TH_SUSPEND  ┃
 *                          ┗━━━━━━━┛
 *
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 4.0
 *  \History:
 *     2016/06/10 12:03 Use C++ 11 boost library.\n
 *     3.2 : 2016/05/12 12:03 Add more details of time management: m_actualWaitTime.\n
 *     3.1 : 2016/04/29 21:17 Fix comments.\n
 *     3.0 : Leon 2016/03/29 06:00 Add m_includeTaskTime = true/false means task time include in m_waitTime or not.\n
 *     2.0 : Leon 2016/03/29 06:00 Add m_hParent,m_nThreadID,m_waitTime.\n
 *     1.0 : Leon 2014/05/01 19:03 build.
 *
 */
///////////////////////////////////////////////////

#pragma once
#include "Macros.h"
#include <boost\thread.hpp>             // std::thread, std::this_thread::yield
//#include <thread>             // std::thread, std::this_thread::yield
//#include <mutex>              // std::mutex
//#include <condition_variable> // std::condition_variable_any
using namespace boost;

///////////////////////////////////////////////////
/** \class ThreadEx : 
 *  \brief 简单线程类,可派生重载任务函数
 *  \note
 *  线程池功能:\n
 *  1:声明后创建线程并执行ThreadExMain,线程在ThreadExMain退出时销毁,\n
 *  2:线程的核心是ThreadExMain主函数中负责决定线程是否退出或挂起,\n
 *    线程执行后可以手动挂起和更改线程 任务处理函数,\n
 *    派生类重载任务函数以实现不同功能\n
 *    当任务函数计数值=0,线程退出,计数初值设为-1,不计数\n
 *  3:线程主函数重复执行任务处理函数,主函数后包含中断处理\n
 *  ┏━━━━━━━━┓\n
 *  ┃线程            ┃ <-随对象声明创建,析构销毁\n
 *  ┠────────┨\n
 *  ┃线程主函数      ┃ <-线程创建Create后开始执行,处理任务Task和消息,主函数退出后线程销毁\n
 *  ┠───┬────┨\n
 *  ┃任务函数        ┃ <-线程执行中,循环执行任务函数并计数执行次数\n
 *  ┠───┴────┨\n
 *  ┃中断处理        ┃ <-确定主函数是否关起退出,ForceEnd中断0强制退出,m_times中断3计数退出\n
 *  ┗━━━━━━━━┛       Suspend中断2挂起等待,主函数循环暂停\n
 *  \author Leon Contact: towanglei@163.com
 *  \version 3.0
 *  \date 2016/04/29 22:34
 */
class ThreadEx
{
//类功能
public:
	/// ThreadEx ID，+1 after construct
	static DWORD m_nThreadID;//2.0
	/// 对象类型记录ID
	const int m_classID = 0;//3.0
	//HANDLE  p_thread;///< ThreadEx handle
	//HANDLE  m_hParent;///< Parent handle
	//HANDLE  m_hEvt;///< Suspend event

//线程参数
protected:
	bool m_bExit;///< Exit flag
	long m_times;///< Run times,-1 means infinite
	long m_timesBack;///< Run times,-1 means infinite
	long m_waitTime;///< Expected time(ms) from task end/start to next task start 
	double m_frameTime;///< Actual frame time(ms) 4.0
	double m_taskTime;///< Actual task time(ms) from task start to end 3.1 
	bool m_includeTaskTime;///< If m_waitTime include task time, false/true is end/start to start
	
	/// Actual frame time(ms), decided by driver or link, -1 means unused
	double GetFrameTime()
	{
		return m_frameTime;
	}
	/// Actual sample time(ms), decided by driver or link, -1 means unused
	double GetSampleTime()
	{
		return m_taskTime;
	}

//线程状态
	/** \enum THSTATUS
	*  \brief ThreadEx status
	*
	*  \note
	** Status:
	*
	*          ThreadEx()━▶ ┏━━━━━━━┓ ◀━~ThreadEx()
	*                          ┃    TH_EXIT   ┃
	*                          ┗━━━━━━━┛ ◀━ForceEnd()
	*                            ┃          ▲
	*                            ▼          ┃
	*             Create()━▶ m_bExit==1  m_bExit==0 ◀━ Destroy()
	*                            ┃          ▲
	*                            ▼          ┃
	*                          ┏━━━━━━━┓
	*                          ┃ TH_AVIALABLE ┃
	*                          ┗━━━━━━━┛
	*                            ┃          ▲
	*                            ▼          ┃
	*                          p_Task      p_Task
	*             RegTask()━▶ !=NUll      ==NUllL ◀━ FreeTask()
	*                            ┃          ▲
	*                            ▼          ┃
	*                          ┏━━━━━━━┓
	*                          ┃  TH_RUNNING  ┃
	*                          ┗━━━━━━━┛
	*                            ┃          ▲
	*                            ▼          ┃
	*                          m_hEvt       m_hEvt
	*             Suspend()━▶ Unexist      Exist  ◀━ Resume()
	*          Destroy()invalide ┃          ▲
	*          ForceEnd() valide ▼          ┃
	*                          ┏━━━━━━━┓
	*                          ┃  TH_SUSPEND  ┃
	*                          ┗━━━━━━━┛
	*
	*/
	enum THSTATUS
	{
		TH_EXIT = 0, ///< Not initialed ThreadEx 
		TH_AVIALABLE, ///< Initialed ThreadEx but not run
		TH_RUNNING, ///< Running ThreadEx
		TH_SUSPEND ///< Suspend ThreadEx 
	};
	/// ThreadEx status
	THSTATUS m_ThStatus;

//线程功能
public:
	/// Create source when construct
	ThreadEx();
	/// Destruct use ForceEnd()
	~ThreadEx(void); 
	/// 创建线程
	bool  Create(long times = -1, long waiteTime = 0, bool includeTaskTime = false);
	/// 继续执行挂起的线程
	bool  Resume(void);
	/// 挂起线程
	bool  Suspend(void);
	/// 设置参数使主函数退出以销毁线程
	void  Destroy(void);
	/// 调用WindowsAPI强制结束当前线程
	void  ForceEnd(void);
//线程功能
private:
	/// 互斥中断2
	mutex m_mutex;
	bool m_suspended = true;
	condition_variable_any m_condition;
	/// 防止p_Task,p_Para访问冲突
	shared_mutex m_section;
	/// 线程主函数 :线程创建后开始执行,处理任务Task和消息,主函数退出后线程销毁
	static void ThreadMain(void* thisObj);
	/// 虚任务函数,派生类重载此函数,静态ThreadMain函数会调用派生类的Task()
	virtual void  Task(void);
	//线程对象
	thread* p_thread=0;// = thread(&(ThreadEx::ThreadMain), (void*)this);//* p_
};
///////////////////////////////////////////////////