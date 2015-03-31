#include "ConcaveHullGenerator.h"
#include "global.h"
#include "f_angle.h"
#include "f_distance.h"
#include "f_intersection.h"
#include "f_vector.h"
#include "draw.h"
#include <algorithm>
#include "NearestCandidate.h"
#include "LineSegment.h"


using namespace std;

class CKnearestCandidate
{
public:
	int pointArrayIndex;
	double distance;
	CKnearestCandidate(int _pai, double _d) : pointArrayIndex(_pai), distance(_d) {}
	bool operator<(CKnearestCandidate _a) { return distance < _a.distance; }
};

void CConcaveHullGenerator::Generate(CPointCluster *_pCluster, vector<CVector*> *_polyPoints, int _k, vector<CVector*> *_noClusterPoints)
{
	addedPointIndices.clear();
	kNearestIndices.clear();

	if(_pCluster)
	AddAllChainedClusterPoints(_pCluster);
	else
		points = _noClusterPoints;

	hull = _polyPoints;

	fistPointRemoved = false;
	k=_k;

	bool done=false;

	firstPointIndex = GetExtremeYAxisVectorIndex();
	(*points)[firstPointIndex]->y--; //mora da nebi bila 2 ista y

	do 
	{
		hull->clear();
		addedPointIndices.clear();
		fistPointRemoved = false;

		addedPointIndices.push_back(firstPointIndex);
		hull->push_back((*points)[firstPointIndex]);

		CVector a(-1,0);

		while(1)
		{
			if(hull->size()>1)
			{
				a = F::VECTOR::GetVectorFromAToB(*(*hull)[hull->size()-1],*(*hull)[hull->size()-2]);
			}

			if(!fistPointRemoved && hull->size()>2)
			{
				fistPointRemoved = true;
				addedPointIndices.erase(addedPointIndices.begin(), addedPointIndices.begin()+1);
			}

			if(!CalculateKNearestVecs())
			{
				break;
			}

			if(!CalculateNextHullPoint(a))
			{
				done=true;
				break;
			}
		} 

		k++;
	}while(!done);
}

int CConcaveHullGenerator::GetExtremeYAxisVectorIndex()
{
	double yMin = (*points)[0]->y;
	int minIndex = 0;
	for(int i=1; i < points->size(); i++)
	{
		if((*points)[i]->y < yMin)
		{
			yMin = (*points)[i]->y;
			minIndex = i;
		}
	}

	return minIndex;
}

bool CConcaveHullGenerator::IsAdded(int _i)
{
	for(int i=0; i < addedPointIndices.size(); i++)
	{
		if(_i == addedPointIndices[i])
			return true;
	}

	return false;
}

bool CConcaveHullGenerator::CalculateKNearestVecs()
{
	vector<CNearestCandidate> candidates;

	for(int i=0; i < points->size(); i++)
	{
		if(IsAdded(i)) continue;
		candidates.push_back( CNearestCandidate(i , ( ( *(*hull)[hull->size()-1]) - *(*points)[i]).LengthSq()) );
	}

	std::sort(candidates.begin(), candidates.end());

	int prevK=k;

	if(hull->size()==1)
		k=k*2;   // bezveze 

	for(int i=0, j=0; i < candidates.size(); i++)
	{
		if(i>=k && kNearestIndices.size()>0 )
			break;

		if( IsKVecGood(candidates[i].index))
		{
			kNearestIndices.push_back(candidates[i].index);
		}
	}

	k=prevK;

	if(!kNearestIndices.size())
		return false;

	return true;
}

bool CConcaveHullGenerator::IsKVecGood(int _pointIndex)
{
	if(hull->size()<3)
		return true;

	int length = hull->size()-3;

	if(_pointIndex != firstPointIndex)
		length = hull->size()-2;

	CLineSegment potentialNewLine(*(*points)[_pointIndex], *(*hull)[hull->size()-1]);
	CLineSegment testLine;
	for(int i=0; i< length; i++)
	{
		testLine.Set(*(*hull)[hull->size()-2-i],  *(*hull)[hull->size()-3-i]);
		if(F::INTERSECTION::LineSegmentIntersection(potentialNewLine, testLine ))
			return false;
	}

	return true;
}

bool CConcaveHullGenerator::CalculateNextHullPoint(const CVector &a)
{
	CVector *b;
	double angleMax=0;
	double currentAngle;
	int indexMax;

	for(int i=0; i < kNearestIndices.size(); i++)
	{
		b = new CVector(( *(*points)[kNearestIndices[i]]) - *(*hull)[hull->size()-1]);
		currentAngle = F::ANGLE::GetCWAFromV1toV2(a,*b);

		if(angleMax < currentAngle)
		{
			angleMax = currentAngle;
			indexMax = kNearestIndices[i];
		}
		else
			delete b;
	}

	kNearestIndices.clear();

	if(indexMax==firstPointIndex)
		return false;

	hull->push_back((*points)[indexMax]);
	addedPointIndices.push_back(indexMax);
	return true;
}

void CConcaveHullGenerator::AddAllChainedClusterPoints(CPointCluster *_pCluster)
{
	points = _pCluster->points;
	
	CPointCluster *nextPC = _pCluster;

	while(nextPC=nextPC->next)
	{
		points->insert(points->end(), nextPC->points->begin(), nextPC->points->end());
	}
}