#include "stdafx.h"

#ifndef __REALTIME

#ifndef	_MMTIMER_
#include "MMTimer.h"
#endif

//Allow non MFC asserting
#ifdef _AFX
	#define MM_ASSERT			ASSERT
#else
	#include <crtdbg.h>
	#define MM_ASSERT			_ASSERT
#endif

////////////////////////////////////////////////////////////////
//CMultimediaTimer support
////////////////////////////////////////////////////////////////
void CALLBACK mmTimerProc(UINT id,UINT msg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CMultimediaTimer*	pTimer = (CMultimediaTimer*)dwUser;
	bool bValidTimer=pTimer!=NULL && pTimer->CheckMarker();
	MM_ASSERT(bValidTimer);

	//Don't call callback unless a valid CMultimediaTimer object 
	if(bValidTimer)
		pTimer->DoLoop();
}

////////////////////////////////////////////////////////////////
// CMultimediaTimer
////////////////////////////////////////////////////////////////
CMultimediaTimer::CMultimediaTimer(CMultimediaTimerCallback& callback) 
:m_Callback(callback),m_Marker(cMMTimerMarker),
m_TimerResolution(0),m_TimerPeriod(0),m_TimerId(0),m_EndTimer(false)
{
}

///////////////////////////////////////////////////////////////////////////////////
//
CMultimediaTimer::~CMultimediaTimer()
{
	Stop();
	m_Marker=0;//invalidate marker
}

///////////////////////////////////////////////////////////////////////////////////
//
void CMultimediaTimer::DoLoop()
{
	if(m_EndTimer)//Previous loop marked as ending
		{
		Stop();//Try to stop
		}

	// Check Active() is true - before calling Loop()
	// This may be false if the overridden OnStarted() takes longer than 1 period
	// and OnStarted() must complete before first loop call.
	//
	if(!m_EndTimer && Active())
		{
		m_EndTimer=!m_Callback.OnLoop();
		}
}

///////////////////////////////////////////////////////////////////////////////////
//
bool	CMultimediaTimer::CheckMarker()const
{
	return m_Marker==cMMTimerMarker;
}

////////////////////////////////////////////////////////////////
//
bool CMultimediaTimer::Start(UINT nPeriodMs,MMRESULT& nError,UINT nResolutionMs)
{
	nError=TIMERR_NOERROR;

	//1. Clean up any current timer
	Stop();

	//2 Get available timer resolution
	TIMECAPS	tc;
	MMRESULT result=::timeGetDevCaps(&tc,sizeof(TIMECAPS));
	if(result!=TIMERR_NOERROR)
		{
		nError=result;//Should be TIMERR_STRUCT
		return false;
		}
	MM_ASSERT(nPeriodMs>=tc.wPeriodMin && nPeriodMs<=tc.wPeriodMax);
	MM_ASSERT(nResolutionMs>=tc.wPeriodMin && nResolutionMs<=tc.wPeriodMax);

	//Clip required resolution to available resolution
	m_TimerResolution=min(max(tc.wPeriodMin,nResolutionMs),tc.wPeriodMax);
	
	//Set global min resolution value
	result=::timeBeginPeriod(m_TimerResolution);
	if(result!=TIMERR_NOERROR)
		{
		m_TimerResolution=0;
		nError=result;//Should be TIMERR_NOCANDO
		return false;
		}

	//Clip required period to available resolution
	m_TimerPeriod=min(max(tc.wPeriodMin,nPeriodMs),tc.wPeriodMax);

	//Set event callback passing this as user data
	result=::timeSetEvent(m_TimerPeriod,m_TimerResolution,
												mmTimerProc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
	if(result!=0)
		{
		m_Callback.OnStarted();	//Success so call override
		m_TimerId=result;
		}
	else	//Failed
		{
		ResetPeriodResolution();	//Need to reset call to timeBeginPeriod()
		nError=TIMERR_NOCANDO;	//Use standard err code to indicate error
		}
	return Active();

}

///////////////////////////////////////////////////////////////////////////////////
//Stop() is protected with a critical section as can be called from the Loop() thread
//as well as via the public method externally.
//
void	CMultimediaTimer::Stop()
{
	FLCriticalSectionSetUp lock(m_StopCriticalSection);
	if(Active())
		{
		UINT nTimerID=m_TimerId;
		m_TimerId=0;		//Force inactive
		MMRESULT result=::timeKillEvent(nTimerID);
		MM_ASSERT(result==TIMERR_NOERROR);
		m_Callback.OnStopped();
		m_EndTimer=false;
		}
	ResetPeriodResolution();
}

///////////////////////////////////////////////////////////////////////////////////
//
void	CMultimediaTimer::ResetPeriodResolution()
{
	if(m_TimerResolution!=0)
		::timeEndPeriod(m_TimerResolution);//match the call in Start() to timeBeginPeriod()
	m_TimerResolution=0;
	m_TimerPeriod=0;
}

///////////////////////////////////////////////////////////////////////////////////
#endif