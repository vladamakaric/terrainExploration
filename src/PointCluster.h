#pragma once
#include <vector>
#include "Vector.h"

using namespace std;

class CPointCluster
{
public:
	vector<CVector*> *points;
	CPointCluster *next;
	CPointCluster *previous;
	CVector position;
	int hRadius;

	static int globalID;
	int ID;

	CPointCluster(const CVector &_position,int _HRadius, int _pointNum, int _minDist);
	bool IsMinDistance(const CVector &v, int _minDist, int _vertNum);
	bool IsInvalid();
	void Draw();
};