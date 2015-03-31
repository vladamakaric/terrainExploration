#include "LineSegmentVisibility.h"
#include "Circle.h"
#include "functions.h"
#include "draw.h"

CLineSegmentVisibility* CLineSegmentVisibility::LSV = 0;

CLineSegmentVisibility::CLineSegmentVisibility( const CLineSegment& _ls )
{
	a = CLSVNode(_ls.a, true);
	b = CLSVNode(_ls.b, true);
}

bool CLineSegmentVisibility::Test( const CLineSegment &_ls, const vector<CPath*> &paths )
{
	if(LSV)
		delete LSV;

	LSV = new CLineSegmentVisibility(_ls);

	return LSV->TestForCollisionRememberNothing(paths);
}

bool CLineSegmentVisibility::TestForCollisionRememberNothing( const vector<CPath*> &paths )
{
	for(int i=0; i<paths.size(); i++)
	{
		for(int j=0; j< paths[i]->capsules.size(); j++)
		{
			if(AddCapsuleIntersections(*paths[i]->capsules[j])) 
				return true;
		}
	}

	return false;
}

bool CLineSegmentVisibility::TestUpdate( const vector<CPath*> &paths )
{
	F::MISC::TagPointerContainer(capsulesTested);

	for(int i=0; i<paths.size(); i++)
	{
		for(int j=0; j< paths[i]->capsules.size(); j++)
		{
			if(paths[i]->capsules[j]->tagged)
				continue;

			if(j==paths[i]->capsules.size()-1)
			{
				if(!paths[i]->lastIsTemp)
					capsulesTested.push_back(paths[i]->capsules[j]);
			}
			else
			capsulesTested.push_back(paths[i]->capsules[j]);

			if(AddCapsuleIntersections(*paths[i]->capsules[j])) 
			{
				F::MISC::UntagPointerContainer(capsulesTested);
				return true;
			}
		}
	}

	F::MISC::UntagPointerContainer(capsulesTested);
	return false;
}

bool CLineSegmentVisibility::AddCapsuleIntersections( const CCapsule &_c )
{
	CCircle boundingCircle((_c.center.a + _c.center.b)/2, F::DISTANCE::GetDistance(_c.center.a, _c.center.b) + _c.r);
	CLineSegment ls(a.position, b.position);

	if(!F::INTERSECTION::LineSegmentIntersectsCircle(ls, boundingCircle) && !F::INTERSECTION::PointInCircle(a.position ,boundingCircle) && !F::INTERSECTION::PointInCircle(b.position ,boundingCircle))
		return false;

	F::MISC::UntagObjectContainer(LSVNodes);
	a.tagged = false;
	b.tagged = false;

	TaggLSVNodesInCapsule( _c );

	if(a.tagged && b.tagged)
		return true;

	CLSVNode *visible=0;

	if(a.tagged) visible = &a;
	if(b.tagged) visible = &b;

	bool LSVisible = false;

	CVector intersections[2];
	int interNum = F::INTERSECTION::GetLineSegmentCapsuleIntersection(ls, _c, intersections);

	if(!interNum)
		return false;

	if(visible)
		LSVisible = AddIntersectionToLS_OneOrigNodeVisible(intersections[0], visible);
	else 
		if(interNum == 2)
			LSVisible = Add2IntersectionsToLS_NoOrigNodesVisible(intersections);

	return LSVisible;
}

void CLineSegmentVisibility::TaggLSVNodesInCapsule( const CCapsule &_c )
{
	if(F::INTERSECTION::PointInCapsule(a.position, _c))
		a.tagged = true;

	if(F::INTERSECTION::PointInCapsule(b.position, _c))
		b.tagged = true;

	if(a.tagged && b.tagged)
		return;

	for(list<CLSVNode>::iterator iter = LSVNodes.begin(); iter!=LSVNodes.end(); iter++)
	{
		if(F::INTERSECTION::PointInCapsule(iter->position, _c)) 
			iter->tagged = true;
	}	
}

bool CLineSegmentVisibility::AddIntersectionToLS_OneOrigNodeVisible( const CVector &_intersection, CLSVNode *_visibleOrigLSVNode )
{
	CLSVNode *outOfViewConnection = 0;
	CLSVNode *otherOrigLSVNode = &a;

	if(_visibleOrigLSVNode == &a)
		otherOrigLSVNode = &b;

	list<CLSVNode>::iterator iter = LSVNodes.begin();
	for( ;iter!=LSVNodes.end(); iter++)
	{
		if(!iter->tagged)
		{
			if(iter->connection == _visibleOrigLSVNode)
				return false;
			continue;
		}

		if(!iter->connection)
			continue;
		
		if(iter->connection == otherOrigLSVNode)
			return true;

		if(!iter->connection->tagged)
		{
			outOfViewConnection = iter->connection;
			break;
		}
	}

	iter = LSVNodes.begin();
	while(iter!=LSVNodes.end())
	{
		if(iter->tagged)
			iter = LSVNodes.erase(iter);
		else
			iter++;
	}

	if(!outOfViewConnection)
	{
		LSVNodes.push_back(CLSVNode(_intersection));
		LSVNodes.rbegin()->connection = _visibleOrigLSVNode;
		_visibleOrigLSVNode->connection = &(*LSVNodes.rbegin());
	}
	else
	{
		outOfViewConnection->connection = _visibleOrigLSVNode;
		_visibleOrigLSVNode->connection = outOfViewConnection;
	}

	return false;
}

