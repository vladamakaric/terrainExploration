#include "MoveAlongPath.h"
#include "f_vector.h"
#include "f_distance.h"
#include "simulationDefinitions.h"

CMoveAlongPath::CMoveAlongPath(CConsciousEntity *_CE,vector<CVector> * _path) : 
CE(_CE), 
path(_path),
lenght(0)
{

}

void CMoveAlongPath::Update()
{
	if(currentPathNodeIndx == path->size()-1)
		return;

	if(F::DISTANCE::DistBetweenVecsBiggerThan(CE->positionViewRadius.position, (*path)[currentPathNodeIndx], CE->speed))
		return;

	bool visible = false;

	CLineSegment ls(CE->positionViewRadius.position, (*path)[currentPathNodeIndx+1]);
	visible = !U_FAG->LineSegmentCollisionWithObstacles_ForbiddenLSPoints(ls);

	if(!visible)
	{
		CVector toCurrentPathNodeFromCE = F::VECTOR::GetVectorFromAToB(CE->positionViewRadius.position, (*path)[currentPathNodeIndx]);
		CVector toCurrentPathNodeFromPreviousPathNode = F::VECTOR::GetVectorFromAToB((*path)[currentPathNodeIndx-1], (*path)[currentPathNodeIndx]);

		if(toCurrentPathNodeFromCE * toCurrentPathNodeFromPreviousPathNode < 0)
			visible = true;

		if(!visible)
			visible = U_FAG->PointInObstacle(CE->positionViewRadius.position + CE->velocity*CE->speed);
	}

	if(visible)
	{
		currentPathNodeIndx++;
		CE->changeVelocity = true;
		CE->velocity = F::VECTOR::GetVectorFromAToB(CE->positionViewRadius.position, (*path)[currentPathNodeIndx]).Direction();
	}
}

void CMoveAlongPath::Reset()
{
	currentPathNodeIndx = 1;
	CE->changeVelocity = true;
	CE->velocity = F::VECTOR::GetVectorFromAToB(CE->positionViewRadius.position, (*path)[currentPathNodeIndx]).Direction();
}

double CMoveAlongPath::GetTraveledLenght()
{
	return pathGValues[currentPathNodeIndx-1] + F::DISTANCE::GetDistance(CE->positionViewRadius.position, (*path)[currentPathNodeIndx-1]);
}

double CMoveAlongPath::GetLenghtToGoal()
{
	return lenght - GetTraveledLenght();
}

void CMoveAlongPath::CurrentPathNodeHasChanged()
{
	CE->changeVelocity = true;
	CE->velocity = F::VECTOR::GetVectorFromAToB(CE->positionViewRadius.position, (*path)[currentPathNodeIndx]).Direction();
}
