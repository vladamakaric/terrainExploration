#pragma once
#include "Vector.h"
#include <vector>

using namespace std;

class CPolygon
{
public:
	CVector* points;
	int pointNum;

	CPolygon(vector<CVector*> *pointsVector, double minAngleDiffToPI);
	CPolygon(int _pNum) : points(new CVector[_pNum]), pointNum(_pNum) {}
	CPolygon(CVector* _p, int _pNum) : points(_p), pointNum(_pNum) {}
	bool PointInside(CVector  &_p);
};