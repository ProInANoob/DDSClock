/*****************************************************************
 * 
 *****************************************************************
 *
 *   Copyright(C) 2024      Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************/

#ifndef TIMER_HH
#define TIMER_HH

#include <chrono>

class Timer
{
  
  std::chrono::steady_clock::time_point _start; //  = std::chrono::steady_clock::now();
  bool _running;
  
public:

  typedef std::chrono::steady_clock::time_point Timestamp;
  
  Timer() : _start( std::chrono::steady_clock::now()), _running(false) { }
  
  void start() { _start = std::chrono::steady_clock::now(); _running = true; }
  void stop()  { _running = false; }
  
  bool isRunning() const { return _running; }
  
  std::chrono::milliseconds elapsed() const {
    std::chrono::steady_clock::time_point _end;
    _end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start); 
  }
  
  int64_t elapsedMsec() const {
    return elapsed().count();
  }
};


#endif
