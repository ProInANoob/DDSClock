#ifndef RB_HH
#define RB_HH

#include "timer.hh"
#include "gui.hh"
#include "context.hh"
#include "controller.hh"



namespace roboClock
{
    extern bool done;
    extern Controller control;
    extern Context context;
    extern std::string deviceId; 
    extern int domain_id; 
 // hmmm I get messges in roboCLock::controler::function, so put this in controler? and access it there ?, check sys stae against clock state in the main loop 
    extern std::map<std::string, int> clockStates;
    extern std::map<std::string, float> clockLengthSec;
    extern std::map<std::string, NewTimer> clockTimers;


    void init(int argc, char * argv[]);
    void startup( );
    void cleanup( );
    void main_loop( );
}; // namespace robobrawl

#endif