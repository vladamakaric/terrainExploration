#pragma once
#include "Vector.h"
#include <vector>
#include "Capsule.h"


using namespace std;

class CPath
{
public:
	CPath() : lastIsTemp(0), firstPoint(0) {}

	void Draw();
	void RemoveLastAddedPoint();
	bool AddPoint(CVector& _point, bool checkForMinDist = true);

	bool lastIsTemp;
	CVector* firstPoint;
	vector<CCapsule*> capsules;
	vector<CVector> points;
	double r; 
};