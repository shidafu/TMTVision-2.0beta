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
/** \class DataBuffer  \implements IProducer
*  \brief Data buffer object, read and write circular_buffer.
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/04/29 10:34
*/
template <typename T>
class DataBuffer : virtual public IProducer<T>, virtual public IConsumer<T>
{
private:
	int bufferMaxSize = 8; ///< Data list size
	enum { BUF_QUEUE = 0, BUF_STACK = 1 };
	int bufferType = BUF_QUEUE; ///< Is queue or stack
	bool resizable = false; ///< Is buffer resizable
	circular_buffer<T>* p_CircularBuffer = 0;
public:
	/// Default constructor
	DataBuffer();
	/** \fn  ~DataBuffer
	*  \brief virtual destruct function, avoid delete wrong object
	*/
	virtual ~DataBuffer();
	/** \fn  Initial
	*  \brief Initial DataBuffer
	*  \param[in] char* setting as json string
	*  \return bool
	*/
	virtual bool Initial(char* setting, long settingLen);
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
	*  \brief Read form buffer of device.
	*  \note
	*    If update buffer from physical device before read.\n
	*    T& data is deep copied from buffer/physical device.\n
	*  \param[out/in] T& data, T must be memory continues
	*  \return False if failed.
	*/
	virtual bool Read(T & data);
	/** \fn  Update
	*  \brief Update buffer of device.
	*  \note
	*    If update buffer from physical device before read.\n
	*  \return False if failed.
	*/
	virtual bool Update();	
	/** \fn  Write
	*  \brief Write data into buffer of device.
	*  \note
	*    T& data is deep copied into buffer/physical device.\n
	*  \param[in] T& data, T must be memory continues
	*  \return False if failed.
	*/
	virtual bool Write(const T & data);
	/** \fn  Clear
	*  \brief Clear buffer of device.
	*  \note
	*    If update buffer from physical device before write.\n
	*  \return False if failed.
	*/
	virtual bool Clear();
};

