#include "Server/Config.h"
#include <fstream>
#include <boost/program_options.hpp>
#include "Log/Log.h"

No1Config::No1Config()
:
 server_port(-1)
,log_level(-1)
,log_tofile(false)
,log_toconsole(false)
{
}

No1Config::~No1Config()
{
}

bool
No1Config::parse(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description g("options");
	g.add_options()
		("version", "print your version"),
		("help", "print your help info");

	po::options_description config("Configuration");
	config.add_options()
		("server.port", po::value<std::int32_t>()->implicit_value(-1), "server port"),
		("log.log_level", po::value<std::int32_t>()->implicit_value(-1), "log_level"),
		("log.log_tofile", po::value<std::string>()->default_value(""), "whether log to file"),
		("log.log_toconsole", po::value<std::string>()->default_value(""), "whether print to console");

	po::options_description command_options;
	command_options.add(g).add(config);

	po::options_description file_options;
	file_options.add(config);

	po::veriable_map vm;

	try
	{
		po::store(po::parse_command_line(argc, argv, command_options), vm);
	}
	catch(std::exception &)
	{
		NO1_CLASS_LOG(error, "Failed to parse command line!!!");
		return false;
	}

	std::ifstream ifs("server.conf", std::ios::binary | std::ios::in);
	if (ifs.is_good())
	{
		try
		{
			po::store(po::parse_config_file(ifs, file_options), vm);
		}
		catch(std::exception &)
		{
			ifs.close();
			NO1_CLASS_LOG(error, "Failed to parse config file!!!");
			return false;
		}
	}

	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << command_options << std::endl;
	}

	if (vm.count("version"))
	{
		std::cout << PACKAGE_VERSION << std::endl;
	}

	if (vm.count("server.port"))
	{
		server_port = vm["server.port"].as<std::int32_t>();
	}

	if (vm.count("log.log_level"))
	{
		log_level = vm["log.log_level"].as<std::int32_t>();
	}

	if (vm.count("log.log_tofile"))
	{
		log_tofile = vm["log.log_tofile"].as<std::string>() == "true";
	}
	
	if (vm.count("log.log_toconsole"))
	{
		log_toconsole = vm["log.log_toconsole"].as<std::string>() == "true";
	}

	return true;
}
