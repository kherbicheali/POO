#ifndef	_MMTIMER_
#define	_MMTIMER_

#ifndef _INC_MMSYSTEM
#include	<mmsystem.h>  //access to multimedia timers
#endif

#ifndef	_FLCRITICALSECTION_
#include "FLCriticalSection.h"
#endif

////////////////////////////////////////////////////////////////
//Valid object marker used to check object is valid on loop call (Paranoia check)
const	DWORD	cMMTimerMarker=0xF5F5F5F5;	
//Set default resolution to 1ms. Note this is the best possible resolution
//should use larger values if accuracy not needed as does add some overhead.
const UINT	cDefaultTimerResolution=1;	

////////////////////////////////////////////////////////////////////////////////////
// CMultimediaTimerCallback  -  The callback support interface
// We seperate loop handling from the Timer class to prevent calls from 
// within OnLoop() accidently calling CMultimediaTimer methods which could cause 
// instability due to a race condition.
////////////////////////////////////////////////////////////////////////////////////
class CMultimediaTimerCallback
{
public:
	CMultimediaTimerCallback(){}
	virtual ~CMultimediaTimerCallback(){}

public:
	virtual bool	OnLoop()=0;				//Must Override timer will stop if return false
	virtual void	OnStarted(){}			//Overrideable called when timer started sucessfully
	virtual void	OnStopped(){}			//Overrideable called after timer stopped
};

////////////////////////////////////////////////////////////////////////////////////
// CMultimediaTimer  -  A simple periodic timer
// To use: 
//	Derive a loop class from CMultimediaTimerCallback and override OnLoop().
//	When Start() is called successfully OnLoop() will be called every nPeriodMs.
//
//	NB. OnLoop() is being called in a seperate thread created by the Multimedia
//	timer library. Thus all Multithreading issues apply.
//	You should NOT call Start() or Stop() from the OnLoop() method
//	If the timer needs to stop itself just return false from OnLoop().
////////////////////////////////////////////////////////////////////////////////////
class CMultimediaTimer
{
public:
	CMultimediaTimer(CMultimediaTimerCallback& callback);
	virtual ~CMultimediaTimer();

	//Start the periodic timer, OnLoop() will be called every nPeriodMs milliseconds
	//Start() returns false if failed and nError returns the error code
	bool						Start(UINT nPeriodMs,MMRESULT& nError,UINT nResolutionMs=cDefaultTimerResolution);
	//Same as above but no error code returned
	bool						Start(UINT nPeriodMs,UINT nResolutionMs=cDefaultTimerResolution)
									{MMRESULT nErr; return Start(nPeriodMs,nErr,nResolutionMs);}
	//Stop the timer
	void						Stop();
	//Is the timer running
	bool						Active()const					{return m_TimerId!=0;}
	//Period set while running
	UINT						GetPeriodMs()const		{return m_TimerPeriod;}
	//Resolution set while running
	UINT						GetResolutionMs()const{return m_TimerResolution;}

private:
	bool						CheckMarker()const;
	void						ResetPeriodResolution();
	void						DoLoop();

private:
	CMultimediaTimerCallback& m_Callback;						//The loop callback
	UINT											m_TimerPeriod;				//Timer period in milliseconds
	UINT											m_TimerResolution;		//Timer resolution in milliseconds
	UINT											m_TimerId;						//ID of timer thread
	bool											m_EndTimer;						//Flag to force timer to stop from loop
	FLCriticalSection					m_StopCriticalSection;//section to protect multi access to Stop()

private:
	DWORD											m_Marker;							//Paranoia check
	friend void CALLBACK			mmTimerProc(UINT,UINT,DWORD,DWORD,DWORD);//Callback
};
 
////////////////////////////////////////////////////////////////

#endif
