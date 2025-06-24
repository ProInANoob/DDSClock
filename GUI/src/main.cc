#include <dds/dds.hpp>

#include <vector>
#include <map>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <array>
#include <iostream>

#include <thread>
#include "types.hh"
#include "robobrawlinit.hh"
#include "gui.hh"
#include "context.hh"


bool roboClock::done = false;
//dds::domain::DomainParticipant dp;
//dds::pub::Publisher pub;
//dds::sub::Subscriber sub;
int domain_id = 0; // think ill just pull em all in this...
DDS_InstanceHandle_t     my_handle = DDS_HANDLE_NIL; 
Context context;

// so a clock list probably... Id like to orginize the buttons a little more than just a list, but idk how

std::vector<ClockInfo*> clocks;

std::map<std::array<unsigned char, 12>, std::vector<ButtonInfo*>*> matchedButtons; // this IG? keyed by clock key 
//and 
std::vector<ButtonInfo*> unknownButtons;






// ---------------------------------------------------------------
int main(int argc, char *argv[])
{
    roboClock::init(argc, argv);
    roboClock::main_loop();
    roboClock::cleanup();

    return 0;
}