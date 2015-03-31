#pragma once
#include "Follow.h"

class CFollowNode : public CFollow
{
public:
	CFollowNode(CFOVNode* _FNode , CAIS_GoToUnseenPosition* _EXP);
	void SetUpTempNodes( CFOVNode &_CEPosTempNode, CFOVNode & _USPositionTempNode );
	bool FollowHasChanged();
	bool Obsolete() {return false;}
	void Update();
	void GenerateNewPath();
	bool NewNodesCouldMakeBetterPath();
};