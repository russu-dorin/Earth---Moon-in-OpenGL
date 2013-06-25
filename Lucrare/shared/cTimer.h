#pragma once
#ifndef CTIMER_H
#define CTIMER_H

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

//this class implements the timer mechanism
//example of usage:
// create an object: cTimer timer;
// set the timer to 0: timer.Reset();
//perform some actions: PerformCustomActions();
// update the time: timer.AdvanceTime();
//get the the time spent; float timeSpent=timer.GetElapsedTime();

class cTimer
{
  int alias;
  //this is the size of the registryx
  static const int registrySize=10000;
  //this is the registry in which we keep track for all alive timers
  static cTimer* timerRegistry[registrySize];
  //a flag used when to initialize the registry, when an object of cTimer class is created first time
  static bool registryInitialized;
  //this is the index of current cTimer object in the registry
  int myRegistryIndex;
  //this is the time elapsed during calling of AdvanceTime()
  float elapsedTime;
  //this is delay between AdvanceTime cal
  float delay;
  //interal stuff data
  double prevTime;
  //the is the frequence data, as it is said in the documentation, the frequency cant change while the sys is ruinnig
  //thus we need only a single copy for all the timers
#ifdef WIN32
  static LARGE_INTEGER frequency;
  //and this is the low resolutions timer data
  LARGE_INTEGER counter;
#else
  timeval counter;
#endif
  ////enables pausing
  bool enablePause;
  //this is the accelerations value
  float acceleration;
  //freeze value for time, internal usage only
  float freezeValue;


private:
  //initializes base data
  void InitBase();
  //register itself in the registry
  void RegisterMe();
  //this is used to reset the registry
  static void ResetRegistry();

public:
  //this is the first free registry slot available for a new timer
  static int firstFreeRegistryEntry;
  //used for whatever
  float tag;

public:
  //the copy constructor
  cTimer(const cTimer &t);
  //constructor with alias
  cTimer(int aliasIn);
  //constructor
  cTimer(void);
  //destructor
  ~cTimer(void);
  //advances time;
  void AdvanceTime();
  //in reverse direction
  void AdvanceTimeNegative();
  //set the timer to 0
  void Reset();
  //returns the elapsed time
  float GetElapsedTime(){return elapsedTime;}
  //returns the time between two AdvanceTimer() calls
  float GetDelay(){return delay;}
  //sets the elapsed time. Can be used in conjunction with AdvanceTimeNegative() as a countdown timer
  void SetElapsedTime(float value){elapsedTime=value;}
  //pasues the timer
  void Pause(bool enable);
  //return true if the timer is paused, false otherwise
  bool IsPaused();
  //usedto accelerate all the timer
  void AccelerateTimer(float accelerateValueIn);
  //returns the acceleration value
  float GetAccelerationValue();
  //--------------------------------------------------------------------------------------------------//
  //The following methods should not be used for game programming. They are used to tweak the engine.
  //Each such method is marked with ENGINEONLY_
  //used to set syncronize each timer after they were inactive as result of lost of window focus or some other win32 moment
  static void ENGINEONLY_SyncronizeAllTimers();
  //freeze a timer
  void ENGINEONLY_FreezeTimer(bool enableFreeze);
  //freezes all timers
  static void ENGINEONLY_FreezeAllTimers(bool enableFreeze, int lowerAliasRangeIn, int upperAliasRangeIn);
  //returns true if most of timers are frozen
  static bool ENGINEONLY_AreTimersFrozen();
  //--------------------------------------------------------------------------------------------------//
};

extern cTimer _emitterTimer;
extern cTimer _deftimer;

#endif
