#pragma once
#include "AIState.h"
#include "FOVNode.h"
#include "Vector.h"
#include <vector>
#include "FieldOfView.h"
#include "MoveAlongPath.h"
#include "UnseenPositionNodeManager.h"



class CFollow;
class CAIS_GoToUnseenPosition : public CAIState
{
public:
	CAIS_GoToUnseenPosition(const CVector &_position,CAIBehaviour* _behaviour); 
	bool DirectionFree();
	void CreateNewFollow();


	bool CEPosFNNotConnected();
	void UpdateFollow();
	void SetUpTempNodes(CFollow* _follow = 0);
	void ClearFollow();
	void ChangeFollow();
	void AddNeighbourToCEPositionFN(CFOVNode *_fnode, bool _LSITest = true);
	bool PathFromNodeToGoalGoodForAllEdgeNodes(bool **edgeNodes);
	bool Run();
	void Draw();

	void UpdateTempCEPosFNWithoutLSITest(CFollow* _follow = 0);
	void ClearPathGenerationVariables();

	CUnseenPositionNodeManager USPNManager;
	CFOVNode CEPosFN, USPositionFN;


	bool CEPosFNUpdated;
	double lenghtToGoal;
	bool directionFree;
	vector<CVector> path;
	CMoveAlongPath* moveAlongPath;
	CFollow* follow;
	CVector USPosition;
	CVector direction;
};

#include "Follow.h"