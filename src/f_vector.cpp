#include "f_vector.h"

bool F::VECTOR::VectorBetweenTwoVectors(CVector &_v, CVector &_v1, CVector &_v2)
{
	CVector a = _v1.Direction();
	CVector b = _v.Direction();
	CVector c = _v2.Direction();

	return a*b>a*c && b*c>a*c;
}

double F::VECTOR::GetLenghtOfVectorProjectionAToB( const CVector& _a,const CVector& _b )
{
	return fabs(((_a*_b)/_b.Length()));
}

double F::VECTOR::GetLenghtSqOfVectorProjectionAToB( const CVector& _a,const CVector& _b )
{
	double dotP = (_a*_b);
	return fabs((dotP*dotP/_b.LengthSq()));
}

CVector F::VECTOR::GetVectorProjectionAToB(const CVector& _a, const CVector& _b)
{
	return _b.Direction() * GetLenghtOfVectorProjectionAToB(_a,_b);
}

CVector F::VECTOR::GetVectorNormalFromAToB(const CVector& _a,const CVector& _b)
{
	return GetVectorFromAToB(GetVectorProjectionAToB(_b, _a), _b);
}

CVector F::VECTOR::GetVectorFromAToB(const CVector& _a, const CVector& _b)
{
	return _b - _a;
}

CVector F::VECTOR::GetVectorProjectionOnLineSegment( const CVector& _v, const CLineSegment& _ls )
{
	CVector LSDirection = GetVectorFromAToB(_ls.a,_ls.b);
	CVector projection = GetVectorProjectionAToB(_v,LSDirection);

	if(_v*LSDirection < 0)
		projection*=-1;

	return projection;
}