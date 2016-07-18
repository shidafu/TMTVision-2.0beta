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
#ifndef LINK_BASE
#define LINK_BASE

#include "..\BThread\BThread_base.hpp"
#include "INode_base.hpp"
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
	void SetFrameTime(long frameTime)
	{
		m_waitTime = frameTime;
	}

public:
	/// Default constructor
	Link() {};
	/// Default destructor
	virtual ~Link() {};
	/* \fn  Initial
	  \brief Initial Link
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Initial(char* setting, long settingLen) 
	{
		if (m_status == LK_INITIALED)
		{
			Unitial();
		}
		bool rtVal = Set(setting, settingLen);
		if (m_section.try_lock())
		{
			rtVal &= Create(frameTimes, frameTime, includeTaskTime);
			m_section.unlock();
		}
		else rtVal = false;
		if (rtVal)
		{
			m_status = LK_INITIALED;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return rtVal;
	}

	/* \fn  Unitial
	  \brief Unitial Link
	*/
	virtual void Unitial()
	{
		try
		{
			if (m_handle != INVALID_HANDLE)
			{
				//Do something here
				//...
				StopLink();
				DetachProducer();
				DetachConsumer();
				DetachProcessor();
				ForceEnd();
				m_handle = INVALID_HANDLE;
			}
			m_status = LK_UNITIALED;
		}
		catch (...)
		{
			WRITE_LOG(severity_level::error, "");
			return;
		}
		return;
	}
	/* \fn  Set
	  \brief Set running link
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Set(char* setting, long settingLen)
	{
		try
		{
			string settingStr = setting;
			stringstream stream;
			stream << settingStr;
			property_tree::ptree pRoot;
			read_json(stream, pRoot);
			if (m_section.try_lock())
			{
				strcpy_s(name, sizeof(name), pRoot.get<string>("name").data());
				frameTimes = pRoot.get<long>("frameTimes");
				frameTime = pRoot.get<long>("frameTime");
				includeTaskTime = pRoot.get<bool>("includeTaskTime");
				readAndUpdate = pRoot.get<int>("readAndUpdate");
				readBlocked = pRoot.get<bool>("readBlocked");
				writeAndClear = pRoot.get<int>("writeAndClear");
				writeBlocked = pRoot.get<bool>("writeBlocked");
				m_section.unlock();
			}
			else
			{
				WRITE_LOG(severity_level::warning, "");
			}
		}
		catch (...)
		{
			WRITE_LOG(severity_level::error, "");
			return false;
		}
		return true;
	}
	/* \fn  Get
	  \brief Get link para
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Get(char* setting, long settingLen) 
	{
		try
		{
			string settingStr = setting;
			stringstream stream;
			stream << settingStr;
			property_tree::ptree pRoot;
			//read_json(stream, pRoot);
			if (m_section.try_lock())
			{
				pRoot.put("name", name);
				pRoot.put("frameTimes", frameTimes);
				pRoot.put("frameTime", frameTime);
				pRoot.put("includeTaskTime", includeTaskTime);
				pRoot.put("readAndUpdate", readAndUpdate);
				pRoot.put("readBlocked", readBlocked);
				pRoot.put("writeAndClear", writeAndClear);
				pRoot.put("writeBlocked", writeBlocked);
				write_json(stream, pRoot);
				//settingStr.clear();stream >> 
				settingStr= stream.str();
				strcpy_s(setting, settingLen, settingStr.data());
				m_section.unlock();
			}
			else
			{
				WRITE_LOG(severity_level::warning, "");
			}
		}
		catch (...)
		{
			WRITE_LOG(severity_level::error, "");
			return false;
		}
		return true;
	}
	/* \fn  ReSet
	  \brief Stop and reset link,need StartLink()
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool ReSet(char* setting, long settingLen)
	{
		Unitial();
		return Initial(setting, settingLen);
	};

public:
	/// Task function for once run
	virtual void  Task(void) 
	{
		// Check status validate
		if (m_handle == INVALID_HANDLE || m_status != LK_INITIALED) return;//Server not initialed
		if (/*p_Producer == 0 || */p_Consumer == 0) return;//Device not Attached
		if (p_Producer != 0)
		{
			if (p_Producer->m_status != INode::ND_INITIALED) return;//Device error or not initialed
		}
		if (p_Consumer->m_status != INode::ND_INITIALED) return;//Device error or not initialed

		// Temp data
		T1 tmpT1In;
		T1 tmpT1Out;
		T2 tmpT2Out;
		T2* tmpT2OutRef = &tmpT2Out;
		bool readedOrProcessed = false;

		if (p_Producer != 0)
		{
			// Read data
			bool readOK = true;
			p_Producer->m_section.lock();
			switch (readAndUpdate)
			{
			case ReadNoUpdate:
				readOK &= p_Producer->Read(tmpT1In);
				readedOrProcessed = true;
				break;
			case ReadThenUpdate:
				readOK &= p_Producer->Read(tmpT1In);
				readOK &= p_Producer->Update();
				readedOrProcessed = true;
				break;
			case UpdateThenRead:
				readOK &= p_Producer->Update();
				readOK &= p_Producer->Read(tmpT1In);
				readedOrProcessed = true;
				break;
			default:
				m_ErrCode = LK_ERR_PARA;
				p_Producer->m_section.unlock();
				return;
			}
			p_Producer->m_section.unlock();
			if (!readOK)
			{
				m_ErrCode = LK_ERR_READ;
				WRITE_LOG(severity_level::error, "");
				return;
			}
		}

		// Process data
		if (p_Processor == 0)
		{
			try
			{
				tmpT2OutRef = &tmpT1Out;
			}
			catch (boost::exception& e)
			{
				m_ErrCode = LK_ERR_PARA;
				WRITE_LOG(severity_level::error, "");
				return;
			}
		}
		else
		{
			bool processOK = true;
			processOK &= p_Processor->Process(tmpT1In, tmpT2Out);
			readedOrProcessed = processOK;
		}

		if (!readedOrProcessed)
		{
			m_ErrCode = LK_ERR_PROCESS;
			WRITE_LOG(severity_level::error, "");
			return;
		}

		// Write data
		bool writeOK = true;
		p_Consumer->m_section.lock();
		switch (writeAndClear)
		{
		case WriteNoClear:
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			break;
		case WriteThenClear:
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			writeOK &= p_Consumer->Clear();
			break;
		case ClearThenWrite:
			writeOK &= p_Consumer->Clear();
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			break;
		default:
			m_ErrCode = LK_ERR_PARA;
			WRITE_LOG(severity_level::error, "");
			p_Consumer->m_section.unlock();
			return;
		}
		p_Consumer->m_section.unlock();
		if (!writeOK)
		{
			m_ErrCode = LK_ERR_READ;
			WRITE_LOG(severity_level::error, "");
			return;
		}
		//double frameTime = GetFrameTime();
		//printf("FrameTime = %f ms\n", frameTime);
	}

	/// Task function for once run
	virtual void  Task(T1& pDataIn) 
	{
		// Check status validate
		if (m_handle == INVALID_HANDLE || m_status != LK_INITIALED) return;//Server not initialed
		if (/*p_Producer == 0 || */p_Consumer == 0) return;//Device not Attached
		if (p_Consumer->m_status != INode::ND_INITIALED) return;//Device error or not initialed

		// Temp data
		T2 tmpT2Out;
		T2* tmpT2OutRef = &tmpT2Out;

		// Process data
		bool processOK = true;
		if (p_Processor == 0)
		{
			try
			{
				tmpT2OutRef = &pDataIn;
			}
			catch (boost::exception& e)
			{
				m_ErrCode = LK_ERR_PARA;
				WRITE_LOG(severity_level::error, "");
				return;
			}
		}
		else
		{
			processOK &= p_Processor->Process(pDataIn, tmpT2Out);
		}

		if (!processOK)
		{
			m_ErrCode = LK_ERR_PROCESS;
			WRITE_LOG(severity_level::error, "");
			return;
		}

		// Write data
		bool writeOK = true;
		p_Consumer->m_section.lock();
		switch (writeAndClear)
		{
		case WriteNoClear:
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			break;
		case WriteThenClear:
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			writeOK &= p_Consumer->Clear();
			break;
		case ClearThenWrite:
			writeOK &= p_Consumer->Clear();
			writeOK &= p_Consumer->Write(*tmpT2OutRef);
			break;
		default:
			m_ErrCode = LK_ERR_PARA;
			WRITE_LOG(severity_level::error, "");
			p_Consumer->m_section.unlock();
			return;
		}
		p_Consumer->m_section.unlock();
		if (!writeOK)
		{
			m_ErrCode = LK_ERR_READ;
			WRITE_LOG(severity_level::error, "");
			return;
		}
		//double frameTime = GetFrameTime();
		//printf("FrameTime = %f ms\n", frameTime);
	}

