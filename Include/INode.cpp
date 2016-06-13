#include "INode.h"
#include "Link.h"
#include <boost/property_tree/json_parser.hpp>
using namespace std;
using namespace boost;

void INode::Unitial()
{
	try
	{
		if (m_handle != INVALID_HANDLE)
		{
			//Do something here
			//...
			m_handle = INVALID_HANDLE;
		}
		m_status = ND_UNITIALED;
	}
	catch (boost::exception& e)
	{
		return;
	}
	return;
}

bool INode::Set(char* setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		strcpy_s(name, sizeof(name), pRoot.get<string>("name").data());
		strcpy_s(path, sizeof(path), pRoot.get<string>("path").data());
		strcpy_s(ip, sizeof(ip), pRoot.get<string>("ip").data());
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return true;
}

bool INode::Get(char* setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		pRoot.put("name", name);
		pRoot.put("path", path);
		pRoot.put("ip", ip);
		write_json(stream, pRoot);
		//settingStr.clear();
		stream >> settingStr;
		strcpy_s(setting, settingLen, settingStr.data());
	}
	catch (boost::exception& e)
	{
		return false;
	}
	return true;
}

template <typename T>
bool IProducer<T>::Click()
{
	for (int i = linkOutPool.Begin(); i <= linkOutPool.End(); i = linkOutPool.Next(i))
	{
		Link* linkOut = linkOutPool.At(i);
		if (linkOut != NULL_POINTER)
		{
			linkOut->Click();
		}
	}
}

template <typename T>
bool IConsumer<T>::Click()
{
	for (int i = linkInPool.Begin(); i <= linkInPool.End(); i = linkInPool.Next(i))
	{
		Link* linkIn = linkInPool.At(i);
		if (linkIn != NULL_POINTER)
		{
			linkIn->Click();
		}
	}
}

template <typename T1, typename T2 >
bool IProcessor<T1,T2>::Click()
{
	Link* linkOn = p_LinkOn;
	if (linkOn != NULL_POINTER)
	{
		linkOn->Click();
	}
}