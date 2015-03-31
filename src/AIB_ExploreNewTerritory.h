#pragma once
#include "AIBehaviour.h"



class CAIB_ExploreNewTerritory : public CAIBehaviour
{
public:

	CAIB_ExploreNewTerritory(CConsciousEntity *_CE);
	CVector goal;


	void Run();
	void Draw();
};


