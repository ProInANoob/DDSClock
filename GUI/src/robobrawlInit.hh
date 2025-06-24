#ifndef RB_HH
#define RB_HH

#include "robobrawlInit.hh"
#include "gui.hh"
#include "context.hh"



namespace roboClock
{
    extern bool done;

    void init(int argc, char * argv[]);
    void cleanup( );
    void main_loop( );
}; // namespace robobrawl

#endif