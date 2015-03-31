#pragma once
#include "Vector.h"
#include "LineSegment.h"
#include "Capsule.h"
#include <list>
#include <vector>
#include "Path.h"

using namespace std;

class CLSVNode
{
public:
	CLSVNode(CVector _pos, bool _orig=false) : position(_pos), orig(_orig), connection(0), tagged(0) {}
	CLSVNode(){}
	CLSVNode *connection;
	CVector position;
	bool orig;
	bool tagged;
};

class CLineSegmentVisibility
{
public:
	CLineSegmentVisibility(const CLineSegment& _ls);
	~CLineSegmentVisibility();

	CLSVNode a, b;
	list<CLSVNode> LSVNodes;
	vector<CCapsule*> capsulesTested;

	bool TestForCollisionRememberNothing( const vector<CPath*> &paths);
	void TaggLSVNodesInCapsule(const CCapsule &_c);
	bool AddCapsuleIntersections(const CCapsule &_c); 
	bool TestUpdate(const vector<CPath*> &paths);
	bool AddIntersectionToLS_OneOrigNodeVisible(const CVector &_intersection, CLSVNode *_visibleOrigLSVNode);
	bool Add2IntersectionsToLS_NoOrigNodesVisible(CVector *_intersections);
	bool NewLSIntersectionsWithNoOrigNodesVisibleGood(CVector *_intersections);	
	void Draw();

	/////////
	static CLineSegmentVisibility* LSV; 
	static bool Test(const CLineSegment &_ls, const vector<CPath*> &paths);
};


