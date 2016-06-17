#include "DataBuffer.h"
#include <boost/property_tree/json_parser.hpp>
using namespace std;
using namespace boost;

template <typename T1, typename T2 = T1>
DataBuffer<T1, T2>::DataBuffer()
{
	try
	{
		if (typeindex(T1) != typeindex(T2))
		{
			DWORD e;
			throw e;
		}
	}
	catch (DWORD& e)
	{
	}
	catch (boost::exception& e)
	{
	}
}

template <typename T1, typename T2 = T1>
DataBuffer<T1, T2>::~DataBuffer()
{
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Initial(char* setting, long settingLen)
{
	bool rtVal = INode::Initial(setting, settingLen);
	try
	{
		if (p_CircularBuffer != 0)
		{
			p_CircularBuffer->clear();
			delete p_CircularBuffer;
			p_CircularBuffer = 0;
		}
		if (resizable)
		{
			p_CircularBuffer = new circular_buffer_space_optimized<T>(bufferMaxSize);
		}
		else
		{
			p_CircularBuffer = new circular_buffer<T>(bufferMaxSize);
		}
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return rtVal;
}

template <typename T1, typename T2 = T1>
void DataBuffer<T1, T2>::Unitial()
{
	try
	{
		if (p_CircularBuffer!=0)
		{
			p_CircularBuffer->clear();
			delete p_CircularBuffer;
			p_CircularBuffer = 0;
		}
	}
	catch (boost::exception& e)
	{
		return;
	}
	INode::Unitial();
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Set(char * setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		bufferMaxSize = MAX(pRoot.get<int>("bufferMaxSize"),2);
		bufferType = pRoot.get<int>("bufferType");
		resizable = pRoot.get<bool>("resizable");
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return INode::Set(setting, settingLen);
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Get(char * setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		pRoot.put("bufferMaxSize", bufferMaxSize);
		pRoot.put("bufferType", bufferType);
		pRoot.put("resizable", resizable);
		write_json(stream, pRoot);
		//settingStr.clear();
		stream >> settingStr;
		strcpy_s(setting, settingLen, settingStr.data());
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return INode::Get(setting, settingLen);
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Read(T1 & data)
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->empty()) return false;
	if (bufferType == BUF_QUEUE)
	{
		data = p_CircularBuffer->front();
	} 
	else if(bufferType == BUF_STACK)
	{
		data = p_CircularBuffer->back();
	}
	else return false;
	return true;
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Update()
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->empty()) return false;
	if (bufferType == BUF_QUEUE)
	{
		p_CircularBuffer->pop_front();
	}
	else if (bufferType == BUF_STACK)
	{
		p_CircularBuffer->pop_back();
	}
	else return false;
	return true;
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Write(const T2 & data)
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->full()) return false;
	p_CircularBuffer->push_back(data);
	return true;
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1, T2>::Clear()
{
	if (p_CircularBuffer == 0) return false;
	if (!p_CircularBuffer->full()) return true;
	if (bufferType == BUF_QUEUE)
	{
		p_CircularBuffer->pop_front();
	}
	else if (bufferType == BUF_STACK)
	{
		p_CircularBuffer->pop_back();
	}
	else return false;
	return true;
}

template <typename T1, typename T2 = T1>
bool DataBuffer<T1,T2>::Click()
{
	return false;
}