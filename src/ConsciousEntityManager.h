#pragma once

#include "ConsciousEntity.h"
#include <list>

using namespace std;

class CConsciousEntityManager
{
public:
	CConsciousEntityManager();
	~CConsciousEntityManager();
	list<CConsciousEntity*> consciousEntities; 
	void Update();
	void Draw();
};