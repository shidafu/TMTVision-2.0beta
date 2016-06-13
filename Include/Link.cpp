#include "Link.h"
using namespace boost;

template <typename T1, typename T2>
Link<T1, T2>::Link()
{
}


template <typename T1, typename T2>
Link<T1, T2>::~Link()
{
}

template <typename T1, typename T2>
bool Link<T1, T2>::Initial(char* setting, long settingLen)
{
	if (m_status == ND_INITIALED)
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
	return rtVal;
}

template <typename T1, typename T2>
void Link<T1, T2>::Unitial()
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
	catch (boost::exception& e)
	{
		return;
	}
	return;
}

template <typename T1, typename T2>
bool Link<T1, T2>::Set(char* setting, long settingLen)
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
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return true;
}

template <typename T1, typename T2>
bool Link<T1, T2>::Get(char* setting, long settingLen)
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
			pRoot.put("name", name);
			pRoot.put("frameTimes", frameTimes);
			pRoot.put("frameTime", frameTime);
			pRoot.put("includeTaskTime", includeTaskTime);
			pRoot.put("readAndUpdate", readAndUpdate);
			pRoot.put("readBlocked", readBlocked);
			pRoot.put("writeAndClear", writeAndClear);
			pRoot.put("writeBlocked", writeBlocked);
			write_json(stream, pRoot);
			//settingStr.clear();
			stream >> settingStr;
			strcpy_s(setting, settingLen, settingStr.data());
			m_section.unlock();
		}
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return true;
}

template <typename T1, typename T2>
void  Link<T1, T2>::Task(void)
{
	// Check status validate
	if (m_handle == INVALID_HANDLE || m_status != LK_INITIALED) return false;//Server not initialed
	if (p_Producer == 0 || p_Consumer == 0) return false;//Device not Attached
	if (p_Producer->m_status != INode::ND_INITIALED) return false;//Device error or not initialed
	if (p_Consumer->m_status != INode::ND_INITIALED) return false;//Device error or not initialed

	// Temp data
	T1 tmpT1In;
	T1 tmpT1Out;
	T2 tmpT2Out;
	T2& tmpT2OutRef = tmpT2Out;

	// Read data
	bool readOK = true;
	p_Producer->m_section.lock();
	switch (readAndUpdate)
	{
	case ReadNoUpdate:
		readOK& = p_Producer->Read(tmpT1In);
		break;
	case ReadThenUpdate:
		readOK& = p_Producer->Read(tmpT1In);
		readOK& = p_Producer->Update();
		break;
	case UpdateThenRead:
		readOK& = p_Producer->Update();
		readOK& = p_Producer->Read(tmpT1In);
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
		return;
	}

	// Process data
	if (p_Processor == 0)
	{
		try
		{
			tmpT2OutRef = tmpT1Out;
		}
		catch (boost::exception& e)
		{
			m_ErrCode = LK_ERR_PARA;
			return;
		}
	}
	else
	{
		bool processOK = true;
		readOK& = p_Processor->Process(tmpT1In,tmpT2Out);
	}
	if (!readOK)
	{
		m_ErrCode = LK_ERR_PROCESS;
		return;
	}

	// Write data
	bool writeOK = true;
	p_Consumer->m_section.lock();
	switch (writeAndClear)
	{
	case WriteNoClear:
		writeOK& = p_Consumer->Write(tmpT2OutRef);
		break;
	case WriteThenClear:
		writeOK& = p_Consumer->Write(tmpT2OutRef);
		writeOK& = p_Consumer->Clear();
		break;
	case ClearThenWrite:
		writeOK& = p_Consumer->Clear();
		writeOK& = p_Consumer->Write(tmpT2OutRef);
		break;
	default:
		m_ErrCode = LK_ERR_PARA;
		p_Consumer->m_section.unlock();
		return;
	}
	p_Consumer->m_section.unlock();
	if (!writeOK)
	{
		m_ErrCode = LK_ERR_READ;
		return;
	}
	//double frameTime = GetFrameTime();
	//printf("FrameTime = %f ms\n", frameTime);
}