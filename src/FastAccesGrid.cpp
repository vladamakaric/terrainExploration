#include "FastAccesGrid.h"
#include "simulationDefinitions.h"
#include "draw.h"
#include <math.h>
#include "functions.h"
#include "LineSegment.h"

CFastAccesGrid::CFastAccesGrid(int _rows, int _columns, CTerrain *_terrain)
{
	rows = _rows;
	columns = _columns;
	tileWidth = CSimulation::simulation->mapWidth/_columns;
	tileHeight = CSimulation::simulation->mapHeight/_rows;

	grid = new CFAGTile*[rows];
	for(int i=0; i < rows; i++)
		grid[i] = new CFAGTile[columns];

	for(int i=0; i < rows; i++)
	{
		for(int j=0; j < columns; j++)
		{
			grid[i][j].SetUp(i,j,this);
		}
	}

	vector<CFAGTile*> lineTiles;
	CLineSegment ls;
	for(int i=0; i < _terrain->obstacles.size(); i++ )
	{
		for(int j=0; j < _terrain->obstacles[i].polygon->pointNum; j++)
		{
			ls.Set(_terrain->obstacles[i].nodeLines[j]);
			GetLineSegmentTiles(ls, lineTiles);

			if(!lineTiles.size())
				continue;
			else
			{
				for(int k=0; k < lineTiles.size(); k++)
					lineTiles[k]->nodeLines.push_back(&_terrain->obstacles[i].nodeLines[j]);

				lineTiles.clear();
			}
		}
	}

	for(int i=0; i < rows; i++)
	{
		for(int j=0; j < columns; j++)
		{
			grid[i][j].intersected = grid[i][j].nodeLines.size();
			grid[i][j].DetermineUniqueObstacles();

			if(!grid[i][j].intersected)
			{

				CObstacle* obstcl = TileCompletelyInObstacle(&grid[i][j]);
				grid[i][j].inObstacle = obstcl;

				if(obstcl)
					grid[i][j].obstacles.push_back(obstcl);
			}
		}
	}

	for(int i=0; i < 4; i++)
	{
		U_TERRAIN->levelEdges[i]->parentID = -45;
		ls.Set(*U_TERRAIN->levelEdges[i]);
		GetLineSegmentTiles(ls, lineTiles);

		for(int k=0; k < lineTiles.size(); k++)
			lineTiles[k]->nodeLines.push_back(U_TERRAIN->levelEdges[i]);

		lineTiles.clear();
	}
}

bool CFastAccesGrid::GetUntaggedNodesInCircle(const CCircle &_c, vector<CNode*> &_circleNodes)
{
	vector<CFAGTile*> _circleTiles;
	GetCircleTiles(_c, _circleTiles);

	bool addedNode = false;
	for(int i=0; i<_circleTiles.size(); i++ )
	{
		for(int j=0; j<_circleTiles[i]->nodes.size(); j++)
		{
			if(_circleTiles[i]->nodes[j]->tagged)
				continue;

			if(F::INTERSECTION::PointInCircle(_circleTiles[i]->nodes[j]->position, _c ))
			{
				_circleNodes.push_back(_circleTiles[i]->nodes[j]);
				addedNode = true;
			}
		}
	}

	return addedNode;
}

CFAGTile *CFastAccesGrid::GetTileAt(const CVector& _v)
{
	int col,row;
	col = int(_v.x / tileWidth);
	row = int(_v.y / tileHeight);

	return &grid[row][col];
}

