/* \file INode_base.hpp
   \brief Interface of all node object.
   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 3.0
   \History:
      Leon 2016/06/20 16:44 pack in *.hpp file.\n
      1.0 : Leon 2016/06/06 12:40 build\n
*/

#pragma once
#ifndef INODE_BASE
#define INODE_BASE

#include "..\Macros.h"
#include "..\Container\PointerPool_templated.hpp"
#include <boost\thread.hpp>
using namespace boost;
#ifndef ND_ERR
#define ND_NORMAL     (int)0
#define ND_ERR        (int)-1000
#define ND_ERR_MEMOUT (int)ND_ERR-1
#define ND_ERR_LOCKED (int)ND_ERR-2
#define ND_ERR_PARA   (int)ND_ERR-3
#endif


/* \interface INode
  \brief Interface of all node object, include Initial and Unitial function.
  \note
  \author Leon Contact: towanglei@163.com
  \version 1.0
  \date 2016/04/29 10:34
*/
class INode
{
private:
	char name[128] = "";///< Node name
	char path[512] = ""; ///< Node path
	char ip[16] = "";///< IP address of host,not support multi-net
public:
	/// Handle of node
	HANDLE m_handle = INVALID_HANDLE;
	/* \enum ND_STATUS
	*  \brief Status of node.
	*  \author Leon Contact: towanglei@163.com
	*/
	enum ND_STATUS
	{
		ND_INVALID = 0, ///< Invalid node e.g. not exist
		ND_UNITIALED = 1, ///< Available node but not initialed
		ND_INITIALED = 2 ///< Initialed node
	};
	/// Status of node
	ND_STATUS m_status = ND_UNITIALED;
	/// Err Code
	int m_ErrCode = ND_NORMAL;
	/// Shared lock to avoid conflicts in multi-thread access
	mutex m_section;

public:
	/// Default constructor
	INode() {};
	/* \fn  ~INode
	  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~INode()
	{
		Unitial();
	}
	/* \fn  Initial
	  \brief Initial node
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool Initial(char* setting, long settingLen)
	{
		if (m_status == ND_INITIALED)
		{
			Unitial();
		}
		bool rtVal = Set(setting, settingLen);
		m_handle = NULL_HANDLE;
		m_status = ND_INITIALED;
		return rtVal;
	}
	/* \fn  Unitial
	  \brief Unitial node
	*/
	virtual void Unitial()
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
		catch (...)
		{
			OutputDebugStringEx("INode::Unitial().", LogLevel::debugLevel);
			return;
		}
		return;
	}
	/* \fn  Set
	  \brief Set node para without stop
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
			strcpy_s(name, sizeof(name), pRoot.get<string>("name").data());
			strcpy_s(path, sizeof(path), pRoot.get<string>("path").data());
			strcpy_s(ip, sizeof(ip), pRoot.get<string>("ip").data());
		}
		catch (...)
		{
			OutputDebugStringEx("INode::Set().", LogLevel::debugLevel);
			return false;
		}
		return true;
	}
	/* \fn  Get
	  \brief Get node para without stop
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
			read_json(stream, pRoot);
			pRoot.put("name", name);
			pRoot.put("path", path);
			pRoot.put("ip", ip);
			write_json(stream, pRoot);
			//settingStr.clear();
			stream >> settingStr;
			strcpy_s(setting, settingLen, settingStr.data());
		}
		catch (...)
		{
			OutputDebugStringEx("INode::Get().", LogLevel::debugLevel);
			return false;
		}
		return true;
	}
	/* \fn  ReSet
	  \brief ReSet node, stop and restart
	  \param[in] char* setting as json string
	  \return bool
	*/
	virtual bool ReSet(char* setting, long settingLen)
	{
		Unitial();
		return Initial(setting, settingLen);
	};
};

/* \interface IProducer
   \implements INode
   \brief Interface of all data producer object, include read function and >> operator.
   \note
   \author Leon Contact: towanglei@163.com
   \version 1.0
   \date 2016/04/29 10:34
 */
template <typename T1>
class IProducer: virtual public INode
{
public:
	PointerPool_templated<8> linkOutPool;
public:
	/// Default constructor
	IProducer() {};
	/* \fn  ~IProducer
	  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~IProducer() {};
public:
	/* \fn  Read
	  \brief Read form buffer of node, or process data and return.
	  \note
	    If update buffer from physical node before read.\n
	    T1& data is deep copied from buffer/physical node.\n
	  \param[out/in] T1& data, T1 must be memory continues
	  \return False if failed.
	*/
	virtual bool Read(T1 & data) = 0;
	/* \fn  Update
	  \brief Update buffer of node.
	  \note
	    If update buffer from physical node before read.\n
	  \return False if failed.
	*/
	virtual bool Update() = 0;
};

