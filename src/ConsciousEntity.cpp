#include "ConsciousEntity.h"
#include "simulationDefinitions.h"
#include "functions.h"

#include "draw.h"
#include "global.h"
#include <iostream>

#include "AIB_ExploreNewTerritory.h"


#include "LineSegmentVisibility.h"
#include "PrecompiledShapes.h"

using namespace std;

CConsciousEntity::CConsciousEntity( CVector _p, double _horizontHRad ) :
positionViewRadius(_p,_horizontHRad)
{
	FOV = new CFieldOfView();
	behaviour = new CAIB_ExploreNewTerritory(this);
	speed = 30;
	traveledPath.push_back(positionViewRadius.position);
	//currentPath.points.push_back(positionViewRadius.position);
	paths.push_back(&currentPath);
}

void CConsciousEntity::Update()
{

	//glDisable(GL_STENCIL_BUFFER_BIT);

	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	
	for(int i=0; i<traveledPath.size(); i++)
		CPCShapes::DrawShapeScaled(CPCShapes::circle100x100, positionViewRadius.r/100, traveledPath[i]);
	
	//for(int i=0; i<currentPath.capsules.size(); i++)
	//	currentPath.capsules[i]->DrawFill();

	//Draw::VectorObjectContainerAsConnectedFilledCapsules(currentPath.points,  currentPath.r);
	//

	changeVelocity = false;
	FOVUpdated = false;
	FOVConnectionsUpdated = false;

	prevVelocity = velocity;
	positionViewRadius.r = F::GLOBAL::FOVCIRCLE;
	currentPath.r = positionViewRadius.r+20;

	if(keystates[SDLK_q])
	{
		speed++;
	}

	if(keystates[SDLK_a])
	{
		speed--;
		
	}

	if(speed <= 0)
		speed = 1;

	if(keystates[SDLK_KP_PLUS])
	{
		F::GLOBAL::FOVCIRCLE+=speed;
		positionViewRadius.r = F::GLOBAL::FOVCIRCLE;
		UpdateFOV();
	}

	if(keystates[SDLK_KP_MINUS])
	{
		F::GLOBAL::FOVCIRCLE-=speed;
		positionViewRadius.r = F::GLOBAL::FOVCIRCLE;
		UpdateFOV();
	}

	positionViewRadius.r = F::GLOBAL::FOVCIRCLE;

	currentPath.lastIsTemp = true;
	currentPath.AddPoint(positionViewRadius.position, false);
	behaviour->Run();
	currentPath.RemoveLastAddedPoint();
	currentPath.lastIsTemp = false;

	if(velocity.LengthSq())
	traveledPath.push_back(positionViewRadius.position);

	if(changeVelocity)
		currentPath.AddPoint(positionViewRadius.position);

	positionViewRadius.position += velocity*speed;

	if(keystates[SDLK_p])
		FOV->UpdateFOVNodesConnections(paths);
}

void CConsciousEntity::Draw()
{
	glColor3f(0,0,1);
	Draw::CircleOutline(positionViewRadius.position, 40, 5);
	behaviour->Draw();
	
	glLineWidth(2);
	glColor3f(1,1,0);
	Draw::CircleOutline(positionViewRadius.position, positionViewRadius.r, 45);
	
	//glColor4f(0,0,1,0.005);

	//Draw::VectorObjectContainerAsConnectedOutlinedCapsules(currentPath.points,  currentPath.r);
	
	FOV->Draw();

	/*
	CLineSegment lst(F::GLOBAL::GlobA, F::GLOBAL::GlobB);

	if(!CLineSegmentVisibility::Test(lst, paths))
	{
		CLineSegmentVisibility::Draw();
		glColor4f(1,0,0,0.5);
		Draw::CircleFill(CVector(5000,5000), 500, 3);
	}

	lst.Draw();*/
}

CConsciousEntity::~CConsciousEntity()
{
	delete FOV;
}

void CConsciousEntity::UpdateFOV()
{
	if(!FOVUpdated)
	{
		FOV->Update(&positionViewRadius);
		FOVUpdated = true;
	}
}

void CConsciousEntity::UpdateFOVConnections()
{
	if(!FOVConnectionsUpdated)
		FOV->UpdateFOVNodesConnections(paths);
}
