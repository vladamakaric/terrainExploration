#include "FollowNode.h"
#include "f_misc.h"
#include "f_vector.h"
#include "f_node.h"
#include "f_distance.h"
#include "AIS_GoToUnseenPosition.h"
#include "draw.h"

#include "ShortestPathGenerator.h"
#include <math.h>

bool CFollowNode::FollowHasChanged()
{
	return lastPathFN->origNLS;
}

CFollowNode::CFollowNode( CFOVNode* _FNode, CAIS_GoToUnseenPosition* _EXP )
{
	EXP= _EXP;
	lastPathFN=_FNode;
	updateCount = 0;
}

void CFollowNode::Update()
{
	nodePath.clear();

	CFieldOfView* FOV = EXP->behaviour->CE->FOV;

	bool origNodeConnectionGood = EXP->USPNManager.OirgNodeConnectionGood(lastPathFN);

	if(!FOV->altered)
	if(origNodeConnectionGood)
		return;

	if(origNodeConnectionGood)
	if(!NewNodesCouldMakeBetterPath())
		return;

	GenerateNewPath();
}

void CFollowNode::GenerateNewPath()
{
	CreatePath(nodePath);

	if(CShortestPathGenerator::pathLenght*1.1 < EXP->moveAlongPath->GetLenghtToGoal() || !EXP->USPNManager.OirgNodeConnectionGood(lastPathFN))
		UpdatePath(nodePath);

	CShortestPathGenerator::Clean();
	EXP->ClearPathGenerationVariables();
}

bool CFollowNode::NewNodesCouldMakeBetterPath()
{
	CFieldOfView *FOV = EXP->behaviour->CE->FOV;

	double testLenght;

	int origFOVNodesSize = FOV->newCurrentlyVisibleOrigFOVNodes.size();
	int allFOVNodesSize = origFOVNodesSize + FOV->newCurrentlyVisibleNLSFOVNodes.size();

	CFOVNode* current;

	for(int i=0; i<allFOVNodesSize; i++)
	{
		if(i<origFOVNodesSize)
			current = FOV->newCurrentlyVisibleOrigFOVNodes[i];
		else
			current = FOV->newCurrentlyVisibleNLSFOVNodes[i-origFOVNodesSize];

		testLenght = F::DISTANCE::GetDistance(current->position, EXP->behaviour->CE->positionViewRadius.position) + 
			F::DISTANCE::GetDistance(EXP->USPosition, current->position);

		if(testLenght*1.3 < EXP->lenghtToGoal)
			return true;
	}

	return false;
}
