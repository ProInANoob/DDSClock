#ifndef RB_HH
#define RB_HH

#include "timer.hh"
#include "gui.hh"
#include "context.hh"
#include "controller.hh"
#include "known_devices.hh"



namespace roboClock
{
    extern bool done;
    extern Controller control;
    extern Context context;
    extern KnownDevices known_devices;
    extern std::string deviceId; 
    extern int domain_id; 
    extern std::vector<std::string> systems;


    void init(int argc, char * argv[]);
    void startup( );
    void cleanup( );
    void main_loop( );
}; // namespace robobrawl

#endif