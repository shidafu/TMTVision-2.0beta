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
//#define LOG_ENABLE

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
#define LOG_ENABLE
#ifndef WRITE_LOG
#ifdef LOG_ENABLE
#define WRITE_LOG(level, tag) {if(LogTool::GetInstance()->isEnabled) {BOOST_LOG_FUNCTION();BOOST_LOG_SEV(LogTool::GetInstance()->slg, level) << tag;}}
#else
#define WRITE_LOG(level, tag) BOOST_LOG_TRIVIAL(level)<<tag;//severity_level::error, ""
#endif
#endif

#ifndef INITIAL_LOG
#ifdef LOG_ENABLE
#define INITIAL_LOG(pathName, logMod, levelFilter) {LogTool::GetInstance()->Initial(pathName, logMod, levelFilter);}
#else
#define INITIAL_LOG(pathName, logMod, levelFilter) //"", LOG_FILE|LOG_FILE, severity_level::info
#endif
#endif

#endif





