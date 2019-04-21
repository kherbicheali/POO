#pragma once
#include "mmtimer.h"
#include <math.h>
#include <iostream>
#include <fstream>

class MyLoopHandlerClass :
	public CMultimediaTimerCallback
{
private: 
	int T1,Tf,T0;
	float Vmax;
	std::ofstream out;
public:
	MyLoopHandlerClass(int,int,int,float);
public:
	virtual ~MyLoopHandlerClass(void);
public:
	bool OnLoop();
	void OnStart();
	void Stopped ();
	void OnStopped ();
};
//int MyLoopHandlerClass *T0