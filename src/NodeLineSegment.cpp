#include "NodeLineSegment.h"
#include "f_distance.h"

double CNodeLineSegment::Lenght() const
{ 
	return F::DISTANCE::GetDistance(down->position, up->position); 
}