void CFastAccesGrid::GetLineSegmentTiles( const CLineSegment &_ls, vector<CFAGTile*> &_lineTiles )
{
	double left, right, top, bottom;
	F::MISC::GetLeftRightTopBottomFromLineSegment(_ls,right, left, top, bottom);

	int firstCol = left/tileWidth;
	int lastCol = right/tileWidth;

	int firstRow = bottom/tileHeight;
	int lastRow = top/tileHeight;

	if(firstRow==lastRow)
	{
		for(int i=firstCol; i< lastCol + 1; i++ )
			_lineTiles.push_back(&grid[firstRow][i]);
		return;
	}

	if(firstCol == lastCol)
	{
		for(int i=firstRow; i< lastRow + 1; i++ )
			_lineTiles.push_back(&grid[i][firstCol]);
		return;
	}

	CLine line(_ls);
	double k = (_ls.a.y - _ls.b.y)/(_ls.a.x - _ls.b.x);
	bool growing = k > 0;
	int currentCol = firstCol;

	int currentRow;
	int rowTraversal;
	double runY;

	if(k>0)
	{
		currentRow = firstRow;
		currentRow++;
		runY = bottom - firstRow*tileHeight;
		rowTraversal = 1;
	}
	else
	{
		currentRow = lastRow;
		currentRow--;
		runY =  lastRow*tileHeight + tileHeight - top;
		rowTraversal = -1;
	}

	k = fabs(k);

	double toRunX = tileWidth - (left - firstCol*tileWidth);
	double toRunY = toRunX*k;
	runY += toRunY;

	double yRize = tileWidth*k;
	double lastYRun;
	lastYRun = (right - lastCol*tileWidth)*k; 

	int prevCol = currentCol;
	for(; currentCol<lastCol; currentCol++)
	{
		if(runY>tileHeight)
		{
			int rowNumRun = runY/tileHeight;
			runY -= rowNumRun*tileHeight;

			for(int i = 0; i<rowNumRun; i++ )
				_lineTiles.push_back(&grid[currentRow+i*rowTraversal][currentCol]);

			int colRunNum = currentCol+1 - prevCol;

			for(int i = prevCol; i< currentCol+1; i++)
				_lineTiles.push_back(&grid[currentRow-rowTraversal][i]);

			currentRow+= rowTraversal*rowNumRun;

			prevCol = currentCol+1;
		}

		runY += yRize;
	}

	runY += lastYRun - yRize;

	if(runY>tileHeight)
	{
		int rowNumRun = runY/tileHeight;

		for(int i = 0; i<rowNumRun; i++ )
			_lineTiles.push_back(&grid[currentRow+i*rowTraversal][currentCol]);
	}

	int colRunNum = currentCol+1 - prevCol;

	for(int i = prevCol; i< currentCol+1; i++)
		_lineTiles.push_back(&grid[currentRow-rowTraversal][i]);
}

static inline bool TileIsInObstaclesOtherwiseAddToTilesVector( CFAGTile *_ftile,vector<CFAGTile*> &_lineTiles)
{
	if(_ftile->inObstacle)
		return true;

	_lineTiles.push_back(_ftile);
	return false;
}

