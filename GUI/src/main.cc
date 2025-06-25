#include <dds/dds.hpp>

#include <vector>
#include <map>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <array>
#include <iostream>

#include <thread>
#include "robobrawlClock.hh"
#include "context.hh"
#include "system.hh"

//dds::domain::DomainParticipant dp;
//dds::pub::Publisher pub;
//dds::sub::Subscriber sub;
int domain_id = 0; // think ill just pull em all in this...
DDS_InstanceHandle_t     my_handle = DDS_HANDLE_NIL; 
std::string deviceId = "";

// so a clock list probably... Id like to orginize the buttons a little more than just a list, but idk how

std::vector<System> activeSystems;
std::map<std::string, System> Systems;





// ---------------------------------------------------------------
int main(int argc, char *argv[])
{
    roboClock::init(argc, argv);
    roboClock::startup( );
    roboClock::main_loop( );
    roboClock::cleanup( );

    return 0;
}