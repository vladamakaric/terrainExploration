#include "Follow.h"
#include "AIS_GoToUnseenPosition.h"
#include "f_vector.h"
#include "f_misc.h"
#include "f_distance.h"
#include "draw.h"
#include "FollowNLS.h"
#include "FollowNode.h"
#include "FieldOfView.h"
#include "ShortestPathGenerator.h"


void CFollow::Draw()
{
	glColor3f(1,0,1);
	//CLineSegment ls(EXP->behaviour->CE->positionViewRadius.position, current->position);
	//ls.Draw();
}

void CFollow::UpdatePath( vector<CPathFindingNode*> &_nodePath )
{
	EXP->path.clear();
	EXP->moveAlongPath->lenght = CShortestPathGenerator::pathLenght;

	CShortestPathGenerator::SavePathGValues(_nodePath, EXP->moveAlongPath->pathGValues);
	F::MISC::PathFindingNodePathToVectorPath(_nodePath, EXP->path);
	EXP->moveAlongPath->Reset();
	lastPathFN = dynamic_cast<CFOVNode*>(_nodePath[_nodePath.size()-2]);
}

bool CFollow::CreatePath( vector<CPathFindingNode*> &_nodePath )
{
	EXP->behaviour->CE->UpdateFOVConnections();
	EXP->SetUpTempNodes(this);
	
	if(!EXP->CEPosFN.NLSNeighbours.size() && !EXP->CEPosFN.origNeighbours.size())
		return false;

	CShortestPathGenerator::Generate(&EXP->CEPosFN, &EXP->USPositionFN, _nodePath);
	return true;
}


//bool FOVNVisible;
//
//if(FOV->edgdeFOVNodes[i]->origNLS)
//	FOVNVisible = FOV->NLSFOVNodeVisibleCircleRadiusOffset(FOV->edgdeFOVNodes[i], 1);
//else
//	FOVNVisible = FOV->NodeVisible(FOV->edgdeFOVNodes[i]->origNode);
//
//if(!FOVNVisible)
//	continue;
//
//nodeToGoal.Set(FOV->edgdeFOVNodes[i]->position, EXP->goal);
//
//if(FOV->LineSegmentIntersectsVisibleObstacles(nodeToGoal, FOV->edgdeFOVNodes[i], false))
//	continue;
///////////////////////////