bool CFastAccesGrid::GetLineSegmentTiles_ReturnTrueIfTileIsInObstacle( const CLineSegment &_ls, vector<CFAGTile*> &_lineTiles )
{
	double left, right, top, bottom;
	F::MISC::GetLeftRightTopBottomFromLineSegment(_ls,right, left, top, bottom);

	int firstCol = left/tileWidth;
	int lastCol = right/tileWidth;

	int firstRow = bottom/tileHeight;
	int lastRow = top/tileHeight;

	if(firstRow==lastRow)
	{
		for(int i=firstCol; i< lastCol + 1; i++ )
		{
			if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[firstRow][i], _lineTiles))
				return true;
		}
		return false;
	}

	if(firstCol == lastCol)
	{
		for(int i=firstRow; i< lastRow + 1; i++ )
		{
			if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[i][firstCol], _lineTiles))
			return true;
		}
		return false;
	}

	CLine line(_ls);
	double k = (_ls.a.y - _ls.b.y)/(_ls.a.x - _ls.b.x);
	bool growing = k > 0;
	int currentCol = firstCol;

	int currentRow;
	int rowTraversal;
	double runY;

	if(k>0)
	{
		currentRow = firstRow;
		currentRow++;
		runY = bottom - firstRow*tileHeight;
		rowTraversal = 1;
	}
	else
	{
		currentRow = lastRow;
		currentRow--;
		runY =  lastRow*tileHeight + tileHeight - top;
		rowTraversal = -1;
	}

	k = fabs(k);

	double toRunX = tileWidth - (left - firstCol*tileWidth);
	double toRunY = toRunX*k;
	runY += toRunY;

	double yRize = tileWidth*k;
	double lastYRun;
	lastYRun = (right - lastCol*tileWidth)*k; 

	int prevCol = currentCol;
	for(; currentCol<lastCol; currentCol++)
	{
		if(runY>tileHeight)
		{
			int rowNumRun = runY/tileHeight;
			runY -= rowNumRun*tileHeight;

			for(int i = 0; i<rowNumRun; i++ )
				if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[currentRow+i*rowTraversal][currentCol], _lineTiles))
					return true;
			

			int colRunNum = currentCol+1 - prevCol;

			for(int i = prevCol; i< currentCol+1; i++)
				if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[currentRow-rowTraversal][i], _lineTiles))
					return true;

			currentRow+= rowTraversal*rowNumRun;

			prevCol = currentCol+1;
		}

		runY += yRize;
	}

	runY += lastYRun - yRize;

	if(runY>tileHeight)
	{
		int rowNumRun = runY/tileHeight;

		for(int i = 0; i<rowNumRun; i++ )
			if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[currentRow+i*rowTraversal][currentCol], _lineTiles))
				return true;
	}

	int colRunNum = currentCol+1 - prevCol;

	for(int i = prevCol; i< currentCol+1; i++)
		if(TileIsInObstaclesOtherwiseAddToTilesVector(&grid[currentRow-rowTraversal][i], _lineTiles))
			return true;

	return false;
}

void CFastAccesGrid::GetCircleTiles(const  CCircle &_c, vector<CFAGTile*> &_circleTiles)
{
	double right = _c.position.x + _c.r;
	double left = _c.position.x - _c.r;
	double up = _c.position.y + _c.r;
	double down = _c.position.y - _c.r;

	int rightCol =  right/tileWidth;
	int leftCol =  left/tileWidth;
	int upRow =  up/tileHeight;
	int downRow =  down/tileHeight;

	KeepGridIndicesInRange(upRow,rightCol);
	KeepGridIndicesInRange(downRow,leftCol);

	int colNum = rightCol - leftCol;
	int rowNum = upRow - downRow;

	for(int i=0; i < rowNum+1; i++)
	{
		for(int j=0; j < colNum+1; j++)
		{
			_circleTiles.push_back(&grid[i+downRow][j+leftCol]);
		}
	}
}

void CFastAccesGrid::GetLineSegmentNodeLineSegments(const CLineSegment &_ls, vector<CNodeLineSegment*> &_nodeLineSegments)
{
	vector<CFAGTile*> lineSegmentFAGTiles;
	GetLineSegmentTiles(_ls,lineSegmentFAGTiles);
	F::MISC::GetUniqueNodeLineSegmentsFromFAGTiles(_nodeLineSegments, lineSegmentFAGTiles);
}

bool CFastAccesGrid::GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle( const CLineSegment &_ls, vector<CNodeLineSegment*> &_nodeLineSegments)
{
	vector<CFAGTile*> lineSegmentFAGTiles;

	if(GetLineSegmentTiles_ReturnTrueIfTileIsInObstacle(_ls,lineSegmentFAGTiles))
		return true;

	F::MISC::GetUniqueNodeLineSegmentsFromFAGTiles(_nodeLineSegments, lineSegmentFAGTiles);

	return false;
}

void CFastAccesGrid::GetCircleNodeLineSegments(const CCircle &_c, vector<CNodeLineSegment*> &_nodeLineSegments)
{
	vector<CFAGTile*> circleFAGTiles;
	GetCircleTiles(_c, circleFAGTiles);
	F::MISC::GetUniqueNodeLineSegmentsFromFAGTiles(_nodeLineSegments, circleFAGTiles);
}

