
#include <chrono>
#include <thread>
#include <cmath>

#include "robobrawlClock.hh"
#include "gui.hh"

// ---------------------------------------------------------------
bool roboClock::done = false;

Controller roboClock::control;
Context roboClock::context;
std::string roboClock::deviceId = "";
int roboClock::domain_id = 0;

// state encoding : -1 retain whatever was going on before. 0 is off- nothing. 1 - to zeros, 2 - full time, 3 - start run, 4 running, 5 paused, 6 resume,- 7 orange win - 8 blue win - 9 red for just in case. 
std::map<std::string, int> roboClock::clockStates;
std::map<std::string, float> roboClock::clockLengthSec;
std::map<std::string, NewTimer> roboClock::clockTimers;

static Timer timer_01hz; //  0.1 Hz
static Timer timer_1hz;  //  1   Hz
static Timer timer_10hz; // 10   Hz

// ---------------------------------------------------------------
void roboClock::main_loop()
{
  timer_01hz.start();
  timer_1hz.start();
  timer_10hz.start();

  while (!done)
  {

    // every 0.1 sec:
    if (timer_01hz.elapsed().count() >= 10)
    {
      timer_01hz.start();
      // for gui frames.
      gui::loop(domain_id);
    }

    // every 1 sec:
    if (timer_1hz.elapsed().count() >= 1000)
    {
      timer_1hz.start();
      // heartbeat.
      std::cout << "Tick....\n";
      control.writeHeartbeat();
    }

    // every 10 sec:
    if (timer_10hz.elapsed().count() >= 10000)
    {
      timer_10hz.start();
      // check for dead heartbeats.
    }


    // do clock timer handling..
    // for all th clocks, check state and handle, senign a message



    for( auto & pair : roboClock::clockStates){
      ClockCommand comm;
      timeValue time;
      float timeNum;
      std::cout << "start\n";
      switch(pair.second){
          case -1: // do nothing 
            
            break;
          case 0: // turn off
            comm.isOff(1);
            comm.sysName(pair.first);

            control.writeToClock(comm);

            roboClock::clockStates[pair.first] = -1; // change handled, now idle - NOTE: may have issues calling htis with a running clock, we wont know that it was running... lost time, state, and may lose resorces... idk.

            break;
          case 1: // go to zeroes
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_RED);
            
            time.minutes( 0 );
            time.seconds( 0 );
            comm.time( time );

            comm.sysName(pair.first);
            control.writeToClock(comm);

            roboClock::clockStates[pair.first] = -1; // change handled, now idle

            break;
          case 2: // full time display
            time.seconds( fmod(roboClock::clockLengthSec[pair.first], 60) );
            time.minutes( int(roboClock::clockLengthSec[pair.first] / 60) );

            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_GREEN);
            comm.time( time );

            comm.sysName( pair.first );
            control.writeToClock(comm);

            roboClock::clockStates[pair.first] = -1; // change handled, now idle


            break;
          case 3: // start
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_GREEN);

            time.seconds( fmod(roboClock::clockLengthSec[pair.first], 60) );
            time.minutes( int(roboClock::clockLengthSec[pair.first] / 60) );
            comm.time( time );
            control.writeToClock(comm);

            roboClock::clockTimers[pair.first].start();
            
            roboClock::clockStates[pair.first] = 4; // claock timeer started, and clock inited to top time, transition into the running state. 

            break;
          case 4: // running
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            
            // so duration - clock timer -> get elapsed = remaining time in seconds, then do color check and display. 
            timeNum = roboClock::clockLengthSec[pair.first] - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.

            if(timeNum <= 0){
              roboClock::clockStates[pair.first] = 1; // or solid color... isk where to direct this. 
              timeNum = 0;
            }

            time.seconds( fmod(timeNum, 60) );
            time.minutes( int(timeNum / 60) );
            comm.time( time );
            control.writeToClock(comm);
            
            //color check ( do by portiaon left? 10% -> yello kinda thing, or just 1min -> 10sec.)
            // also need a zero check.. 

            break;

          case 5: // pause.

            roboClock::clockTimers[pair.first].pause();
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            
            // so duration - clock timer -> get elapsed = remaining time in seconds, then do color check and display. 
            timeNum = roboClock::clockLengthSec[pair.first] - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.

            time.seconds( fmod(timeNum, 60) );
            time.minutes( int(timeNum / 60) );
            comm.time( time );
            control.writeToClock(comm);
            
            roboClock::clockStates[pair.first] = 4; // put back into running but with a paused timer. 

            break;
          case 6:
            roboClock::clockTimers[pair.first].resume();
            roboClock::clockStates[pair.first] = 4; // put back into running but with a resumed timer. 
            control.writeToClock(comm);
            
            break;
          case 7:
            time.seconds( 88 );
            time.minutes( 88 );

            comm.isOff( 0 );
            comm.blueGearColor( Colors::COLOR_ORANGE );
            comm.orangeGearColor( Colors::COLOR_ORANGE );
            comm.mainColor( Colors::COLOR_ORANGE );

            comm.doDisplayTime( 1 );

            comm.time( time );
            control.writeToClock( comm );

            roboClock::clockStates[pair.first] = -1;
            break;
          case 8:
            time.seconds( 88 );
            time.minutes( 88 );

            comm.isOff( 0 );
            comm.blueGearColor( Colors::COLOR_BLUE );
            comm.orangeGearColor( Colors::COLOR_BLUE );
            comm.mainColor( Colors::COLOR_BLUE );

            comm.doDisplayTime( 1 );

            comm.time( time );
            control.writeToClock( comm );

            roboClock::clockStates[pair.first] = -1;
            break;

          case 9:
            time.seconds( 88 );
            time.minutes( 88 );

            comm.isOff( 0 );
            comm.blueGearColor( Colors::COLOR_RED );
            comm.orangeGearColor( Colors::COLOR_RED );
            comm.mainColor( Colors::COLOR_RED );

            comm.doDisplayTime( 1 );

            comm.time( time );
            control.writeToClock( comm );

            roboClock::clockStates[pair.first] = -1;
            break;


          default:
            std::cout << "unknown Clock state in: " << pair.first << std::endl;
            break;
      }
      
      std::cout << "end\n";



    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}
