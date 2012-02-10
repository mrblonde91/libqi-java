/*
** Author(s):
**  - Nicolas HUREAU <nhureau@aldebaran-robotics.com>
**
** Copyright (C) 2011 Aldebaran Robotics
*/

#include <iostream>
#include <sstream>
#include <cstring>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <qi/log.hpp>

#include <signal.h>

#include "daemon.h"
#include "gateway.hpp"
#include "master.hpp"

namespace po = boost::program_options;

static
void launch(const char* gatewayHost, unsigned short gatewayPort,
            const std::string& masterAddress);
static
void sig_handler(int signum, siginfo_t* info, void* vctx);

int main(int argc, char *argv[])
{
  // declare the program options
  po::options_description desc(("Usage:\n  " + std::string(argv[0]) +
                                " [options]\nOptions").c_str());
  desc.add_options()
    ("help,h", "Print this help.")
    ("gateway-listen,l",
      po::value<std::string>()->default_value(std::string("127.0.0.1")),
      "Address to listen to (Gateway)")
    ("gateway-port,p",
      po::value<unsigned short>()->default_value(12890),
      "Port to listen on (Gateway)")
    ("master-listen,a",
      po::value<std::string>()->default_value(std::string("127.0.0.1")),
      "Address to listen to (Master)")
    ("master-port,o",
      po::value<unsigned short>()->default_value(5555),
      "Port to listen on (Master)")
    ("daemon,d", "Daemonize the server");

  // allow listen address to be specified as the first arg
  po::positional_options_description pos;
  pos.add("listen", 1);

  // parse and store
  po::variables_map vm;
  try
  {
    po::store(po::command_line_parser(argc, argv).
      options(desc).positional(pos).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
      std::cout << desc << "\n";
      return 0;
    }

    if (vm.count("daemon"))
      if (!daemonize())
        throw std::runtime_error("Could not daemonize");

    if (vm.count("gateway-listen") && vm.count("gateway-port") &&
        vm.count("master-listen") && vm.count("master-port"))
    {
      struct sigaction sigact;
      memset(&sigact, 0, sizeof (sigact));
      sigact.sa_sigaction = sig_handler;

      if (sigaction(SIGINT, &sigact, NULL) == -1)
        throw std::runtime_error("Could not set SIG handler");

      std::stringstream masterAddress;
      masterAddress << vm["master-listen"].as<std::string>() << ":"
                    << vm["master-port"].as<unsigned short>();

      launch(vm["gateway-listen"].as<std::string>().c_str(),
             vm["gateway-port"].as<unsigned short>(),
             masterAddress.str());
    }
    else
    {
      std::cerr << desc << "\n";
      return 1;
    }
  }
  catch (const std::exception& e)
  {
    qiLogFatal("qigateway", "%s", e.what());
    return 1;
  }

  return 0;
}

static
void launch(const char* gatewayHost, unsigned short gatewayPort,
            const std::string& masterAddress)
{
  qiLogInfo("qigateway", "Launching QiMessaging Gateway");
  boost::thread gatewayThread(qi::gateway::Gateway::launch,
      gatewayHost, gatewayPort);

  qiLogInfo("qigateway", "Launching QiMessaging Master");
  boost::thread masterThread(qi::gateway::Master::launch,
      masterAddress);

  for (;;)
  {
    if (gatewayThread.timed_join(boost::posix_time::milliseconds(10)))
    {
      gatewayThread = boost::move(boost::thread(qi::gateway::Gateway::launch,
            gatewayHost, gatewayPort));
    }

    if (masterThread.timed_join(boost::posix_time::milliseconds(10)))
    {
      masterThread = boost::move(boost::thread(qi::gateway::Master::launch,
            masterAddress));
    }

    qi::os::sleep(1);
  }
}

static
void sig_handler(int signum, siginfo_t* info, void* vctx)
{
  (void) signum;
  (void) info;
  (void) vctx;

  qiLogInfo("qigateway", "Stopping QiMessaging Gateway + Master");

  exit(0);
}