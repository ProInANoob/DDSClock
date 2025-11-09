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
#include <stdio.h>

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



class NewTimer {
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;

    TimePoint _start;
    Duration _elapsed;
    
public:

    bool _running;

    NewTimer() : _start(Clock::now()), _elapsed(Duration::zero()), _running(false) {}

    void start() {
        printf("start a timer --------------\n");
        _start = Clock::now();
        _elapsed = Duration::zero();
        _running = true;
    }

    void stop() {
        if (_running) {
            _elapsed += std::chrono::duration_cast<Duration>(Clock::now() - _start);
            _running = false;
        }
    }

    void pause() {
        if (_running) {
            _elapsed += std::chrono::duration_cast<Duration>(Clock::now() - _start);
            _running = false;
        }
    }

    void resume() {
        if (!_running) {
            _start = Clock::now();
            _running = true;
        }
    }

    bool isRunning() const { return _running; }

    Duration elapsed() const {
        
        if (_running) {
            std::chrono::steady_clock::time_point _end;
            _end = std::chrono::steady_clock::now();
             
            return _elapsed + std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
        } else {
            return _elapsed;
        }
    }

    int64_t elapsedMsec() const {
        return elapsed().count();
    }
};


#endif