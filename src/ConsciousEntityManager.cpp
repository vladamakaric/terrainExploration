#include "ConsciousEntityManager.h"
#include "Simulation.h"
#include "f_global.h"

CConsciousEntityManager::CConsciousEntityManager()
{
	//consciousEntities.push_back(new CConsciousEntity(CVector(0, 0), F::CONSTANTS::FOVCIRCLE));
}

void CConsciousEntityManager::Update()
{
	list<CConsciousEntity*>::iterator iter = consciousEntities.begin();
	while(iter!=consciousEntities.end())
	{
		(*iter)->Update();
		iter++;
	}
}

void CConsciousEntityManager::Draw()
{
	list<CConsciousEntity*>::iterator iter = consciousEntities.begin();
	while(iter!=consciousEntities.end())
	{
		(*iter)->Draw();
		iter++;
	}
}

CConsciousEntityManager::~CConsciousEntityManager()
{
	list<CConsciousEntity*>::iterator iter = consciousEntities.begin();
	while(iter!=consciousEntities.end())
	{
		delete (*iter);
		iter++;
	}
}
