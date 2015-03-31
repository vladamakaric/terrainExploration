#pragma once
#include <vector>
#include "Obstacle.h"
#include "NodeLineSegment.h"

using namespace std;

class CFastAccesGrid;
class CFAGTile
{
public:
	int rowNum;
	int colNum;
	bool tagged;
	bool intersected;
	bool inObstacle;
	vector<CNodeLineSegment*> nodeLines;
	vector<CNode*> nodes;
	vector<CObstacle*> obstacles;
	CFastAccesGrid* parentFAG;

	CFAGTile() : tagged(0), intersected(0), inObstacle(0) {}
	void SetUp(int _rNum, int _cNum, CFastAccesGrid* _parentFAG); 
	void DetermineUniqueObstacles();

	//////
	void Draw();
};

#include "FastAccesGrid.h"