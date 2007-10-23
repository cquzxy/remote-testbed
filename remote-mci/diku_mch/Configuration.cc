#include "Configuration.h"

namespace remote { namespace diku_mch {

void Configuration::printHelp(po::options_description& desc)
{
	std::cerr << PACKAGE_NAME << " (MCH) version " << PACKAGE_VERSION
		  << std::endl << std::endl
		  << desc
		  << std::endl;
	exit(0);
}

void Configuration::read(int argc, char **argv)
{
	// command line options
	po::options_description cmdline_options("Command line options");

	cmdline_options.add_options()
	("help", "Print usage help and exit")
	("config-help", "Print config file usage help and exit")
	("config-file",
		 po::value<std::string>()->default_value("/etc/diku_mch.cfg"),
		 "Path to the configuration file.")
	("daemonize",
		po::value<int>()->default_value(false),
		"Run as a daemon.");

	// declare all configuration groups
	po::options_description config("Configuration options");

	config.add_options()
	("devicePath",
		po::value<std::string>()->default_value("/dev/remote"),
		"Path to the remote mote device hierarchy.")
	("usbPlugEventPipe",
		po::value<std::string>()->default_value("/var/run/motehost.events"),
		"Path to the fifo notifying the motehost of mote hotplug events.")
	("serverAddress",
		po::value<std::string>()->default_value("localhost"),
		"DNS or IP address of the mote server.")
	("serverPort",
		po::value<uint16_t>()->default_value(10001),
		"Port number to use for connecting to the mote server.")
	("serverConnectionRetryInterval",
		po::value<uint64_t>()->default_value(30),
		"Number of seconds to wait between server connection retries.")
	/* FIXME: To make the MCH support multiple platforms, this should
	 * be made platform aware. It might also require a moteImageExtension
	 * option to set the extension of flash images although it having it
	 * hardcoded for each platform should be OK and simpler. */
	("moteProgrammerPath",
		po::value<std::string>()->default_value("/usr/bin/hc08sprg"),
		"Path to the DIG528-2 flash programmer.")
	;

	try {
		store(po::parse_command_line(argc, argv, cmdline_options), vm);
	} catch (boost::program_options::error exception) {
		std::cerr << "Error while parsing command line options: "
			  << exception.what() << std::endl << std::endl;
		printHelp(cmdline_options);
	}
	notify(vm);

	if (vm.count("help"))
		printHelp(cmdline_options);
	if (vm.count("config-help"))
		printHelp(config);

	// get the configuration settings from a file
	std::ifstream ifs(Configuration::vm["config-file"].as<std::string>().c_str());
	try {
		store(parse_config_file(ifs, config), vm);
	} catch (boost::program_options::error exception) {
		std::cerr << "Error while parsing configuration file: "
			  << exception.what() << std::endl << std::endl;
		printHelp(config);
	}
	notify(vm);
}

po::variables_map Configuration::vm;

}}
