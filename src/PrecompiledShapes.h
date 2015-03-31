#pragma once
#include "Vector.h"
#include "global.h"

class CPCShapes //precompiled Shapes
{
public:
	static void Initialize();
	static void DrawShape( GLint shape, CVector& _position);
	static void DrawShapeScaled(GLint shape, double scale, CVector& _position);

	static GLint triangle100;
	static GLint triangle50;
	static GLint circle100x100;
};