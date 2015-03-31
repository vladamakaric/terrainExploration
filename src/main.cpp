#include "Simulation.h"
#include "global.h"
#include <iostream>
#include "functions.h"
#include <stdlib.h>
int main( int argc, char *argv[] )
{
	srand( (unsigned int)(time(NULL)) );
	
	CSimulation::CreateSimulation(20000,20000);
	CSimulation::Instance()->Run();

	SDL_Quit();
	return 0;
}

