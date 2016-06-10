///////////////////////////////////////////////////
/** \file Link.h
 *  \brief Connect INode and drive data flow from one to another.
 *  \note
 *   detailed description
 *   detailed description.
 *  \author Leon Contact: towanglei@163.com
 *  \copyright TMTeam
 *  \version 1.0
 *  \History:
 *     Leon 2016/06/06 12:40 build\n
 */
///////////////////////////////////////////////////

#pragma once
#include "ThreadEx.h"
#include "INode.h"
#ifndef LINK_ERR
#define LINK_NORMAL     (int)0
#define LINK_ERR        (int)-1000
#define LINK_ERR_MEMOUT (int)LINK_ERR-1
#define LINK_ERR_LOCKED (int)LINK_ERR-2
#define LINK_ERR_PARA   (int)LINK_ERR-3
#endif

///////////////////////////////////////////////////
/** \class Link : public Thread, include *IDevice *IProcessor
*  \brief Drive data from IDevice to IDevice.
*  \note
*  \author Leon Contact: towanglei@163.com
*  \version 1.0
*  \date 2016/04/29 23:09
*/
template <typename T1, typename T2>
class Link : public ThreadEx
{
private:
	long motoTimes = -1; ///< Thread run times
	long motoFrameTime = 0; ///< Thread wait time
	bool motoQueueBlock = false; ///< If Thread block until queue released by other object
private:
	IDevice<T1,T2>* p_DeviceIn = 0; ///< Device object pointer
	IDevice<T1,T2>* p_DeviceOut = 0; ///< Device object pointer
	IProcessor<T1,T2>* p_Processor = 0; ///< Processor object pointer

public:
	/// Actual frame time(ms), decided by driver or link, -1 means unused
	double GetFrameTime()
	{
		return MAX((double)m_waitTime, m_taskTime);
	}
	/// Actual sample time(ms), decided by driver or link, -1 means unused
	double GetSampleTime()
	{
		return m_taskTime;
	}

	/// Set expected frame time(ms)
	void SetFrameTime(long frameTime)
	{
		m_waitTime = frameTime;
	}
	/// Set expected sample time(ms)
	virtual void SetSampleTime(long sampleTime) {}

public:
	/// Default constructor
	Link();
	/// Default destructor
	virtual ~Link();
	/** \fn  Initial
	*  \brief Initial MotoLink
	*  \param[in] DeSetting* pSetting = new MotoLinkSetting(MEGASTR& setting)
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
	*  \brief Unitial MotoLink
	*/
	virtual void Unitial();
	/** \fn  Set
	*  \brief Set running link
	*  \param[in] DeSetting* pSetting = (DeSetting*)&setting;
	*  \return bool
	*/
	virtual bool Set(char* setting, long settingLen);
	/** \fn  ReSet
	*  \brief Stop and reset link,need StartLink()
	*  \param[in] DeSetting* pSetting = (DeSetting*)&setting;
	*  \return bool
	*/
	virtual bool ReSet(char* setting, long settingLen)
	{
		Unitial();
		return Initial(setting, settingLen);
	};
	/** \fn  AttachDeviceIn
	*  \brief Add device to link
	*  \param[in] Device* pDevice = (Device*)&deviceObj;
	*  \return bool
	*/
};