void CFastAccesGrid::Draw()//**
{
	glColor3f(0,1,1);
	glBegin(GL_LINES);
	for(int i=0; i < rows; i++)
	{
		Draw::PushVertex(0, i*tileHeight);
		Draw::PushVertex(U_SIMULATION->mapWidth, i*tileHeight);
	}

	for(int i=0; i < columns; i++)
	{
		Draw::PushVertex(i*tileWidth, 0);
		Draw::PushVertex(i*tileWidth,U_SIMULATION->mapHeight);
	}
	glEnd();


	for(int i=0; i < rows; i++)
	{
		for(int j=0; j < columns; j++)
		{
			grid[i][j].Draw();
		}
	}
}

bool CFastAccesGrid::GetNearestCollisionWithLineSegment( const CLineSegment &_ls, const CVector &_refPoint, CCollisionPoint& _cp, CNode* _forbiden1/*=0*/, CNode* _forbiden2/*=0*/ )
{
	vector<CNodeLineSegment*> nodeLines;
	GetLineSegmentNodeLineSegments(_ls, nodeLines);

	CVector intersection(0,0);

	double minDist = 999999999999;
	double tempDist;

	bool foundIntersection = false;

	CLineSegment testLS;
	for(int i=0; i< nodeLines.size(); i++)
	{
		if(nodeLines[i]->down == _forbiden1 || nodeLines[i]->down == _forbiden2 || nodeLines[i]->up == _forbiden1 || nodeLines[i]->up == _forbiden2)
			continue;

		testLS.Set(*nodeLines[i]);
		if(F::INTERSECTION::LineSegmentIntersection(_ls, testLS, &intersection))	
		{
			foundIntersection = true;
			tempDist = F::DISTANCE::GetDistanceSq(intersection, _refPoint);
			if( tempDist < minDist )
			{
				minDist = tempDist;
				_cp.intersection = intersection;
				_cp.nodeLine = nodeLines[i];
			}
		}
	}

	return foundIntersection;
}

bool CFastAccesGrid::GoodEmptyTileUnTagged(int _r, int _c)
{
	if(GoodTileIndices(_r, _c))
		return !(grid[_r][_c].tagged || grid[_r][_c].intersected);
	return false;
}

bool CFastAccesGrid::GetGoodEmptyUnTaggedTileIndices8Way(int &_r, int &_c)
{
	if(GoodEmptyTileUnTagged(_r,_c))
		return true;

	if(GoodEmptyTileUnTagged(_r-1,_c-1))
	{
		_r--;
		_c--;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r+1,_c+1))
	{
		_r++;
		_c++;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r+1,_c-1))
	{
		_r++;
		_c--;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r-1,_c+1))
	{
		_r--;
		_c++;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r,_c+1))
	{
		_c++;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r+1,_c))
	{
		_r++;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r,_c-1))
	{
		_c--;
		return true;
	}

	if(GoodEmptyTileUnTagged(_r-1,_c))
	{
		_r--;
		return true;
	}

	return false;
}

bool CFastAccesGrid::LineSegmentCollisionWithObstacles_ForbiddenNLSs( const CLineSegment &_ls, CNodeLineSegment* _NLS1/*=0*/, CNodeLineSegment* _NLS2/*=0*/ )
{
	vector<CNodeLineSegment*> nodeLines;

	CLineSegment testLS;

	if(GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle(_ls, nodeLines))
		return true;

	for(int i=0; i < nodeLines.size(); i++)
	{
		if( nodeLines[i] == _NLS1 || nodeLines[i] == _NLS2)
			continue;

		testLS.Set(*nodeLines[i]);
		if(F::INTERSECTION::LineSegmentIntersection( _ls, testLS))
			return true;
	}

	CVector middlePoint = _ls.MiddlePoint();
	return PointInObstacle(middlePoint);
}