public:
	/* \fn  AttachProducer
	  \brief Add producer to link
	  \param[in] IProducer<T1>* pProducer = (IProducer<T1>*)&producerObj;
	  \return bool
	*/
	virtual bool AttachProducer(IProducer<T1>* pProducer) 
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Producer = pProducer;
				//p_Producer->linkOutPool.Add((void*)this);
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}
	/* \fn  AttachConsumer
	  \brief Add consumer to link
	  \param[in] IConsumer<T2>* pConsumer = (IConsumer<T2>*)&consumerObj;
	  \return bool
	*/
	virtual bool AttachConsumer(IConsumer<T2>* pConsumer)
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Consumer = pConsumer;
				//p_Producer->linkInPool.Add((void*)this);
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}

	/* \fn  AttachProcessor
	  \brief Add device to link
	  \param[in] IProcessor<T1, T2>* pProcessor = (IProcessor<T1, T2>*)&processorObj;
	  \return bool
	*/
	virtual bool AttachProcessor(IProcessor<T1, T2>* pProcessor)
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Processor = pProcessor;
				//p_Producer->p_LinkOn = (void*)this;
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}
	/* \fn  DetachProducer
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachProducer()
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Producer = 0;
				//p_Producer->linkOutPool.Del((void*)this);
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}
	/* \fn  DetachConsumer
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachConsumer()
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Consumer = 0;
				//p_Producer->linkInPool.Del((void*)this);
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}
	/* \fn  DetachProcessor
	  \brief Delete device from link,
	         but device object is not delete in function
	  \return void
	*/
	virtual bool DetachProcessor()
	{
		if (StopLink())
		{
			bool rtVal = true;
			if (m_section.try_lock())
			{
				p_Processor = 0;
				//p_Producer->p_LinkOn = NULL_POINTER;
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
			return rtVal;
		}
		else
		{
			WRITE_LOG(severity_level::error, "");
		}
		return false;
	}

	/* \fn  StartLink
	  \brief Start link to run the added device
	  \return bool
	*/
	virtual bool StartLink()
	{
		bool rtVal = true;
		if (m_section.try_lock())
		{
			rtVal = Resume();
			rtVal = Resume();
			m_section.unlock();
		}
		else
		{
			rtVal = false;
			WRITE_LOG(severity_level::warning, "");
		}
		if (rtVal) triggerInside = true;
		return rtVal;
	}
	/* \fn  StopLink
	  \brief Stop link without delete anything
	  \return bool
	*/
	virtual bool StopLink()
	{
		bool rtVal = true;
		if (m_section.try_lock())
		{
			if (p_thread!=0)
			{
				rtVal = Suspend();
			}
			m_section.unlock();
		}
		else
		{
			rtVal = false;
			WRITE_LOG(severity_level::warning, "");
		}
		if (rtVal) triggerInside = false;
		return rtVal;
	}

	/* \fn  TriggerInside
	  \brief Trigger-inside mode means Started Link.
	  \return bool
	*/
	virtual bool IsTriggerInside()
	{
		return triggerInside;
	}
	/* \fn  Click
	  \brief Click link to run once, only work in Stopped Link(Trigger-outside mode).
	  \return bool
	*/
	virtual bool Click(bool isSync) 
	{
		if (triggerInside) return false;
		bool rtVal = true;
		if (!isSync)
		{
			if (m_section.try_lock())
			{
				rtVal = BThread::Click();
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
		}
		else
		{
			if (m_section.try_lock())
			{
				Task();
				rtVal = true;
				m_section.unlock();
			}
			else
			{
				rtVal = false;
				WRITE_LOG(severity_level::warning, "");
			}
		}
		return rtVal;
	}

};

#endif

