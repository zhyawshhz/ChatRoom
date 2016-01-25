#include "Config/Config.h"

#include <fstream>
#include <boost/program_options.hpp>

No1Config*   No1Config::m_instance = NULL;

No1Config::No1Config()
:
 m_port(-1)
,m_level(-1)
,m_tofile(false)
,m_toconsole(false)
{
	parse();
}

No1Config::~No1Config()
{
	if (m_instance) delete m_instance;
}


No1Config*
No1Config::get_instance()
{
	if (m_instance) return m_instance;
	m_instance = new No1Config();
	return m_instance;
}



bool
No1Config::parse()
{
	namespace po = boost::program_options;
	po::options_description g("options");
	g.add_options()
		("version", "print your version"),
		("help", "print your help info");

	po::options_description config("Configuration");
	config.add_options()
		("server.port", po::value<int>()->implicit_value(-1), "server port"),
		("log.log_level", po::value<int>()->implicit_value(-1), "log_level"),
		("log.log_tofile", po::value<std::string>()->default_value(""), "whether log to file"),
		("log.log_toconsole", po::value<std::string>()->default_value(""), "whether print to console");

	po::options_description command_options;
	command_options.add(g).add(config);

	po::options_description file_options;
	file_options.add(config);

	po::variables_map vm;

	std::ifstream ifs("server.conf", std::ios::binary | std::ios::in);
	if (ifs.is_open())
	{
		try
		{
			po::store(po::parse_config_file(ifs, file_options), vm);
		}
		catch(std::exception &)
		{
			ifs.close();
			return false;
		}
	}

	po::notify(vm);

	if (vm.count("server.port"))
	{
		m_port = vm["server.port"].as<int>();
	}

	if (vm.count("log.log_level"))
	{
		m_level = vm["log.log_level"].as<int>();
	}

	if (vm.count("log.log_tofile"))
	{
		m_tofile = vm["log.log_tofile"].as<std::string>() == "true";
	}
	
	if (vm.count("log.log_toconsole"))
	{
		m_toconsole = vm["log.log_toconsole"].as<std::string>() == "true";
	}


	if (vm.count("thread.handle_thrd_num"))
	{
		m_handle_thrd_num = vm["thread.handle_thrd_num"].as<int>();
	}


	if (vm.count("process.process_num"))
	{
		m_process_num = vm["process.process_num"].as<int>();
	}


	return true;
}
