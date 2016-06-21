/* \file LogTool.h
   \brief A convenience header for \LogTool library
	 \note 
	 Include a singleton class:LogTool£¬Initial debug tool will construct.
	 Use like:
	 LogTool::Initial("C\\log.log");//just call once anywhere.
	 :
	 WRITE_LOG(severity_level::error, "...");

   \author Leon Contact: towanglei@163.com
   \copyright TMTeam
   \version 1.0
   \History:
      Leon 2016/06/20 12:04 build\n
*/
#pragma once
#ifndef LOGTOOL
#define LOGTOOL
#include "LogTool\LogTool_base.hpp"

/// Enable/disable WRITE_LOG by define/comment this macros.
#define LOG_ENABLE

///// Debug Level
//enum severity_level
//{
//	traceLevel = (int)1,
//	infoLevel = (int)2,
//	debugLevel = (int)3,
//	warningLevel = (int)4,
//	errorLevel = (int)5,
//	fatalLevel = (int)6
//};

#ifndef WRITE_LOG
#ifdef LOG_ENABLE
#define WRITE_LOG(level, tag) {BOOST_LOG_FUNCTION();BOOST_LOG_SEV(LogTool::GetInstance()->slg, level) << tag;}
#else
#define WRITE_LOG(level, tag)
#endif
#endif

#endif



