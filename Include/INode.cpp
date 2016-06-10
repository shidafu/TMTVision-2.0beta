#include "INode.h"
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
		//strcpy_s(deviceMAC, sizeof(deviceMAC), pRoot.get<string>("deviceMAC").data());
		strcpy_s(ip, sizeof(ip), pRoot.get<string>("ip").data());
		readUpdate = pRoot.get<bool>("readUpdate");
		writeForce = pRoot.get<bool>("writeForce");
		accessBlocked = pRoot.get<bool>("accessBlocked");
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
		pRoot.put("readUpdate", readUpdate);
		pRoot.put("writeForce", writeForce);
		pRoot.put("accessBlocked", accessBlocked);
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