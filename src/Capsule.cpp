#include "Capsule.h"
#include "f_vector.h"
#include "draw.h"
#include "PrecompiledShapes.h"


void CCapsule::UpdateSides()
{
	CVector sideLinesOffset = F::VECTOR::GetVectorFromAToB(center.a, center.b).Direction().perp() * r;
	sides[0].a = center.a + sideLinesOffset;
	sides[0].b = center.b + sideLinesOffset;
	sides[1].a = center.a - sideLinesOffset;
	sides[1].b = center.b - sideLinesOffset;
}

void CCapsule::DrawOutline()
{
	Draw::CircleOutline(center.b, r, 40);
	Draw::CircleOutline(center.a, r, 40);
	Draw::Line(sides[0].a ,sides[0].b);
	Draw::Line(sides[1].a ,sides[1].b);
}

void CCapsule::DrawFill()
{
	CPCShapes::DrawShapeScaled(CPCShapes::circle100x100, r/100, center.a);
	CPCShapes::DrawShapeScaled(CPCShapes::circle100x100, r/100, center.b);
	//Draw::Line(sides[0].a ,sides[0].b);
	//Draw::Line(sides[1].a ,sides[1].b);

	glBegin(GL_QUADS);
	Draw::PushVertex(sides[0].a);
	Draw::PushVertex(sides[0].b);
	Draw::PushVertex(sides[1].b);
	Draw::PushVertex(sides[1].a);
	glEnd();
}
