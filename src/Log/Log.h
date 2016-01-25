/*
 * Log.h
 *
 *  Created on: Jan 12, 2016
 *      Author: brian
 */

#ifndef CHATROOM_LOG_LOG_H_
#define CHATROOM_LOG_LOG_H_


#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/unlocked_frontend.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/thread/thread.hpp>

#include "Config/Config.h"

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;


BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

class No1Log
{
public:
	typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink;

public:
	No1Log(){}
	~No1Log(){}

	static void init_log_file()
	{
		boost::shared_ptr<sinks::text_file_backend> backend = boost::make_shared<sinks::text_file_backend>(
			keywords::file_name = "log/log_%Y-%m-%d_%H-%M-%S.%N.log",
			keywords::rotation_size = 100 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::min_free_space = 30 * 1024 * 1024
		);

		backend->set_file_collector(sinks::file::make_collector(
				keywords::target = "log/log_back",
				keywords::max_size = 30 * 1024 * 1024 * 1024,
				keywords::min_free_space = 10 * 1024 * 1024 * 1024
		));

		backend->scan_for_files(sinks::file::scan_all);
		backend->auto_flush(true);

		boost::shared_ptr<TextSink> sink = boost::make_shared<TextSink>(backend);

		sink->set_formatter
		(
			expr::format("<%1%> %2% %3%")
			% expr::attr<logging::trivial::severity_level>("Severity")
			% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
			% expr::message
		);

		logging::core::get()->add_sink(sink);
	}

	static void init_log_console()
	{
		logging::add_console_log
		(
			std::cout,
			keywords::format =
			(
				expr::format("<%1%> %2% %3%")
				% expr::attr<logging::trivial::severity_level>("Severity")
				% expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
				% expr::message
			)
		);
	}

	static void init_log()
	{
		if (No1Config::get_instance()->get_log_tofile())
		{
			init_log_file();
		}

		if (No1Config::get_instance()->get_log_toconsole())
		{
			init_log_console();
		}

		boost::shared_ptr<boost::log::core> core = boost::log::core::get();
		core->set_logging_enabled(true);
		core->set_filter(expr::attr<logging::trivial::severity_level> <= static_cast<logging::trivial::severity_level>(No1Config::get_instance()->get_log_level()));
		logging::add_common_attributes();
	}

};

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(g_logger, boost::log::sources::severity_logger<boost::log::trivial::severity_level>);


#ifndef GLOBAL_LOG_SEV
#define GLOBAL_LOG_SEV(level, msg)	\
		BOOST_LOG_SEV(g_logger::get(), boost::log::trivial::severity_level::level) << msg
#endif


#ifndef MAIN_LOG_ENTRY
#define MAIN_LOG_ENTRY()      									\
	struct MainLogEntry											\
	{															\
		MainLogEntry()											\
		{														\
			boost::filesystem::path::imbue(std::locale("C"));	\
		}														\
																\
		~ MainLogEntry()										\
		{														\
			 boost::log::core::get()->remove_all_sinks();		\
		}														\
	}log_entry;
#endif


#endif /* CHATROOM_LOG_LOG_H_ */
