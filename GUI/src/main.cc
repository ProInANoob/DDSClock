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




// ---------------------------------------------------------------
int main(int argc, char *argv[])
{
        roboClock::init(argc, argv);
        roboClock::startup( );
        roboClock::main_loop( );
        roboClock::cleanup( );
    
    return 0;
}