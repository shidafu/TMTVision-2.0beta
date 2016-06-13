///////////////////////////////////////////////////
/** \file BThread.h
 *  \brief Thread manager using Boost library
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 4.1
 *  \History:
 *     2016/06/12 00:36 Add Click() for to one time in suspended status.\n
 *     4.0 : 2016/06/10 12:03 Use C++ 11 boost library.\n
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
#include <boost\thread.hpp>
//#include <thread>             // std::thread, std::this_thread::yield
//#include <mutex>              // std::mutex
//#include <condition_variable> // std::condition_variable_any
using namespace boost;

///////////////////////////////////////////////////
/** \class BThread : 
 *  \brief Thread manager using Boost library
 *  \note
 *  1:Create thread to run ThreadMain, destroy thread after ThreadMain return;\n
 *  2:The core function ThreadExMain has several interruption point, to implement:\n
 *    Suspend() vs Resume(),\n
 *    Destroy() or ForceEnd(),\n
 *    Create(long times = -1, long waitTime = 0, bool includeTaskTime = false),\n
 *           times: run times count, -1 means infinite,
 *           waitTime: sleep times(ms) between task,
 *           includeTaskTime: if task time included in wait time count.\n
 *  3:ThreadExMain run Task() function several times until encounter exit/suspend/sleep interruption,\n
 *           the Task() function is virtual and implimented by externded class.\n
 *
 *  \author Leon Contact: towanglei@163.com
 *  \version 3.0
 *  \date 2016/04/29 22:34
 */
class BThread
{
//类功能
public:
	/// BThread ID，+1 after construct.
	static DWORD m_nThreadID;//2.0
	/// Class ID to distinct base and extend class.
	const int m_classID = 0;//3.0
	HANDLE  m_hThread = INVALID_HANDLE;///< BThread handle
	HANDLE  m_hParent = INVALID_HANDLE;;///< Parent handle

//Thread paras:
protected:
	bool m_bExit;///< Exit flag.
	long m_times;///< Run times,-1 means infinite.
	long m_timesBack;///< Run times,-1 means infinite.
	long m_waitTime;///< Expected time(ms) from task end/start to next task start.
	double m_frameTime;///< Actual frame time(ms) 4.0.
	double m_taskTime;///< Actual task time(ms) from task start to end 3.1.
	bool m_includeTaskTime;///< If m_waitTime include task time, false/true is end/start to start.
	
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
	/** \enum THSTATUS
	*  \brief BThread status
	*
	*  \note
	** Status:
	*
	*             BThread()━▶ ┏━━━━━━━┓ ◀━~BThread()
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
		TH_EXIT = 0, ///< Not initialed BThread.
		TH_AVIALABLE =1, ///< Initialed BThread but not run.
		TH_RUNNING = 2, ///< Running BThread.
		TH_SUSPEND = 3 ///< Suspend BThread.
	};
	/// BThread status.
	THSTATUS m_ThStatus;

//Thread methods:
public:
	/// Create source when construct.
	BThread(HANDLE hParent = INVALID_HANDLE);
	/// Destruct use ForceEnd().
	~BThread(void); 
	/// Create thread.
	bool  Create(long times = -1, long waitTime = 0, bool includeTaskTime = false);
	/// Resume suspended thread.
	bool  Resume(void);
	/// Suspend thread
	bool  Suspend(void);
	/// Stop thread and destroy source, invalid when suspended,
	bool  Destroy(void);
	/// Force Stop thread and destroy source, valid when suspended.
	bool  ForceEnd(void);
	/// Click to run one time.
	bool  Click(bool wait=false);

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
	static void ThreadMain(void* thisObj);
	/// Thread Task, implement this function in extend class.
	virtual void  Task(void);
	/// Thread object.
	thread* p_thread=0;
};
///////////////////////////////////////////////////