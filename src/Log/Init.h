#ifndef No1_Log_Init_h
#define No1_Log_Init_h
#include <boost/log/support/date_time.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/unlocked_frontend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/sources/severity_feature.hpp>

enum log_level
{
	fatal 					= 0,
	error 					= 1,
	warning 				= 2,
	info  					= 3,
	debug 					= 4,
};

typedef boost::log::sources::severity_logger_mt<log_level> No1LogType;
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, boost::log::sources::severity_logger_mt<log_level>);

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", log_level);

class No1InitLog
{
public:
	static void init(const bool console_log, const bool file_log, const int level)
	{
		if (console_log)
		{
			boost::log::add_console_log
			(
				std::cout,
				boost::log::keywords::format =
				(
					boost::log::expressions::stream
					<< "<" << severity << "> "
					<< boost::log::expressions::message
				)
			);
		}

		if (file_log)
		{
			typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend> file_sink;
			const std::string time_str = boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day());
			const uintmax_t max_file_size = 100 * 1024 * 1024;
			boost::shared_ptr<boost::log::sinks::text_file_backend> backend 
				= boost::make_shared<boost::log::sinks::text_file_backend>
				(
				 boost::log::keywords::file_name = "log/log_" + time_str + "_%N.txt",
				 boost::log::keywords::rotation_size = max_file_size,
				 boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0)
			);
			
			backend->set_file_collector(boost::log::sinks::file::make_collector(
				boost::log::keywords::target = "log/logs.d",
				boost::log::keywords::max_size = 4000 * max_file_size,
				boost::log::keywords::min_free_space = 64 * max_file_size
			));
			
			backend->scan_for_files(boost::log::sinks::file::scan_all);
			backend->auto_flush(true);

			boost::shared_ptr<file_sink> sink = boost::make_shared<file_sink>(backend);
			sink->set_formatter
			(
				boost::log::expressions::stream
				<< "<" << severity << ">"
				<< boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f%Q")
				<< " "
				<< boost::log::expressions::message
			);
			boost::log::core::get()->add_sink(sink);
		}

		boost::shared_ptr<boost::log::core> core = boost::log::core::get();
		core->set_logging_enabled(true);
		core->set_filter(severity <= static_cast<log_level>(level));
		boost::log::add_common_attributes();
		core->add_global_attribute("Scope", boost::log::attributes::named_scope());
	}
};
#endif
