#include ".\ctimer.h"

//the registry
cTimer* cTimer::timerRegistry[cTimer::registrySize];
//the first free slot
int cTimer::firstFreeRegistryEntry=0;
//are we initialized
bool cTimer::registryInitialized=false;
//the frequence of the sys low res timer
#ifdef WIN32
LARGE_INTEGER cTimer::frequency;
#endif

cTimer::cTimer(const cTimer &t)
{
  this->InitBase();
  alias=t.alias;
}
cTimer::cTimer(int aliasIn)
{
  this->InitBase();
  alias=aliasIn;
}
cTimer::cTimer(void)
{
  this->InitBase();
}
//initializes base data
void cTimer::InitBase()
{
  alias=0;
  freezeValue=1;
  elapsedTime=0.0f;
  delay=0.0f;
#ifdef WIN32
  QueryPerformanceCounter(&counter);
  prevTime=counter.QuadPart;
#else
  gettimeofday(&counter, NULL);
  prevTime = counter.tv_sec + counter.tv_usec / 1000000.0;
#endif
  enablePause=false;
  acceleration=1.0f;
  myRegistryIndex=-1;

  if(false==registryInitialized)
  {
    cTimer::ResetRegistry();
    registryInitialized=true;
#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
#endif
  }

  this->RegisterMe();
}
//register itself in the registry
void cTimer::RegisterMe()
{
  //registring the timer in the timer registry list
  //we are full, the static size must be expanded
  if(firstFreeRegistryEntry>=registrySize-1)
  {
    //assert0;
  }
  else
  {
    //an error occured, this place should be free, or we didn't zeromemory the timer registry at app init
    if(NULL!=timerRegistry[firstFreeRegistryEntry])
    {
      //assert0;
    }
    else
    {
      //add me to the registry
      timerRegistry[firstFreeRegistryEntry]=this;

      //save my registry index
      myRegistryIndex=firstFreeRegistryEntry;

      //update the first free entry
      firstFreeRegistryEntry++;
    }
  }
}
cTimer::~cTimer(void)
{
  //unregister the timer
  //did we ever register?
  if(myRegistryIndex>=0)
  {
    //perform exchange with the last entry in registry and with current entry being freed, to avoid fragmentation
    if(firstFreeRegistryEntry>0)
    {
      timerRegistry[myRegistryIndex]=timerRegistry[firstFreeRegistryEntry-1];
      timerRegistry[myRegistryIndex]->myRegistryIndex=myRegistryIndex;
      timerRegistry[firstFreeRegistryEntry-1]=NULL;
      firstFreeRegistryEntry--;
    }

  }
  else
  {
    //we didn't ever register? what the hack, this is an error
    //assert0;
  }
}
void cTimer::ResetRegistry()
{
  for(int i=0;i<registrySize;i++)
  {
    timerRegistry[i]=NULL;
  }
  firstFreeRegistryEntry=0;
}
void cTimer::ENGINEONLY_SyncronizeAllTimers()
{
#ifdef WIN32
  static LARGE_INTEGER tmpCounter;
  QueryPerformanceCounter(&tmpCounter);
#else
  timeval tmpCounter;
  gettimeofday(&tmpCounter, NULL);
#endif

  for(int i=0;i<firstFreeRegistryEntry;i++)
  {
    timerRegistry[i]->counter=tmpCounter;
#ifdef WIN32
    timerRegistry[i]->prevTime=tmpCounter.QuadPart;
#else
    timerRegistry[i]->prevTime = tmpCounter.tv_sec + tmpCounter.tv_usec / 1000000.0;
#endif
  }
}
void cTimer::AdvanceTime()
{
#ifdef WIN32
  QueryPerformanceCounter(&counter);
  delay=(float)(counter.QuadPart-prevTime)/(float)frequency.QuadPart;
#else
  gettimeofday(&counter, NULL);
  delay=(float)((counter.tv_sec + counter.tv_usec / 1000000.0) - prevTime);
#endif

  delay*=acceleration*freezeValue;

  if(enablePause==false)
  {
    elapsedTime+=delay;
  }
  else
  {
    delay=0.0f;
  }
#ifdef WIN32
  prevTime=counter.QuadPart;
#else
  prevTime = counter.tv_sec + counter.tv_usec / 1000000.0;
#endif
}
void cTimer::AdvanceTimeNegative()
{
#ifdef WIN32
  QueryPerformanceCounter(&counter);
  delay=(float)(counter.QuadPart-prevTime)/(float)frequency.QuadPart;
#else
  gettimeofday(&counter, NULL);
  delay=(float)((counter.tv_sec + counter.tv_usec / 1000000.0) - prevTime);
#endif

  delay*=acceleration*freezeValue;

  if(enablePause==false)
  {
    elapsedTime-=delay;
  }
  else
  {
    delay=0.0f;
  }

#ifdef WIN32
  prevTime=counter.QuadPart;
#else
  prevTime = counter.tv_sec + counter.tv_usec / 1000000.0;
#endif
}
void cTimer::Reset()
{
  elapsedTime=0.0f;
  delay=0.0f;
#ifdef WIN32
  QueryPerformanceCounter(&counter);
  prevTime=counter.QuadPart;
#else
  gettimeofday(&counter, NULL);
  prevTime = counter.tv_sec + counter.tv_usec / 1000000.0;
#endif
}
void cTimer::Pause(bool enable)
{
  enablePause=enable;
  if(enablePause==true)
  {
#ifdef WIN32
    LARGE_INTEGER tmpCounter;
    QueryPerformanceCounter(&tmpCounter);
    this->prevTime=tmpCounter.QuadPart;
#else
    timeval tmpCounter;
    gettimeofday(&tmpCounter, NULL);
    this->prevTime = counter.tv_sec + counter.tv_usec / 1000000.0;
#endif
    this->counter=tmpCounter;
  }
}
bool cTimer::IsPaused()
{
  return enablePause;
}
void cTimer::ENGINEONLY_FreezeTimer(bool enableFreeze)
{
  if(enableFreeze)
  {
    this->freezeValue=0;
  }
  else
  {
    this->freezeValue=1;
  }
}
//returns true if most of timers are frozen
bool cTimer::ENGINEONLY_AreTimersFrozen()
{
  int frozenTimersCount=0;

  for(int i=0;i<firstFreeRegistryEntry;i++)
  {
    if(timerRegistry[i]->freezeValue<=0.00001)
    {
      frozenTimersCount++;
    }
  }

  if((float)frozenTimersCount/(float)firstFreeRegistryEntry>0.9f)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void cTimer::ENGINEONLY_FreezeAllTimers(bool enableFreeze, int lowerAliasRangeIn, int upperAliasRangeIn)
{
  if(enableFreeze)
  {
    for(int i=0;i<firstFreeRegistryEntry;i++)
    {
      if((timerRegistry[i]->alias>=lowerAliasRangeIn)&&(timerRegistry[i]->alias<=upperAliasRangeIn))
      {
        timerRegistry[i]->freezeValue=0;
      }
    }
  }
  else
  {
    for(int i=0;i<firstFreeRegistryEntry;i++)
    {
      if((timerRegistry[i]->alias>=lowerAliasRangeIn)&&(timerRegistry[i]->alias<=upperAliasRangeIn))
      {
        timerRegistry[i]->freezeValue=1;
      }
    }
  }
}
void cTimer::AccelerateTimer(float accelerateValueIn)
{
  this->acceleration=accelerateValueIn;
}
//returns the acceleration value
float cTimer::GetAccelerationValue()
{
  return this->acceleration;
}

cTimer _emitterTimer;
cTimer _deftimer;
