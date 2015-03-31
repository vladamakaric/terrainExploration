#include "AIB_ExploreNewTerritory.h"
#include "AIS_GoToUnseenPosition.h"
#include "f_global.h"

CAIB_ExploreNewTerritory::CAIB_ExploreNewTerritory(CConsciousEntity *_CE)
{
	CE = _CE;
	currentState = 0;
}

void CAIB_ExploreNewTerritory::Run()
{
	if(!F::GLOBAL::globalGoal)
	{
		

		CE->velocity = CVector(0);
		return;
	}

	goal = *F::GLOBAL::globalGoal;

	if(!currentState)
		currentState = new CAIS_GoToUnseenPosition(CVector(1,1).Direction(),this);

	while(1)
	{
		if(currentState->Run())
			break;

		delete currentState;
		currentState = newState;
	};
}

void CAIB_ExploreNewTerritory::Draw()
{
	if(currentState)
	currentState->Draw();
}

