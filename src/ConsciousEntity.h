#pragma once

#include "Vector.h"
#include "Node.h"
#include "NodeLineSegment.h"
#include "FieldOfView.h"
#include "Circle.h"
#include <vector>
#include "Path.h"

using namespace std;

class CAIBehaviour;
class CConsciousEntity
{
public:
	CAIBehaviour *behaviour;
	CFieldOfView *FOV;
	CCircle positionViewRadius;
	CVector prevVelocity;
	CVector velocity;
	bool changeVelocity;
	double speed;
	vector<CVector> traveledPath;
	vector<CPath*> paths;
	CPath currentPath;
	bool FOVUpdated;
	bool FOVConnectionsUpdated;

	CConsciousEntity( CVector _p, double _horizontHRad );
	~CConsciousEntity();
	void UpdateFOV();
	void UpdateFOVConnections();
	void Update();
	void Draw();
};

#include "AIBehaviour.h"