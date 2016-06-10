#include "DataBuffer.h"
#include <boost/property_tree/json_parser.hpp>
using namespace std;
using namespace boost;

template <typename T1, typename T2>
DataBuffer<T1, T2>::DataBuffer()
{
}

template <typename T1, typename T2>
DataBuffer<T1, T2>::~DataBuffer()
{
}

template<typename T1, typename T2>
void DataBuffer<T1, T2>::Unitial()
{
	if (p_CircularBuffer!=0)
	{
		p_CircularBuffer->clear();
		delete p_CircularBuffer;
		p_CircularBuffer = 0;
	}
	IDevice::Unitial();
}

template<typename T1, typename T2>
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
		if (p_CircularBuffer != 0)
		{
			p_CircularBuffer->clear();
			delete p_CircularBuffer;
			p_CircularBuffer = 0;
		}
		if (resizable)
		{
			p_CircularBuffer = new circular_buffer_space_optimized<T1>(bufferMaxSize);
		} 
		else
		{
			p_CircularBuffer = new circular_buffer<T1>(bufferMaxSize);
		}		
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return INode::Set(setting, settingLen);
}

template<typename T1, typename T2>
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

template<typename T1, typename T2>
bool DataBuffer<T1, T2>::Read(T2 & data, bool update)
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->empty()) return false;
	if (bufferType == BUF_QUEUE)
	{
		data = p_CircularBuffer->front();
		if (update)
		{
			p_CircularBuffer->pop_front();
		}
	} 
	else if(bufferType == BUF_STACK)
	{
		data = p_CircularBuffer->back();
		if (update)
		{
			p_CircularBuffer->pop_back();
		}
	}
	else return false;
	return true;
}

template<typename T1, typename T2>
bool DataBuffer<T1, T2>::Write(const T1 & data, bool force)
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->full())
	{
		if (force)
		{
			if (bufferType == BUF_QUEUE)
			{
				p_CircularBuffer->pop_front();
			} 
			else if(bufferType == BUF_STACK)
			{
				p_CircularBuffer->pop_front();
			}
			else return false;
		}
		else return false;
	}
	p_CircularBuffer->push_back(data);
	return true;
}

template<typename T1, typename T2>
bool DataBuffer<T1, T2>::Update()
{
	if (p_CircularBuffer == 0) return false;
	if (p_CircularBuffer->empty()) return false;
	if (bufferType == BUF_QUEUE)
	{
		if (update)
		{
			p_CircularBuffer->pop_front();
		}
	}
	else if (bufferType == BUF_STACK)
	{
		if (update)
		{
			p_CircularBuffer->pop_back();
		}
	}
	else return false;
	return true;
}
