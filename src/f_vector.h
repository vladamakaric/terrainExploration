#pragma once
#include "Vector.h"
#include "LineSegment.h"

namespace F
{
	namespace VECTOR
	{
		CVector GetVectorFromAToB(const CVector& _a,const CVector& _b);
		CVector GetVectorProjectionAToB(const CVector& _a,const CVector& _b);
		CVector GetVectorNormalFromAToB(const CVector& _a,const CVector& _b);
		CVector GetVectorProjectionOnLineSegment(const CVector& _v, const CLineSegment& _ls);
		double GetLenghtOfVectorProjectionAToB(const CVector& _a,const CVector& _b);
		double GetLenghtSqOfVectorProjectionAToB(const CVector& _a,const CVector& _b);
		bool VectorBetweenTwoVectors(CVector &_v, CVector &_v1, CVector &_v2);
	}
}