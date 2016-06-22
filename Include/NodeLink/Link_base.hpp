/* \file Link_base.hpp
   \brief Connect INode and drive data flow from one to another.
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 2.0
   \History:
      Leon 2016/06/06 12:40 pack in *.hpp file.\n
      1.0 : Leon 2016/06/06 12:40 build\n
 */

#pragma once
#ifndef INODE_BASE
#define INODE_BASE

#include "..\BThread\BThread_base.hpp"
#include "INode_base.h"
#ifndef LK_ERR
#define LK_NORMAL      (int)0
#define LK_ERR         (int)-1000
#define LK_ERR_MEMOUT  (int)LK_ERR-1
#define LK_ERR_LOCKED  (int)LK_ERR-2
#define LK_ERR_READ    (int)LK_ERR-3
#define LK_ERR_WRITE   (int)LK_ERR-4
#define LK_ERR_PROCESS (int)LK_ERR-4
#define LK_ERR_PARA    (int)LK_ERR-6
#endif

/* \class Link : public Thread, include *IProducer *IProcessor
  \brief Drive data from IProducer to IProducer.
  \note
  \author Leon Contact: towanglei@163.com
  \version 1.0
  \date 2016/04/29 23:09
*/
template <typename T1, typename T2>
class Link : public BThread
{
//Thread tags
public:
	/// Class ID to distinct base and extend class.
	const int m_classID = 1;
private:
	char name[128] = "";///< Link name
	long& frameTimes = m_times; ///< Thread run times
	long& frameTime = m_waitTime; ///< Thread wait time
	bool& includeTaskTime = m_includeTaskTime; ///< Thread wait time

	enum { ReadNoUpdate = 0, ReadThenUpdate = 1, UpdateThenRead = -1 };
	int readAndUpdate = ReadThenUpdate; ///< If case:0 Read without Update; case:1 Read then Update; case:-1 Update then Read.
	bool readBlocked = false; ///< Is data blocked in multi-threads reading
	enum { WriteNoClear = 0, WriteThenClear = 1, ClearThenWrite = -1 };
	int writeAndClear= ClearThenWrite; ///< If case:0 Write without Clear; case:1 Write then Clear; case:-1 Clear then Write.
	bool writeBlocked = false; ///< Is data blocked in multi-threads writing
private:
	bool triggerInside = false; ///< Is object pointer triggered inside or outside
	IProducer<T1>* p_Producer = 0; ///< Produce object pointer
	IConsumer<T2>* p_Consumer = 0; ///< Consume object pointer
	IProcessor<T1,T2>* p_Processor = 0; ///< Process object pointer

public:
	/// Handle of node
	HANDLE& m_handle = m_hThread;
	/* \enum LK_STATUS
	  \brief Status of link.
	  \author Leon Contact: towanglei@163.com
	*/
	enum LK_STATUS
	{
		LK_INVALID = 0, ///< Invalid node e.g. not exist
		LK_UNITIALED = 1, ///< Available node but not initialed 
		LK_INITIALED = 2 ///< Initialed node
	};
	/// Status of node
	LK_STATUS  m_status = LK_UNITIALED;
	/// Err Code
	int m_ErrCode = LK_NORMAL;

public:
	/// Set expected frame time(ms)
	void SetFrameTime(long frameTime);

public:
	/// Default constructor
	Link();
	/// Default destructor
	virtual ~Link();
	/* \fn  Initial
	  \brief Initial Link
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Initial(char* setting, long settingLen);
	/* \fn  Unitial
	  \brief Unitial Link
	*/
	virtual void Unitial();
	/* \fn  Set
	  \brief Set running link
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Set(char* setting, long settingLen);
	/* \fn  Get
	  \brief Get link para
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Get(char* setting, long settingLen);
	/* \fn  ReSet
	  \brief Stop and reset link,need StartLink()
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool ReSet(char* setting, long settingLen);

public:
	/// Task function for once run
	virtual void  Task(void);
	/// Task function for once run
	virtual void  Task(T1& pDataIn);

public:
	/* \fn  AttachProducer
	  \brief Add producer to link
	  \param[in] IProducer<T1>* pProducer = (IProducer<T1>*)&producerObj;
	  \return bool
	*/
	virtual bool AttachProducer(IProducer<T1>* pProducer);
	/* \fn  AttachConsumer
	  \brief Add consumer to link
	  \param[in] IConsumer<T2>* pConsumer = (IConsumer<T2>*)&consumerObj;
	  \return bool
	*/
	virtual bool AttachConsumer(IConsumer<T2>* pConsumer);
	/* \fn  AttachProcessor
	  \brief Add device to link
	  \param[in] IProcessor<T1, T2>* pProcessor = (IProcessor<T1, T2>*)&processorObj;
	  \return bool
	*/
	virtual bool AttachProcessor(IProcessor<T1, T2>* pProcessor);
	/* \fn  DetachProducer
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachProducer();
	/* \fn  DetachConsumer
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachConsumer();
	/* \fn  DetachProcessor
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachProcessor();
	/* \fn  StartLink
	  \brief Start link to run the added device
	  \return bool
	*/
	virtual bool StartLink();
	/* \fn  StopLink
	  \brief Stop link without delete anything
	  \return bool
	*/
	virtual bool StopLink();

	/* \fn  TriggerInside
	  \brief Trigger-inside mode means Started Link.
	  \return bool
	*/
	virtual bool IsTriggerInside();
	/* \fn  Click
	  \brief Click link to run once, only work in Stopped Link(Trigger-outside mode).
	  \return bool
	*/
	virtual bool Click(bool isSync);
};

#endif
