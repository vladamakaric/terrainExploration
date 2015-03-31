#pragma once
#include <vector>
#include "Vector.h"
#include "ConsciousEntity.h"

using namespace std;


class CMoveAlongPath
{
public:
	CMoveAlongPath(CConsciousEntity *_CE,vector<CVector> * _path);
	void Update();
	void Reset();


	void CurrentPathNodeHasChanged();
	double GetTraveledLenght();
	double GetLenghtToGoal();

	double lenght;
	vector<CVector> *path;
	vector<double> pathGValues;
	CConsciousEntity* CE;
	int currentPathNodeIndx;
};