#pragma once
#include "FOVNode.h"
#include "Vector.h"
#include <vector>
#include "PathFindingNode.h"

using namespace std;


class CAIS_GoToUnseenPosition;
class CFollow
{
public:
	virtual bool Obsolete()=0;
	virtual bool FollowHasChanged()=0;
	virtual void Update()=0;
	virtual void UpdatePath(vector<CPathFindingNode*> &_nodePath);
	virtual bool DiscardFNode(CFOVNode* _FOVN){return false;}
	virtual void GenerateNewPath(){};


	bool CreatePath(vector<CPathFindingNode*> &_nodePath);

	vector<CPathFindingNode*> nodePath;
	CAIS_GoToUnseenPosition* EXP;
	CFOVNode *lastPathFN;
	int currentID;
	int updateCount;

	/////
	virtual void Draw();
};
