///////////////////////////////////////////////////
/** \file DataBuffer.h
 *  \brief Data buffer object, read and write circular_buffer.
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/04 22:12 build\n
 */
///////////////////////////////////////////////////

#pragma once
#include "INode.h"
#include <boost/circular_buffer.hpp>
using namespace boost;

///////////////////////////////////////////////////
/** \class DataBuffer  \implements IDevice
*  \brief Data buffer object, read and write circular_buffer.
*  \note
*   Inlined methods for better performents;\n
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/04/29 10:34
*/
template <typename T1, typename T2 = T1>
class DataBuffer : implements IDevice<T1,T2>
{
private:
	int bufferMaxSize = 8; ///< Data list size
	enum { BUF_QUEUE = 0, BUF_STACK = 1 };
	int bufferType = BUF_QUEUE; ///< Is queue or stack
	bool resizable = false; ///< Is buffer resizable
	circular_buffer<T1>* p_CircularBuffer = 0;
public:
	/// Default constructor
	DataBuffer();
	/** \fn  ~DataBuffer
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~DataBuffer();
	///** \fn  Initial
	//*  \brief Initial DataBuffer
	//*  \param[in] char* setting as json string
	//*  \return bool
	//*/
	//virtual bool Initial(char* setting, long settingLen)
	//{
	//	if (m_status == ND_INITIALED)
	//	{
	//		Unitial();
	//	}
	//	bool rtVal = Set(setting, settingLen);
	//	m_handle = NULL_HANDLE;
	//	m_status = ND_INITIALED;
	//	return rtVal;
	//}
	/** \fn  Unitial
	*  \brief Unitial DataBuffer
	*/
	virtual void Unitial();
	/** \fn  Set
	*  \brief Set device para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Set(char* setting, long settingLen);
	/** \fn  Get
	*  \brief Get device para without stop
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Get(char* setting, long settingLen);
	/** \fn  Read
	*  \brief Read form buffer of device, or process data and return.
	*  \note
	*    If update buffer from physical device before read.\n
	*    T2& data is deep copied from buffer/physical device.\n
	*  \param[out/in] T2& data, T2 must be memory continues
	*  \param[in] update =true will update device buffer after read
	*  \return False if failed.
	*/
	virtual bool Read(T2 & data, bool update = false);
	/** \fn  Write
	*  \brief Write data into buffer of device,if force push buffer into physical device after write.
	*  \note
	*    T1& data is deep copied into buffer/physical device.\n
	*  \param[in] T1& data, T1 must be memory continues
	*  \return False if failed.
	*/
	virtual bool Write(const T1 & data, bool force = false);
	/** \fn  Update
	*  \brief Update buffer of device.
	*  \note
	*    If update buffer from physical device before read.\n
	*  \return False if failed.
	*/
	virtual bool Update();
};

