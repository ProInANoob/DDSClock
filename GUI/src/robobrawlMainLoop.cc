
#include <chrono>
#include <thread>

#include "robobrawlClock.hh"
#include "gui.hh"

// ---------------------------------------------------------------
bool                roboClock::done = false;

Controller roboClock::control;
Context roboClock::context;
roboClock::KnownDevices roboClock::known_devices;
std::string roboClock::deviceId = "";
int roboClock::domain_id = 0; 



static Timer          timer_01hz; //  0.1 Hz 
static Timer          timer_1hz;  //  1   Hz
static Timer          timer_10hz; // 10   Hz

// ---------------------------------------------------------------
void
roboClock::main_loop( )
{
  timer_01hz.start();
  timer_1hz.start();
  timer_10hz.start();
  
  while (! done )
    {
      
      
      // every 0.1 sec:
      if ( timer_01hz.elapsed().count() >= 10 )
        {
            //for gui frames.
            gui::loop(domain_id);
        }
      
      // every 1 sec:
      if ( timer_1hz.elapsed().count() >= 1000 )
        {
            //heartbeat. 
          
        }

      // every 10 sec:
      if ( timer_10hz.elapsed().count() >= 10000 )
        {
          //unused 
        }
      std::this_thread::sleep_for( std::chrono::milliseconds(5) );
    }
}
