#pragma once
#include "Follow.h"
#include "NodeLineSegment.h"

class CFollowNLS : public CFollow
{
public:
	CFollowNLS(CFOVNode* _pathGoalFOVN, CAIS_GoToUnseenPosition* _EXP);

	bool Obsolete();
	void SetUpTempNodes( CFOVNode &_CEPosTempNode, CFOVNode & _USPositionTempNode );
	bool FollowHasChanged();
	bool DiscardFNode(CFOVNode* _FOVN);
	bool AnyNodeOnNLSSatisfiesDirection();
	bool CurrentFNIsValid();
	void Update();
	void GenerateNewPath();
	bool NewNodesCouldMakeBetterPath();
	bool DiscardNodeOnSameNLS(CFOVNode* _sameNlsFN);
	bool ContinueAlongSameNLS();
	void UpdatePathWithNewNLSNode(CFOVNode* _nlsNode);

	bool anyNodeOnNLSSatisfiesDirection;
	CNodeLineSegment* NLS;
	CVector currentFollowNodePosition;
	CVector direction;

};