bool CFastAccesGrid::LineSegmentCollisionWithObstacles_ForbiddenNodes( const CLineSegment &_ls, CNode* _node1/*=0*/, CNode *_node2/*=0*/ )
{
	vector<CNodeLineSegment*> nodeLines;

	CLineSegment testLS;

	if(GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle(_ls, nodeLines))
		return true;

	for(int i=0; i < nodeLines.size(); i++)
	{
		if( nodeLines[i]->down == _node1 || nodeLines[i]->up == _node1 || nodeLines[i]->down == _node2 || nodeLines[i]->up == _node2 )
			continue;

		testLS.Set(*nodeLines[i]);
		if(F::INTERSECTION::LineSegmentIntersection( _ls, testLS))
			return true;
	}


	CVector middlePoint = _ls.MiddlePoint();
	return PointInObstacle(middlePoint);
}

bool CFastAccesGrid::LineSegmentCollisionWithObstacles_ForbiddenNodeAndNLS( const CLineSegment &_ls, CNode* _node, CNodeLineSegment *_NLS )
{
	vector<CNodeLineSegment*> nodeLines;

	CLineSegment testLS;

	if(GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle(_ls, nodeLines))
		return true;

	for(int i=0; i < nodeLines.size(); i++)
	{
		if( nodeLines[i]->down == _node || nodeLines[i]->up == _node || nodeLines[i] == _NLS )
			continue;

		testLS.Set(*nodeLines[i]);
		if(F::INTERSECTION::LineSegmentIntersection( _ls, testLS))
			return true;
	}

	CVector middlePoint= _ls.MiddlePoint();
	return PointInObstacle(middlePoint);
}

bool CFastAccesGrid::LineSegmentCollisionWithObstacles_ForbiddenLSPoints( const CLineSegment &_ls )
{
	vector<CNodeLineSegment*> nodeLines;

	CLineSegment testLS;

	if(GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle(_ls, nodeLines))
		return true;

	CVector intersection;
	for(int i=0; i < nodeLines.size(); i++)
	{
		testLS.Set(*nodeLines[i]);
		if(F::INTERSECTION::LineSegmentIntersection( _ls, testLS,&intersection))
		{
			if(!F::DISTANCE::DistBetweenVecsBiggerThan(intersection, _ls.a, 0.2) || !F::DISTANCE::DistBetweenVecsBiggerThan(intersection, _ls.b, 0.2))
				continue;
		
			return true;
		}
	}

	CVector middlePoint= _ls.MiddlePoint();
	return PointInObstacle(middlePoint);
}

bool CFastAccesGrid::LineSegmentCollisionWithObstacles( const CLineSegment &_ls )
{
	return LineSegmentCollisionWithObstacles_ForbiddenNLSs(_ls);
}

CObstacle* CFastAccesGrid::PointInObstacle( CVector _point )
{
	CFAGTile *ftile = GetTileAt(_point);

	if(ftile->inObstacle)
		return ftile->obstacles[0];

	if(!ftile->intersected)
		return 0;

	for(int i=0; i< ftile->obstacles.size(); i++)
	{
		if(ftile->obstacles[i]->PointInside(_point))
			return ftile->obstacles[i];
	}

	return 0;
}

CObstacle* CFastAccesGrid::TileCompletelyInObstacle( CFAGTile* _ftile )
{
	double xLeft = _ftile->colNum * tileWidth;
	double yDown = _ftile->rowNum * tileHeight;
	CVector testEdge(xLeft,yDown);

	CObstacle* returnObstacle;

	if(!(returnObstacle = U_TERRAIN->PointInObstacles(testEdge)))
		return NULL;

	testEdge.x += tileWidth;

	if(!(returnObstacle = U_TERRAIN->PointInObstacles(testEdge)))
		return NULL;

	testEdge.y += tileHeight;

	if(!(returnObstacle = U_TERRAIN->PointInObstacles(testEdge)))
		return NULL;

	testEdge.x -= tileWidth;

	if(!(returnObstacle = U_TERRAIN->PointInObstacles(testEdge)))
		return NULL;

	return returnObstacle;
}








