#include "DatabaseNode.h"
#include "..\Link.h"
#include <boost\property_tree\json_parser.hpp>
//#include <boost\lexical_cast.hpp>
//#include <boost\format.hpp>
using namespace std;
using namespace boost;

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
DatabaseNode<T1, T2>::DatabaseNode()
{
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
DatabaseNode<T1, T2>::~DatabaseNode()
{
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
bool DatabaseNode<T1, T2>::Set(char * setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		strcpy_s(dbProvider, sizeof(dbProvider), pRoot.get<string>("dbProvider").data());
		strcpy_s(dbServerPath, sizeof(dbServerPath), pRoot.get<string>("dbServerPath").data());
		strcpy_s(dbName, sizeof(dbName), pRoot.get<string>("dbName").data());
		strcpy_s(dbUser, sizeof(dbUser), pRoot.get<string>("dbUser").data());
		strcpy_s(dbPassword, sizeof(dbPassword), pRoot.get<string>("dbPassword").data());
	}
	catch (...)
	{
		OutputDebugStringEx("DatabaseNode::Set().", LogLevel::debugLevel);
		return false;
	}
	return INode::Set(setting, settingLen);
}

template <typename T1 = DBIOFormat, typename T2 = DBIOFormat >
bool DatabaseNode<T1, T2>::Get(char * setting, long settingLen)
{
	try
	{
		string settingStr = setting;
		stringstream stream;
		stream << settingStr;
		property_tree::ptree pRoot;
		read_json(stream, pRoot);
		pRoot.put("dbProvider", dbProvider);
		pRoot.put("dbServerPath", dbServerPath);
		pRoot.put("dbName", dbName);
		pRoot.put("dbUser", dbUser);
		pRoot.put("dbPassword", dbPassword);
		write_json(stream, pRoot);
		//settingStr.clear();
		stream >> settingStr;
		strcpy_s(setting, settingLen, settingStr.data());
	}
	catch (...)
	{
		OutputDebugStringEx("DatabaseNode::Get().", LogLevel::debugLevel);
		return false;
	}
	return INode::Get(setting, settingLen);
}

//template <typename DBIOFormat, typename DBIOFormat>
//bool DatabaseNode<DBIOFormat, DBIOFormat>::Click(bool isSync)
//{
//	Link<DBIOFormat, DBIOFormat>* linkOn = (Link<DBIOFormat, DBIOFormat>*)p_LinkOn;
//	if (linkOn != NULL_POINTER)
//	{
//		linkOn->Click(isSync);
//	}
//	return true;
//}