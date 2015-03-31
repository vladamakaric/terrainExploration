#pragma once
#include "Vector.h"

class CCamera
{
public:
	CVector offset;
	int cameraThreshold;
	double cameraSpeed;
	double zoom;
	double prevZoom;

//public:
	CCamera(){};
	void Init(double _x, double _y, int _cameraThreshold, int _cameraSpeed);
	void Update(int _screenWidth, int _screenHeight, int _mapWidth, int _mapHeight);
	void Events();
	void Apply();
};