/* \interface ConstProducer
  \implements IProducer
  \brief Produce const value of T1.
  \note
  \author Leon Contact: towanglei@163.com
  \version 1.0
  \date 2016/06/16 10:34
*/
template <typename T1>
class ConstProducer : virtual public IProducer<T1>
{
private:
	T1 constData;
public:
	/// Setting constructor
	ConstProducer(T1& data)
	{
		try
		{
			constData = data;
		}
		catch (...)
		{
			OutputDebugStringEx("ConstProducer::ConstProducer().", LogLevel::debugLevel);
		}
	}
public:
	/* \fn  Read
	  \brief Read form buffer of node, or process data and return.
	  \note
	    If update buffer from physical node before read.\n
	    T1& data is deep copied from buffer/physical node.\n
	  \param[out/in] T1& data, T1 must be memory continues
	  \return False if failed.
	*/
	virtual bool Read(T1 & data)
	{
		try
		{
			data = constData;
		}
		catch (...)
		{
			OutputDebugStringEx("ConstProducer::Read().", LogLevel::debugLevel);
			return false;
		}
		return true;
	}
	/* \fn  Update
	  \brief Update buffer of node.
	  \note
	    If update buffer from physical node before read.\n
	  \return False if failed.
	*/
	virtual bool Update(T1& data)
	{
		try
		{
			constData = data;
		}
		catch (...)
		{
			OutputDebugStringEx("ConstProducer::Update().", LogLevel::debugLevel);
			return false;
		}
		return true;
	}
};

/* \interface IConsumer
  \implements INode
  \brief Interface of all data consumer object, include write function and << operator.
  \note
  \author Leon Contact: towanglei@163.com
  \version 1.0
  \date 2016/04/29 10:34
*/
template <typename T2>
class IConsumer : virtual public INode
{
public:
	PointerPool_templated<8> linkInPool;
public:
	/// Default constructor
	IConsumer() {};
	/* \fn  ~IConsumer
	  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~IConsumer() {};
public:
	/* \fn  Write
	  \brief Write data into buffer of node,if force push buffer into physical node after write.
	  \note
	    T2& data is deep copied into buffer/physical node.\n
	  \param[in] T2& data, T2 must has deep copy operator=
	  \return False if failed.
	*/
	virtual bool Write(const T2 & data) = 0;
	/* \fn  Clear
	  \brief Clear buffer of node.
	  \note
	    If update buffer from physical node before read.\n
	  \return False if failed.
	*/
	virtual bool Clear() = 0;
};

/* \interface ConstConsumer
  \implements IConsumer
  \brief Const Consumer do nothing.
  \note
  \author Leon Contact: towanglei@163.com
  \version 1.0
  \date 2016/06/16 10:34
*/
template <typename T2>
class ConstConsumer : virtual public IConsumer<T2>
{
public:
	/* \fn  Write
	  \brief Write data into buffer of node,if force push buffer into physical node after write.
	  \note
	    T2& data is deep copied into buffer/physical node.\n
	  \param[in] T2& data, T2 must has deep copy operator=
	  \return False if failed.
	*/
	virtual bool Write(const T2 & data)
	{
		return true;
	}
	/* \fn  Clear
	  \brief Clear buffer of node.
	  \note
	    If update buffer from physical node before read.\n
	  \return False if failed.
	*/
	virtual bool Clear()
	{
		return true;
	}
};

/* \interface IProcessor
   \implements INode
   \brief Interface of all processor object, include Process function.
   \note
   \author Leon Contact: towanglei@163.com
   \version 1.0
   \date 2016/04/29 10:34
*/
template <typename T1, typename T2 >
class IProcessor:public INode
{
public:
	void* p_LinkOn = NULL_POINTER;
public:
	/// Default constructor
	IProcessor() {};
	/* \fn  ~IProcessor
	  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~IProcessor() {};
	/* \fn  Process
	  \brief Process data and return.
	  \param[in] T1& dataIn, T1 must has deep copy operator=
	  \param[out] T2& dataOut, T2 must has deep copy operator=
	  \return False if failed.
	*/
	virtual bool Process(T1 & dataIn, T2 & dataOut) = 0;
};

#endif

