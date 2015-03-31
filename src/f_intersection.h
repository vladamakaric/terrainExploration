#pragma once
#include "Vector.h"
#include "Line.h"
#include "LineSegment.h"
#include "Circle.h"
#include "FAGTile.h"
#include "NodeLineSegment.h"
#include "Square.h"
#include "Capsule.h"

namespace F
{
	namespace INTERSECTION
	{

		int GetLineSegmentCapsuleIntersection(const CLineSegment &_ls, const CCapsule &_c, CVector *_interPoints);
		bool PointInCapsule(const CVector &_p, const CCapsule &_c);
		bool LineIntersectsCapsule(const CLine &_l, const CCapsule &_c);

		CVector GetLineIntersection(const CLine &_a, const CLine &_b);
		int GetLineCircleIntersection(const CLine &_l, const CCircle &_c, CVector *_interPoints, bool checkForIntersection = true);
		int GetLineSegmentCircleIntersection(const CLineSegment &_ls, const CCircle &_c, CVector *_interPoints, bool checkForIntersection = true);
		bool CircleOutOfSquare(const CCircle &_c, const CSquare &_s);
		bool CircleInSquare(const CCircle &_c, const CSquare &_s);
		bool PointInSquare(const CVector &_p, const CSquare &_s);
		bool PointInCircle(const CVector &_p, const CCircle &_c);
		bool PointInCircleWithRadiusOffset(const CVector &_p, const CCircle &_c, double _offset);
		bool NodeLineFAGTileIntersection(CNodeLineSegment &_nl, CFAGTile &_ftile);
		bool PointOnLineOnLineSegment(const CVector &_p, const CLineSegment &_ls);
		bool LineSegmentIntersection(const CLineSegment &_ls1, const CLineSegment &_ls2, CVector *_ip = 0);
		bool PointProjectionOnLineSegment(const CVector &_p, const CLineSegment &_ls);
		bool LineSegmentIntersectsCircle(const CLineSegment &_ls, const CCircle &_c);
		bool LineIntersectsCircle(const CLine &_l, const CCircle &_c);
		bool CircleIntersectsCircle(const CCircle &_c1, const CCircle &_c2);
	}
}