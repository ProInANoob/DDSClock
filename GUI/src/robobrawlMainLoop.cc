
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
  SysName &temp = SysName();
  temp.deviceId( "hi " );
  temp.sysName ( "hp2" );
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
      control.writeSysName(temp);
      //control.writeHeartbeat();
    }

    // every 10 sec:
    if (timer_10hz.elapsed().count() >= 10000)
    {
      timer_10hz.start();
      // check for dead heartbeats.
    }





    for( auto & pair : roboClock::control.systemStates){
      ClockCommand comm;
      timeValue time;
      float timeNum;
      std::cout << "start\n";
      switch(pair.second){
          case -1: // do nothing 
            
            break;
          case 0: // idle state... 
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_GREEN ); // red???/ green? idk man
            comm.blueGearColor( Colors::COLOR_BLACK ); //off
            comm.orangeGearColor( Colors::COLOR_BLACK ); //off
            time.minutes( 0 );
            time.seconds( 0 );
            comm.time( time );
            control.writeToClock( comm ); // ill be writing this ALOT... but for timeing ill do it anyway? just be good at cleanup on esp... 


            break;
          case 1: // one is ready... orange
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_RED);
            comm.blueGearColor( Colors::COLOR_BLACK ); //off
            comm.orangeGearColor( Colors::COLOR_ORANGE); // trun this one on
            time.minutes( 0 );
            time.seconds( 0 );
            comm.time( time );

            comm.sysName(pair.first);
            control.writeToClock(comm);


            break;
          case 2: // blue is ready, not orange. 
            
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_RED);
            comm.blueGearColor( Colors::COLOR_BLUE ); //off
            comm.orangeGearColor( Colors::COLOR_BLACK); // trun this one on
            time.minutes( 0 );
            time.seconds( 0 );
            comm.time( time );

            comm.sysName(pair.first);
            control.writeToClock(comm);



            break;
          case 3: // both ready

            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_RED);
            comm.blueGearColor( Colors::COLOR_BLUE ); //off
            comm.orangeGearColor( Colors::COLOR_ORANGE); // trun this one on
            time.minutes( 0 );
            time.seconds( 0 );
            comm.time( time );

            comm.sysName(pair.first);
            control.writeToClock(comm);


          /*
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_GREEN);

            time.seconds( fmod(roboClock::clockLengthSec[pair.first], 60) );
            time.minutes( int(roboClock::clockLengthSec[pair.first] / 60) );
            comm.time( time );
            control.writeToClock(comm);

            roboClock::clockTimers[pair.first].start();
            
            roboClock::clockStates[pair.first] = 4; // claock timeer started, and clock inited to top time, transition into the running state. 
            */
            break;
          case 4: // ui for 3? sec countdown

            timeNum = roboClock::clockLengthSec[pair.first] - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_YELLOW);

            time.seconds( int(timeNum) * 11  ); // shoudld go 33::33 -> 22:22 -> 11:11 -. start. 
            time.minutes( int(timeNum) * 11  );
            comm.time( time );
            control.writeToClock(comm);

            if(timeNum <= 0 ){
              roboClock::control.systemStates[pair.first] = 5;
            }


          /*
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
*/
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
            control.writeToClock(comm); // should just work probaly.... need a resume state tho. 
            

            break;
          case 6:
            time.seconds( 88 );
            time.minutes( 88 );

            comm.isOff( 0 );
            comm.blueGearColor( Colors::COLOR_BLUE );
            comm.orangeGearColor( Colors::COLOR_BLUE );
            comm.mainColor( Colors::COLOR_BLUE );

            comm.doDisplayTime( 1 );

            comm.time( time );
            control.writeToClock( comm );

            
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
            time.seconds( 0 );
            time.minutes( 0 );

            comm.isOff( 0 );
            comm.blueGearColor( Colors::COLOR_RED );
            comm.orangeGearColor( Colors::COLOR_RED );
            comm.mainColor( Colors::COLOR_RED );

            comm.doDisplayTime( 1 );

            comm.time( time );
            control.writeToClock( comm );

            break;

          case 9:
            roboClock::clockTimers[pair.first].pause();
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            
            // so duration - clock timer -> get elapsed = remaining time in seconds, then do color check and display. 
            timeNum = roboClock::clockLengthSec[pair.first] - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.

            time.seconds( fmod(timeNum, 60) );
            time.minutes( int(timeNum / 60) );
            comm.time( time );
            control.writeToClock(comm); // should just work probaly.... need a resume state tho. (12)
            break;


          case 10:
            timeNum = 3 - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            comm.mainColor( Colors::COLOR_YELLOW);

            time.seconds( int(timeNum) * 11  ); // shoudld go 33::33 -> 22:22 -> 11:11 -. start. 
            time.minutes( int(timeNum) * 11  );
            comm.time( time );
            control.writeToClock(comm);

            roboClock::clockTimers[pair.first].start();
            roboClock::control.systemStates[pair.first] = 4;


            break;
          case 11:
            
            roboClock::clockTimers[pair.first].start();
            comm.isOff( 0 );
            comm.doDisplayTime( 1 );
            
            // so duration - clock timer -> get elapsed = remaining time in seconds, then do color check and display. 
            timeNum = roboClock::clockLengthSec[pair.first] - (roboClock::clockTimers[pair.first].elapsedMsec() / 1000); // division cause ms.

            time.seconds( fmod(timeNum, 60) );
            time.minutes( int(timeNum / 60) );
            comm.time( time );
            control.writeToClock(comm); // should just work probaly.... need a resume state tho. (12)


            break;
          case 12:
            roboClock::clockTimers[pair.first].resume();
            roboClock::clockStates[pair.first] = 5; // put back into running but with a resumed timer. - can ony beapaused in the 3:00 state (5) 
            control.writeToClock(comm);
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
