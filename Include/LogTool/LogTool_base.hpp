/* \file LogTool_base.hpp
  \brief Out put debug info, used in try-catch block
  \author Leon Contact: towanglei@163.com
  \copyright TMTeam
  \version 2.0
  \History:
	 Leon 2016/06/20 12:04 pack into *.hpp files.\n
	 1.0 : Leon 2016/06/17 12:04 build.\n
*/
#pragma once
#ifndef LOGTOOL_BASE
#define LOGTOOL_BASE

#include <iostream>
#include <boost/log/trivial.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include <boost/log/sources/logger.hpp>

#include <boost/log/support/date_time.hpp>

#include <boost/thread/mutex.hpp>
//#include <boost\thread.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;
using namespace boost;
using namespace boost::log::trivial;
//! Trivial severity levels
//enum severity_level
//{
//	trace,
//	debug,
//	info,
//	warning,
//	error,
//	fatal
//};

//#define  LOG_ENABLE
/// ShowMode
#define LOG_NULL      (int)0x00
#define LOG_CONSOLE   (int)0x01
#define	LOG_FILE      (int)0x10
#define	LOG_BOTH      (int)0x11

/* \class LogTool : 
   \brief A lazy mode singleton class:LogTool£¬Initial debug tool will construct.
   \note
      Use like: 
	    LogTool::Initial("C\\log.log");//just call once anywhere.
		   :
	    WRITE_LOG(severity_level::error, "...");

   \author Leon Contact: towanglei@163.com
   \version 1.0
   \date 2016/06/20 19:29
*/
class LogTool
{
private:
	mutex m_mutex;
public:
	bool isInitialed = false;
	bool isEnabled = false;
	// Now, let's try logging with severity
	src::severity_logger< logging::trivial::severity_level > slg;
	/// Initial LogTool behavior.
	void Initial(char* pathName, int logMod = LOG_FILE, severity_level levelFilter = logging::trivial::info)
	{
		m_mutex.lock();
		if (logMod == LOG_NULL)
		{
			isEnabled = false;
			return;
		}
		else
		{
			isEnabled = true;
		}
		if (isInitialed) return;
		m_mutex.unlock();
#ifdef _DEBUG
		if ((logMod & LOG_CONSOLE)== LOG_CONSOLE)
		{
			logging::add_console_log
			(
				std::clog,
				keywords::format = expr::stream
			    << "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
				<< "]<" << expr::attr< logging::trivial::severity_level >("Severity")// == severity_level::warning ? "warning" : "error")//<< logging::trivial::severity //
				<< ">(" << expr::attr<attrs::current_thread_id::value_type >("ThreadID")
				<< "){" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
				<< "}:" << expr::message
			);
		}
		if ((logMod & LOG_FILE)== LOG_FILE)
		{
			logging::add_file_log
			(
				keywords::file_name = pathName,
				keywords::rotation_size = 10 * 1024 * 1024,
				keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
				keywords::format = expr::stream
				<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
				<< "]<" << expr::attr< logging::trivial::severity_level >("Severity")// == severity_level::warning ? "warning" : "error")//<< logging::trivial::severity //
				<< ">(" << expr::attr<attrs::current_thread_id::value_type >("ThreadID")
				<< "){" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
				<< "}:" << expr::message
			);
		}
		logging::core::get()->set_filter
		(
			logging::trivial::severity >= levelFilter/*logging::trivial::info*/
		);
		// Also let's add some commonly used attributes, like timestamp and record counter.  /*LogTool::GetInstance()->slg.add_attribute("Uptime", attrs::timer());*/
		logging::add_common_attributes();
		logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
#endif
		m_mutex.lock();
		isInitialed = true;
		m_mutex.unlock();
	};
private:
	/// Singleton construction.
	LogTool()
	{

	};
public:
	/// Singleton get instance method, lazy mode.
	static LogTool * GetInstance()
	{
		static LogTool instance;
		return &instance;
	};
};

//#ifndef WRITE_LOG
//#ifdef LOG_ENABLE
//#define WRITE_LOG(level, tag) {if(LogTool::GetInstance()->isEnabled) {BOOST_LOG_FUNCTION();BOOST_LOG_SEV(LogTool::GetInstance()->slg, level) << tag;}}
//#else
//#define WRITE_LOG(level, tag) BOOST_LOG_TRIVIAL(level)<<tag; //severity_level::error, ""
//#endif
//#endif
//
//#ifndef INITIAL_LOG
//#ifdef LOG_ENABLE
//#define INITIAL_LOG(pathName, logMod, levelFilter) {LogTool::GetInstance()->Initial(pathName, logMod, levelFilter);}
//#else
//#define INITIAL_LOG(pathName, logMod, levelFilter) //"", LOG_CONSOLE|LOG_FILE, severity_level::info
//#endif
//#endif

#endif

