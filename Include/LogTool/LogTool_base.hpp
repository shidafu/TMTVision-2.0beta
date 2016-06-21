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

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;
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

/* \class LogTool : 
   \brief A singleton class:LogTool£¬Initial debug tool will construct.
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
public:
	// Now, let's try logging with severity
	src::severity_logger< logging::trivial::severity_level > slg;
	char logPathName[512] = "";
	void Initial(char* pathName)
	{
		if (logPathName[0] != 0) return;
		strcpy_s(logPathName, sizeof(logPathName), pathName);
#ifdef _DEBUG
		//logging::add_console_log
		//(
		//	std::clog, 
		//	keywords::format = "sample_%N.log",
		//	keywords::filter = expr::attr< severity_level >("Severity") >= warning,
		//	keywords::format = expr::stream
		//  << "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
		//	<< "]<" << expr::attr< logging::trivial::severity_level >("Severity")// == severity_level::warning ? "warning" : "error")//<< logging::trivial::severity //
		//	<< ">(" << expr::attr<attrs::current_thread_id::value_type >("ThreadID")
		//	<< "){" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
		//	<< "}:" << expr::message
		//);
		logging::add_file_log
		(
			keywords::file_name = logPathName,
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::format = expr::stream
			<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
			<< "]<" << expr::attr< logging::trivial::severity_level >("Severity")// == severity_level::warning ? "warning" : "error")//<< logging::trivial::severity //
			<< ">(" << expr::attr<attrs::current_thread_id::value_type >("ThreadID")
			<< "){" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
			<< "}:" << expr::message
		);
		logging::core::get()->set_filter
		(
			logging::trivial::severity >= logging::trivial::info
		);
		// Also let's add some commonly used attributes, like timestamp and record counter.  /*LogTool::GetInstance()->slg.add_attribute("Uptime", attrs::timer());*/
		logging::add_common_attributes();
		logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
#endif
	};
private:
	LogTool()
	{

	};
public:
	static LogTool * GetInstance()
	{
		static LogTool instance;
		return &instance;
	};
};

#endif

