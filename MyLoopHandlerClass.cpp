#include "stdafx.h"
#include "MyLoopHandlerClass.h"
#include "MMTimer.h"
//#include <chrono>
//#include <boost/thread/thread.hpp>
using namespace std;

MyLoopHandlerClass::MyLoopHandlerClass(int t0,int t1,int tf, float vmax) : out("trajectoire.txt")
{
	this -> T1=t1; //ms
	this -> Tf=tf; //ms
	this -> Vmax=vmax; // Vmax = 0.1 cm/ms
	this -> T0=t0;
	
}

MyLoopHandlerClass::~MyLoopHandlerClass(void)
{
}

 bool MyLoopHandlerClass:: OnLoop (void)
 {	
	int t = GetTickCount()-T0-20;
	double D,V; // distance et Vitesse 
	
	if (t <= T1)
	{
		D =  0.5*Vmax*t*t/T1; // cm
		V= t*Vmax/T1;  
	}
	if((t>T1) && (t<=Tf-T1))
	{
		D=  0.5*Vmax*T1+Vmax*(t-T1); // cm
		V=Vmax;
	} 
	if(t>Tf-T1 && t<=Tf)
	{
		//D =  0.5*Vmax*T1+Vmax*(Tf-T1-T1)+0.5*Vmax/(Tf-(Tf-T1))*(t-(Tf-T1))*(t-(Tf-T1));//cm 
		D =   0.5*Vmax*T1+Vmax*(Tf-T1-T1)+ 0.5*Vmax/T1*((Tf-T1)*(Tf-T1)-t*t) + Vmax*Tf/T1*(t-(Tf-T1));//cm 
		V=-Vmax+((Vmax/(Tf-T1-Tf))*(t-Tf-T1));
		
	}
	cout << t << " ms\t" << V << " cm/s\t"<< D <<"cm"<< std::endl;
	out << t << "\t" << D << "\t" <<V << std::endl;

	return true;
}

 void MyLoopHandlerClass:: OnStopped ()
 {
	 out.close();
 }
