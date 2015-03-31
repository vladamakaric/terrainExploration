#pragma once
#include "Vector.h"
#include "LineSegment.h"
#include "NodeLineSegment.h"

class CCircle
{
public:
	CCircle(){}
	CCircle(CVector _p, double _r) : r(_r), position(_p) {}

	CCircle(const CLineSegment &_ls) : position((_ls.a + _ls.b)/2), r(_ls.Lenght()) {}
	CCircle(const CNodeLineSegment &_nls) : position((_nls.down->position + _nls.up->position)/2), r(_nls.Lenght()) {}
	void Set(const CVector &_pos, double _r) { position = _pos; r=_r; }
	void Set(CLineSegment &_ls) {position=(_ls.a + _ls.b)/2; r=_ls.Lenght();}
	void Set(CNodeLineSegment &_nls) {position=(_nls.down->position + _nls.up->position)/2; r=_nls.Lenght();}
	double r;
	CVector position;
};