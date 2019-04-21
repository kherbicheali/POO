// maeva_sql.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "MMTimer.h"
#include "libRobotPekee.h"
#include "MyLoopHandlerClass.h"

using namespace std;

//Declarations globales au programme
//

WRobotPekee Pekee;

// -- Commenter en fonction pour selectionner le mode de commande
// Configuration mode Simulation
BUFFER experience="simulation sur le RSL.";
BUFFER szRSLHostName="localhost";

// Configuration mode Reel
//BUFFER experience="commande du robot reel.";
//BUFFER szRSLHostName="130.120.12.101";
class  handler:public CMultimediaTimerCallback{
public:
	int GetTickCount (void);
	void Sleep(unsigned int t );
};


int main(int argc, char* argv[])
{
	
int TT1=0;
int TTf=0;


//MyLoopHandlerClass TT0 (*handler);
//TT0.OnStart();

float VVmax;
	printf("Coucou\n");
	
	cout << "Vmax = " ;
	cin >> VVmax ;
	cout << "t1 = " ;
	cin >> TT1;
	cout << "tf = ";
	cin >> TTf;
	// Connexion au robot
	//
	std::cout << "Essai de connection a " << szRSLHostName << std::endl;
	if(Pekee.Connect(szRSLHostName))
    {
		std::cout << "La connexion est etablie" << std::endl;


	  // --------------------------------- Initialisation du Timer
		int TT0=GetTickCount();
		MyLoopHandlerClass *handler = new MyLoopHandlerClass (TT0,TT1,TTf,VVmax);
		CMultimediaTimer timer(*handler);

		timer.Start(20,1);

	  // --------------------------------- Tracage de la Position

		while (!kbhit()) {Sleep(1); Pekee.SetSpeed(100,0) ;}
 
	  // --------------------------------- Arret du Timer
		
		timer.Stop();

	  // Arret de la connexion au Robot

		Pekee.Disconnect();
    }
	else
    {
		std::cout << "La connexion n'a pu etre etablie." << std::endl;
    }
	return 0;
}
