#ifndef DRAW_H
#define DRAW_H

#include "Vector.h"
#include "global.h"
#include "Capsule.h"

namespace Draw
{
	inline void PushVertex(const CVector &_v) { glVertex2d(_v.x,_v.y); }
	inline void PushVertex(double _x, double _y) { glVertex2d(_x,_y); }
	void Border(int _width, int _height, double _thickness, const CVector &_pos);
	void CircleFill(double _x, double _y,double _HRadius, int _vertices);
	void CircleOutline(double _x, double _y,double _HRadius, int _vertices);
	void CircleFill(const CVector &_v, double _HRadius, int _vertices);
	void CircleOutline(const CVector &_v, double _HRadius, int _vertices);
	void PushCircle(double _x, double _y,double _HRadius, int _vertices);
	void Line(CVector _a, CVector _b);

	template<class T> void VectorObjectContainerAsConnectedOutlinedCapsules(T &_oContainer, double _r)
	{
		if(!_oContainer.size())
			return;

		auto limit = _oContainer.end();
		limit--;
		decltype(limit) tempIter;
		CCapsule c;
		c.r = _r;

		for(auto iter = _oContainer.begin(); iter!=limit; iter++)
		{
			tempIter = iter;
			tempIter++;
			c.SetCenter(*tempIter, *iter);
			c.DrawOutline();
		}
	}

	template<class T> void VectorObjectContainerAsConnectedFilledCapsules(T &_oContainer, double _r)
	{
		if(!_oContainer.size())
			return;

		typename T::iterator limit = _oContainer.end();
		limit--;
		typename T::iterator tempIter;
		CCapsule c;
		c.r = _r;

		for(typename T::iterator iter = _oContainer.begin(); iter!=limit; iter++)
		{
			tempIter = iter;
			tempIter++;
			c.SetCenter(*tempIter, *iter);
			c.DrawFill();
		}
	}

	template<class T> void VectorObjectContainerAsOpenLine(T &_oContainer)
	{
		if(!_oContainer.size())
			return;

		typename T::iterator limit = _oContainer.end();
		limit--;
		typename T::iterator tempIter;


		for(typename T::iterator iter = _oContainer.begin(); iter!=limit; iter++)
		{
			tempIter = iter;
			tempIter++;

			Line(*iter, *tempIter);
		}
	}

	template<class T> void PointerToObjectWithPositionVector_Container_AsOpenLine(T &_oContainer)
	{
		if(!_oContainer.size())
			return;

		typename T::iterator limit = _oContainer.end();
		limit--;
		typename T::iterator tempIter;


		for(auto iter = _oContainer.begin(); iter!=limit; iter++)
		{
			tempIter = iter;
			tempIter++;

			Line((*iter)->position, (*tempIter)->position);
		}
	}
}

#endif
