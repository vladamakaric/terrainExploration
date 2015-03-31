#ifndef F_MISC_H
#define F_MISC_H

#include "NearestCandidate.h"
#include "Vector.h"
#include "Node.h"
#include "FAGTile.h"
#include "Circle.h"
#include "PathFindingNode.h"

#include <vector>

using namespace std;


class CLine;
namespace F
{
	namespace MISC
	{
		template<class T> inline T MinByRef(T& _a,T& _b)  { return _a > _b ? _b : _a; }
		template<class T> inline T MaxByRef(T& _a,T& _b)  { return _a > _b ? _a : _b; }
		template<class T> inline T Min(T _a,T _b)  { return _a > _b ? _b : _a; }
		template<class T> inline T Max(T _a,T _b)  { return _a > _b ? _a : _b; }

		template<class T> inline void Swap(T &_a, T &_b) 
		{
			T temp = _a;
			_a = _b;
			_b = temp;
		}

		template<class T> void TagObjectContainer(T &_oContainer)
		{
			for(auto iter = _oContainer.begin(); iter!=_oContainer.end(); iter++)
				iter->tagged = true;
		}

		template<class T> void UntagObjectContainer(T &_oContainer)
		{
			for(auto iter = _oContainer.begin(); iter!=_oContainer.end(); iter++)
				iter->tagged = false;
		}

		template<class T> void UntagPointerContainer(T &_pContainer)
		{
			for(int i=0; i < _pContainer.size(); i++)
				_pContainer[i]->tagged=false;
		}

		template<class T> void TagPointerContainer(T &_pContainer)
		{
			for(int i=0; i < _pContainer.size(); i++)
				_pContainer[i]->tagged=true;
		}
			
		CNearestCandidate* GetSmallestValueCandidate(vector<CNearestCandidate> &candidates);
		void CalculateCircleFrom3Points(const CVector& _p1, const CVector &_p2,const CVector &_p3, CCircle& _c);
		void DrawBorder(int _width, int _height, double _thickness);
		void GetUniqueNodeLineSegmentsFromFAGTiles(vector<CNodeLineSegment*> &_uNodeLineSegments, vector<CFAGTile*> &_fagTiles);
		void GetLeftRightTopBottomFrom2Vecs(const CVector &_a, const CVector &_b, double &right, double &left,  double &top, double &bottom);
		void GetLeftRightTopBottomFromLineSegment(const CLineSegment &_ls, double &right, double &left,  double &top, double &bottom);

		void PathFindingNodePathToVectorPath(vector<CPathFindingNode*> &PFNPath, vector<CVector> &vectorPath);
		bool IsCircleOutOfMap(int _HR, const CVector &_v, int _mHeight, int _mWidth , int _border = 0 );
		bool AddIntersection(double _upper, double _lower, double _y);
		bool ValueBetweenValues(double _v1, double _v2, double _value );
		bool TwoPointsOnSameSideOfLine(const CVector &_p1, const CVector &_p2, const CLine &_l);
	}
}
#endif
