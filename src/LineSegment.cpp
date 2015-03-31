#include "LineSegment.h"
#include "f_distance.h"
#include "draw.h"

double CLineSegment::Lenght() const
{
	return F::DISTANCE::GetDistance(a,b);
}

CVector CLineSegment::MiddlePoint() const
{
	return (a+b)/2;
}

void CLineSegment::Draw(){
	Draw::Line(a,b);
}