bool CLineSegmentVisibility::Add2IntersectionsToLS_NoOrigNodesVisible( CVector *_intersections )
{
	CLSVNode *maxDistLSVN=0;
	CLSVNode *minDistLSVN=0;
	CVector refPoint = a.position;
	double maxDist = 0;
	double minDist= 99999999999999;
	double dist;

	list<CLSVNode>::iterator iter = LSVNodes.begin();
	for( ;iter!=LSVNodes.end(); iter++)
	{
		if(!iter->tagged)
			continue;

		dist = F::DISTANCE::GetDistanceSq(refPoint, iter->position);

		if(dist > maxDist)
		{
			maxDist = dist;
			maxDistLSVN = &(*iter);
		}

		if(dist < minDist)
		{
			minDist = dist;
			minDistLSVN = &(*iter);
		}
	}

	if(!maxDist)
	{
		if(!NewLSIntersectionsWithNoOrigNodesVisibleGood(_intersections))
			return false;

		CLSVNode *newNode1, *newNode2;
		LSVNodes.push_back(CLSVNode(_intersections[0]));
		newNode1 = &(*LSVNodes.rbegin());
		LSVNodes.push_back(CLSVNode(_intersections[1]));
		newNode2 = &(*LSVNodes.rbegin());

		newNode1->connection = newNode2;
		newNode2->connection = newNode1;

		return false;
	}

	if(maxDist)
	{
		CLSVNode *tempMaxDistLSVN = maxDistLSVN->connection;
		CLSVNode *tempMinDistLSVN = minDistLSVN->connection;
		double tempMinDist, tempMaxDist;

		bool minLSVNIsOrig = false, maxLSVNIsOrig = false;

		if(tempMinDistLSVN)
		if((tempMinDist = F::DISTANCE::GetDistanceSq(refPoint, tempMinDistLSVN->position))<minDist)
		{
			minDistLSVN = tempMinDistLSVN;
			minDist = tempMinDist;
		}

		if(tempMaxDistLSVN)
		if((tempMaxDist = F::DISTANCE::GetDistanceSq(refPoint, tempMaxDistLSVN->position))>maxDist)
		{
			maxDistLSVN = tempMaxDistLSVN;
			maxDist = tempMaxDist;
		}

		minLSVNIsOrig = minDistLSVN->orig;
		maxLSVNIsOrig = maxDistLSVN->orig;

		bool newMaxLSVN=false;
		bool newMinLSVN=false;

		if(	minLSVNIsOrig && maxLSVNIsOrig)
			return true;

		for(int i=0; i<2; i++)
		{
			dist = F::DISTANCE::GetDistanceSq(refPoint, _intersections[i]);

			if(dist>maxDist)
			{
				newMaxLSVN = true;
				LSVNodes.push_back(CLSVNode(_intersections[i]));
				maxDistLSVN = &(*LSVNodes.rbegin());
				maxDist = dist;
				continue;
			}

			if(dist<minDist)
			{
				newMinLSVN = true;
				LSVNodes.push_back(CLSVNode(_intersections[i]));
				minDistLSVN = &(*LSVNodes.rbegin());
				minDist = dist;
			}
		}

		if(!newMaxLSVN)
		{
			if(maxDistLSVN->tagged)
				maxDistLSVN->tagged=false;
		}

		if(!newMinLSVN)
		{
			if(minDistLSVN->tagged)
				minDistLSVN->tagged=false;
		}

		iter = LSVNodes.begin();
		while(iter!=LSVNodes.end())
		{
			if(iter->tagged)
				iter = LSVNodes.erase(iter);
			else
				iter++;
		}

		minDistLSVN->connection = maxDistLSVN;
		maxDistLSVN->connection = minDistLSVN;
	}

	return false;
}

bool CLineSegmentVisibility::NewLSIntersectionsWithNoOrigNodesVisibleGood( CVector *_intersections )
{
	//ovde CVektor je ustvari jednodimnezionalna linija, x je levo, a y je desno
	vector<CVector> oneDLines; 
	for(list<CLSVNode>::iterator iter = LSVNodes.begin();iter!=LSVNodes.end(); iter++)
	{
		if(iter->tagged)
			continue;

		if(iter->connection)
		{
			if(!iter->connection->orig)
				iter->connection->tagged = true;

			oneDLines.push_back(   CVector(  F::MISC::Min(iter->connection->position.x, iter->position.x),  F::MISC::Max(iter->connection->position.x, iter->position.x) )   );
		}
	}

	bool intersectionsGood = true;
	CVector middleIntersect =  (_intersections[0] + _intersections[1])/2;

	for(int j=0; j<oneDLines.size(); j++)
	{
		if(middleIntersect.x >= oneDLines[j].x && middleIntersect.x <= oneDLines[j].y)
		{
			intersectionsGood = false;
			break;
		}
	}

	F::MISC::UntagObjectContainer(LSVNodes);
	return intersectionsGood;
}

void CLineSegmentVisibility::Draw()
{
	for(list<CLSVNode>::iterator iter = LSVNodes.begin();iter!=LSVNodes.end(); iter++)
	{
		glColor3f(1,0,1);
		Draw::CircleFill(iter->position, 60,10);
	}
}

CLineSegmentVisibility::~CLineSegmentVisibility()
{
	LSVNodes.clear();
	capsulesTested.clear();
}
