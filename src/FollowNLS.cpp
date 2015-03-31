#include "FollowNLS.h"
#include "f_vector.h"
#include "f_distance.h"
#include "AIS_GoToUnseenPosition.h"
#include "FieldOfView.h"
#include "ShortestPathGenerator.h"


CFollowNLS::CFollowNLS( CFOVNode* _FNode, CAIS_GoToUnseenPosition* _EXP ) :  NLS(_FNode->origNLS)
{
	EXP = _EXP;
	lastPathFN = _FNode;
	currentID = _FNode->ID;
	updateCount = 0;
	currentFollowNodePosition = lastPathFN->position;

	CVector aPathNode = EXP->path[EXP->path.size()-3];
	CVector toNode = F::VECTOR::GetVectorFromAToB(aPathNode, lastPathFN->position );
	direction = F::VECTOR::GetVectorProjectionOnLineSegment(toNode, CLineSegment(*NLS)).Direction();
}

bool CFollowNLS::Obsolete()
{
	return EXP->behaviour->CE->FOV->DAT->NLSEmpty(NLS);
}

bool CFollowNLS::FollowHasChanged()
{
	if(updateCount*EXP->behaviour->CE->speed > 1600)
	{
		updateCount = 0;
		return true;
	}

	return lastPathFN->origNLS != NLS;
}

bool CFollowNLS::DiscardFNode( CFOVNode* _FOVN )
{
	if(!anyNodeOnNLSSatisfiesDirection)
		return false;

	if(!_FOVN->origNLS)
		return false;

	if(_FOVN->origNLS != NLS)
		return false;

	return DiscardNodeOnSameNLS(_FOVN);
}

bool CFollowNLS::AnyNodeOnNLSSatisfiesDirection()
{
	CFOVNode **fovNodes;
	int fovNodesNumber =  EXP->behaviour->CE->FOV->DAT->GetFOVNodesFromNLS(NLS, &fovNodes);
	if(!fovNodesNumber) return false;

	bool satisfy = false;

	for(int i=0; i < fovNodesNumber; i++)
	{
		if(!DiscardNodeOnSameNLS(fovNodes[i]))
		{
			satisfy = true;
			break;
		}
	}

	return satisfy;
}

void CFollowNLS::Update()
{
	updateCount++;
	anyNodeOnNLSSatisfiesDirection = true;
	anyNodeOnNLSSatisfiesDirection = AnyNodeOnNLSSatisfiesDirection();

	nodePath.clear();

	bool FNIValid = CurrentFNIsValid();

	CFieldOfView* FOV = EXP->behaviour->CE->FOV;

	if(!FOV->altered && FNIValid)
		return;

	bool createNewPath = NewNodesCouldMakeBetterPath();

	if(FNIValid)
	{
		if(!createNewPath)
			return;
	}
	else
	{
		if(ContinueAlongSameNLS())
			return;
	}

	CreatePath(nodePath);

	if(FNIValid)
	{
		if(CShortestPathGenerator::pathLenght*1.1 < EXP->lenghtToGoal)
			UpdatePath(nodePath);
	}
	else UpdatePath(nodePath);

	EXP->ClearPathGenerationVariables();
}

bool CFollowNLS::CurrentFNIsValid()
{
	CFieldOfView *FOV = EXP->behaviour->CE->FOV;
	for(int i=0; i< FOV->edgdeFOVNodes.size(); i++)
	{
		if(FOV->edgdeFOVNodes[i]->ID == currentID)
			return true;
	}

	return false;
}
void CFollowNLS::GenerateNewPath()
{

}

bool CFollowNLS::NewNodesCouldMakeBetterPath()
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
		
		if(current->origNLS==NLS)
			continue;

		testLenght = F::DISTANCE::GetDistance(current->position, EXP->behaviour->CE->positionViewRadius.position) + 
		F::DISTANCE::GetDistance(EXP->USPosition, current->position);

		if(testLenght*1.1 < EXP->lenghtToGoal)
			return true;
	}

	return false;
}

bool CFollowNLS::ContinueAlongSameNLS()
{
	CFieldOfView *FOV = EXP->behaviour->CE->FOV;
	CFOVNode* current;

	for(int i=0; i < FOV->newCurrentlyVisibleNLSFOVNodes.size(); i++)
	{
		current = FOV->newCurrentlyVisibleNLSFOVNodes[i];

		if(current->origNLS == NLS)
		if(!DiscardNodeOnSameNLS(current))
		{
			UpdatePathWithNewNLSNode(current);
			return true;
		}
	}

	return false;
}

void CFollowNLS::UpdatePathWithNewNLSNode( CFOVNode* _nlsNode )
{
	lastPathFN = _nlsNode;
	currentID = _nlsNode->ID;
	EXP->path[EXP->path.size()-2] = _nlsNode->position;
	EXP->moveAlongPath->CurrentPathNodeHasChanged();
}

bool CFollowNLS::DiscardNodeOnSameNLS( CFOVNode* _sameNlsFN )
{
	CVector aPathNode = EXP->path[EXP->path.size()-3];
	CVector toNode =  F::VECTOR::GetVectorFromAToB(aPathNode, _sameNlsFN->position );
	CVector tempDirection = F::VECTOR::GetVectorProjectionOnLineSegment(toNode, CLineSegment(*NLS)).Direction();

	if( direction * tempDirection > 0)
		return false;

	return true;
}

