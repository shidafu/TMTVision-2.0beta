///////////////////////////////////////////////////
/** \file INode.h
 *  \brief Interface of all node object.
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/04 16:44 build\n
 */
///////////////////////////////////////////////////

#pragma once
#include "Macros.h"

///////////////////////////////////////////////////
/** \interface INode
*  \brief Interface of all node object, include Initial and Unitial function.
*  \note
*   Inlined methods for better performents;\n
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/04/29 10:34
*/
interface INode
{
private:
	char name[128] = "";///< Device name
	char path[512] = ""; ///< Device path
	char ip[16] = "";///< IP address of host,not support multi-net
	bool readUpdate = true; ///< If Read(...,bool update) in task
	bool writeForce = false; ///< If Write(...,bool force) in task
	bool accessBlocked = false; ///< Is buffer blocked in multi-threads
private:
	/// Handle of device
	HANDLE m_handle = INVALID_HANDLE;
	/** \enum Status
	*  \brief Status of node.
	*  \author Leon Contact: towanglei@163.com
	*/
	enum ND_STATUS
	{
		ND_INVALID = 0, ///< Invalid device e.g. not exist
		ND_UNITIALED = 1, ///< Available device but not initialed 
		ND_INITIALED = 2 ///< Initialed device
	};
	/// Status of node
	ND_STATUS m_status = ND_INVALID;
public:
	/// Default constructor
	INode() {}
	/** \fn  ~INode
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~INode() { Unitial(); }
	/** \fn  Initial
	*  \brief Initial node
	*  \param[in] char* setting as json string
	*  \return bool
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
	/** \fn  Unitial
	*  \brief Unitial node
	*/
	virtual void Unitial();
	/** \fn  Set
	*  \brief Set node para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Set(char* setting, long settingLen);
	/** \fn  Get
	*  \brief Get node para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Get(char* setting, long settingLen);
	/** \fn  ReSet
	*  \brief ReSet node, stop and restart
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool ReSet(char* setting, long settingLen)
	{
		Unitial();
		return Initial(setting, settingLen);
	};
};
///////////////////////////////////////////////////

///////////////////////////////////////////////////
/** \interface IDevice
 *  \implements INode
 *  \brief Interface of all device object, include read and write function.
 *  \note
 *   Inlined methods for better performents;\n
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/04/29 10:34
 */
template <typename T1, typename T2>
interface IDevice: implements INode
{
public:
	/// Default constructor
	IDevice() {}
	/** \fn  ~IDevice
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~IDevice() {}
	/** \fn  Read
	*  \brief Read form buffer of device, or process data and return.
	*  \note
	*    If update buffer from physical device before read.\n
	*    T2& data is deep copied from buffer/physical device.\n
	*  \param[out/in] T2& data, T2 must be memory continues
	*  \param[in] update =true will update device buffer after read
	*  \return False if failed.
	*/
	virtual bool Read(T2 & data, bool update = false) = 0;
	/** \fn  operator>>
	*  \brief Read form buffer of device, or process data and return.
	*  \note
	*    If update buffer from physical device before read.\n
	*    T2& data is deep copied from buffer/physical device.\n
	*  \param[out/in] T2& data, T2 must has deep copy operator=
	*  \return False if failed.
	*/
	virtual bool operator >> (T2 & data) = 0
	{
		return Read(data,readUpdate);
	};
	/** \fn  Write
	*  \brief Write data into buffer of device,if force push buffer into physical device after write.
	*  \note
	*    T1& data is deep copied into buffer/physical device.\n
	*  \param[in] T1& data, T1 must has deep copy operator=
	*  \param[in] force = true will force data to physical device
	*  \return False if failed.
	*/
	virtual bool Write(const T1 & data, bool force = false) = 0;
	/** \fn  operator<<
	*  \brief Write data into buffer of device,if force push buffer into physical device after write.
	*  \note
	*    T1& data is deep copied into buffer/physical device.\n
	*  \param[in] T1& data, T1 must has deep copy operator=
	*  \return False if failed.
	*/
	virtual bool operator<<(const T1 & data)
	{
		return Write(data,writeForce);
	};
	/** \fn  Update
	*  \brief Update buffer of device.
	*  \note
	*    If update buffer from physical device before read.\n
	*  \return False if failed.
	*/
	virtual bool Update() = 0;
	/** \fn  operator++
	*  \brief Update buffer of device.
	*  \note
	*    If update buffer from physical device before read.\n
	*  \return False if failed.
	*/
	virtual bool operator++()
	{
		return Update();
	};
};
///////////////////////////////////////////////////

///////////////////////////////////////////////////
/** \interface IProcessor
 *  \implements INode
 *  \brief Interface of all processor object, include Process function.
 *  \note
 *   Inlined methods for better performents;\n
 *  \author Leon Contact: towanglei@163.com
 *  \version 1.0
 *  \date 2016/04/29 10:34
 */
template <typename T1, typename T2 >
interface IProcessor:implements INode
{
public:
	/// Default constructor
	IProcessor() {}
	/** \fn  ~IProcessor
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~IProcessor() {}
	/** \fn  Process
	*  \brief Process data and return.
	*  \param[in] T1& dataIn, T1 must has deep copy operator=
	*  \param[out] T2& dataOut, T2 must has deep copy operator=
	*  \return False if failed.
	*/
	virtual bool Process(T1 & dataIn, T2 & dataOut) = 0;
	/** \fn  operator()
	*  \brief Process data and return.
	*  \param[in] T1& dataIn, T1 must has deep copy operator=
	*  \param[out] T2& dataOut, T2 must has deep copy operator=
	*  \return False if failed.
	*/
	virtual bool operator() (T1 & dataIn, T2 & dataOut)
	{
		return Process(dataIn, dataOut);
	};
};
///////////////////////////////////////////////////

