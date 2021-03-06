#include "f_angle.h"
#include <math.h>
#include "f_distance.h"

double F::ANGLE::GetAngleBetweenTwoSidesOfATriangle(double _s1, double _s2, double _s3)
{
	double cosOfAngle = -(_s3*_s3 - (_s1*_s1) - (_s2*_s2))/(2*_s2*_s1);
	return acos(cosOfAngle);
}

double F::ANGLE::GetCWAFromV1toV2(const CVector &_v1, const CVector &_v2)
{
	double angle = atan2(_v1^_v2, _v1*_v2);

	if(angle < 0)
		angle += 2*PI;

	angle = 2*PI - angle;

	return angle;
}

double F::ANGLE::GetSmallerAngleBetweenTwoVectors(const CVector &_a, const CVector &_b)
{
	return GetAngleBetweenTwoSidesOfATriangle(_a.Length(), _b.Length(), F::DISTANCE::GetDistance(_a,_b));
}

double F::ANGLE::DegToRad(double _angle)
{
	return _angle*PI/180;
}

double F::ANGLE::RadToDeg(double _angle)
{
	return  _angle*180/PI;
}