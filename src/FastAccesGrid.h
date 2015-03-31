#pragma once
#include "FAGTile.h"
#include "Terrain.h"
#include <vector>
#include "LineSegment.h"
#include "Circle.h"

class CCollisionPoint
{
public:
	CNodeLineSegment* nodeLine;
	CVector intersection;
};

class CFastAccesGrid
{
public:
	CFAGTile **grid;
	int rows;
	int columns;
	int tileWidth;
	int tileHeight;

	bool LineSegmentCollisionWithObstacles_ForbiddenNLSs(const CLineSegment &_ls, CNodeLineSegment* _NLS1=0, CNodeLineSegment* _NLS2=0);
	bool LineSegmentCollisionWithObstacles_ForbiddenNodes(const CLineSegment &_ls, CNode* _node1=0, CNode *_node2=0);
	bool LineSegmentCollisionWithObstacles_ForbiddenNodeAndNLS(const CLineSegment &_ls, CNode* _node, CNodeLineSegment *_NLS);
	bool LineSegmentCollisionWithObstacles(const CLineSegment &_ls);
	bool LineSegmentCollisionWithObstacles_ForbiddenLSPoints(const CLineSegment &_ls);

	CObstacle* PointInObstacle(CVector &_point);
	

	void AddTileToSTLVector(int _r, int _c, vector<CFAGTile*> &_tilesVector);
	void KeepGridIndicesInRange(int &_row, int &_col);
	void KeepRowIndexInRange(int &_row);
	void KeepColumnIndexInRange(int &_col);
	void AddBorders();

	bool GetNearestCollisionWithLineSegment(const CLineSegment &_ls, const CVector &_refPoint, CCollisionPoint& _cp, CNode* _forbiden1=0, CNode* _forbiden2=0);
	bool GoodTileIndices(int _row, int _col);
	bool GoodTileColumnIndex(int _col);
	bool GoodTileRowIndex(int _row);
	
	void GetLineSegmentTiles(const CLineSegment &_ls, vector<CFAGTile*> &_lineTiles);
	bool GetLineSegmentTiles_ReturnTrueIfTileIsInObstacle(const CLineSegment &_ls, vector<CFAGTile*> &_lineTiles);

	void GetLineSegmentNodeLineSegments(const CLineSegment &_ls, vector<CNodeLineSegment*> &_nodeLines);
	bool GetLineSegmentNodeLineSegments_ReturnTrueIfLSIntersectsObstacle(const CLineSegment &_ls, vector<CNodeLineSegment*> &_nodeLines);
	void GetCircleTiles(const  CCircle &_c, vector<CFAGTile*> &_circleTiles);
	void GetCircleNodeLineSegments(const  CCircle &_c, vector<CNodeLineSegment*> &_nodeLines);
	bool GetUntaggedNodesInCircle(const CCircle &_c, vector<CNode*> &_circleNodes);

	CObstacle* TileCompletelyInObstacle(CFAGTile* _ftile);
	bool GetGoodEmptyUnTaggedTileIndices8Way(int &_r, int &_c);
	bool GoodEmptyTileUnTagged(int _r, int _c);
	
	CFastAccesGrid(int _rows, int _columns, CTerrain *_terrain);
	CFAGTile *GetTileAt(const CVector& _v);

	void Draw();
};

inline void CFastAccesGrid::KeepRowIndexInRange(int &_row)
{
	if(_row>rows-1) _row = rows-1;
	if(_row<0) _row =0;
}

inline void CFastAccesGrid::KeepColumnIndexInRange(int &_col)
{
	if(_col>columns-1) _col = columns-1;
	if(_col<0) _col =0;
}

inline void CFastAccesGrid::KeepGridIndicesInRange(int &_row, int &_col)
{
	KeepRowIndexInRange(_row);
	KeepColumnIndexInRange(_col);
}

inline void CFastAccesGrid::AddTileToSTLVector(int _r, int _c, vector<CFAGTile*> &_tilesVector)
{
	if(!grid[_r][_c].tagged)
	{
		_tilesVector.push_back(&grid[_r][_c]);
		grid[_r][_c].tagged = true;
	}
}

inline bool CFastAccesGrid::GoodTileColumnIndex(int _col)
{
	if(_col>0 && _col < columns)
		return true;
	return false;
}

inline bool CFastAccesGrid::GoodTileRowIndex(int _row)
{
		if(_row>0 && _row < rows)
			return true;
		return false;
}

inline bool CFastAccesGrid::GoodTileIndices(int _row, int _col)
{
	return GoodTileRowIndex(_row) && GoodTileColumnIndex(_col);
}

