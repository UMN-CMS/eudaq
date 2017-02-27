#include "eudaq/Controller.hh"
#include "eudaq/Logger.hh"
#include "eudaq/Timer.hh"
#include "eudaq/Utils.hh"
#include "eudaq/OptionParser.hh"
#include "eudaq/Configuration.hh"

#include "PIController.hh"

#include <iostream>
#include <ostream>
#include <vector>
//#include <sched.h>

using namespace std;


PIController::PIController(const std::string &name,
			     const std::string &runcontrol)
  : eudaq::Controller(name, runcontrol), m_terminated(false), 
m_name(name), 
m_hostname(hostname),
m_portname(portname)
//m_pinnr(0), 
//m_waiting_time(4000) 
{ }

void PIController::OnInitialise(const eudaq::Configuration & init) {
      try {
        std::cout << "Reading: " << init.Name() << std::endl;
        
        PIWrapper wrapper(m_portnumber, m_hostname);

        if (!wrapper.axisIsReferenced(m_axis1)) { 
		std::cout << "Axis1 not referenced! Please use PIControl!" << std::endl; }
        if (!wrapper.axisIsReferenced(m_axis2)) { 
		std::cout << "Axis2 not referenced! Please use PIControl!" << std::endl; }
        if (!wrapper.axisIsReferenced(m_axis3)) { 
		std::cout << "Axis3 not referenced! Please use PIControl!" << std::endl; }
        if (!wrapper.axisIsReferenced(m_axis4)) { 
		std::cout << "Axis4 not referenced! Please use PIControl!" << std::endl; }

        SetConnectionState(eudaq::ConnectionState::STATE_UNCONF, "Initialised (" + init.Name() + ")");
      } 
      catch (...) {
        // Message as cout in the terminal of your producer
        std::cout << "Unknown exception" << std::endl;
        // Message to the LogCollector
        EUDAQ_ERROR("Unknown error during initialization from PI controller");
        // Otherwise, the State is set to ERROR
        SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Initialisation Error");
      }
}

void PIController::OnConfigure(const eudaq::Configuration &config) {
  try {
    std::cout << "Configuring: " << config.Name() << std::endl;

  // Read and store configured pin from eudaq config:
  //m_pinnr = config.Get("pinnr", 0);
  //std::cout << m_name << ": Configured pin " << std::to_string(m_pinnr) << " to be set to high." << std::endl;
  //EUDAQ_INFO(string("Configured pin " + std::to_string(m_pinnr) +
  //                  " of the Raspberry Pi controller to be set to high."));

  // Store waiting time in ms before the pin is set to high in OnRunStart():
  //m_waiting_time = config.Get("waiting_time", 4000);
  //std::cout << m_name << ": Waiting " << std::to_string(m_waiting_time) << "ms for pin action." << std::endl;
  //EUDAQ_INFO(string("Waiting " + std::to_string(m_waiting_time) +
  //                  "ms before enabling output pin at run start."));

  //if(wiringPiSetupGpio() == -1) {
  //  std::cout << "WiringPi could not be set up" << std::endl;
  //  throw eudaq::LoggedException("WiringPi could not be set up");
  //}
  // Set pin mode to output:
  //pinMode(m_pinnr, OUTPUT);

  // Initialize as low:
  //digitalWrite(m_pinnr, 0);

    SetConnectionState(eudaq::ConnectionState::STATE_CONF, "Configured (" + config.Name() + ")");
  } 
  catch (...) {
    EUDAQ_ERROR(string("Unknown exception."));
    SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Unknown exception.");
  }
}

void PIController::OnStartRun(unsigned runnumber) {

  try {
    // Wait defined time before returning OK.
    //std::cout << "Waiting configured time of "
    //	      << std::to_string(m_waiting_time) << "ms" << std::endl;
    //eudaq::mSleep(m_waiting_time);
    // Set configured pin to high:
    //std::cout << "Calling digitalWrite() to set pin high" << std::endl;
    //digitalWrite(m_pinnr, 1);
    //EUDAQ_INFO(string("GPIO pin " + std::to_string(m_pinnr) + " now high."));


    SetConnectionState(eudaq::ConnectionState::STATE_RUNNING, "Running");
  } 
  catch (...) {
    EUDAQ_ERROR(string("Unknown exception."));
    SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Unknown exception.");
  }
}

// This gets called whenever a run is stopped
void PIController::OnStopRun() {

  try {
    // Set configured pin to low:
    //std::cout << "Calling digitalWrite() to set pin low" << std::endl;
    //digitalWrite(m_pinnr, 0);
    //EUDAQ_INFO(string("GPIO pin " + std::to_string(m_pinnr) + " now low."));
    
    SetConnectionState(eudaq::ConnectionState::STATE_CONF, "Stopped");
  } 
  catch (const std::exception &e) {
    printf("While Stopping: Caught exception: %s\n", e.what());
    SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Stop Error");
  } 
  catch (...) {
    printf("While Stopping: Unknown exception\n");
    SetConnectionState(eudaq::ConnectionState::STATE_ERROR, "Stop Error");
  }
}

void PIController::OnTerminate() {

  std::cout << "PIController terminating..." << std::endl;

  m_terminated = true;
  std::cout << "PIController " << m_name << " terminated." << std::endl;
}

void PIController::Loop() {

  // Loop until Run Control tells us to terminate
  while (!m_terminated) {
    // Move this thread to the end of the scheduler queue:
    //sched_yield();
	eudaq::mSleep(20);
    continue;
  }

}

// The main function that will create a Producer instance and run it
int main(int /*argc*/, const char **argv) {
  // You can use the OptionParser to get command-line arguments
  // then they will automatically be described in the help (-h) option
  eudaq::OptionParser op("PI Stages Controller", "0.0", "Run options");
  eudaq::Option<std::string> rctrl(op, "r", "runcontrol",
                                   "tcp://localhost:44000", "address",
                                   "The address of the RunControl.");
  eudaq::Option<std::string> level(
      op, "l", "log-level", "NONE", "level",
      "The minimum level for displaying log messages locally");
  eudaq::Option<std::string> name(op, "n", "name", "PIController", "string",
                                  "The name of this Producer/Controller");
  eudaq::Option<std::string> hostname(op, "ho", "HostName", "192.168.22.5", "string", "set hostname for TCP/IP connection");
  eudaq::Option<int> portnumber(op, "p", "PortNumber", 50000, "number", "set portnumber for TCP/IP connection");


  try {
    // This will look through the command-line arguments and set the options
    op.Parse(argv);
    // Set the Log level for displaying messages based on command-line
    EUDAQ_LOG_LEVEL(level.Value());

    // Create the instance
    PIController controller(name.Value(), rctrl.Value());
    // And set it running...
    controller.Loop();

    // When the keep-alive loop terminates, it is time to go
    std::cout << "Quitting" << std::endl;
  } catch (...) {
    // This does some basic error handling of common exceptions
    return op.HandleMainException();
  }
  return 0;
}