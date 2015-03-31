#pragma once

//enum {EXPLORE_DIRECTION,  };

#include "AIBehaviour.h"

class CAIState
{
public:
	int STATE_ID;
	CAIBehaviour *behaviour;
	virtual bool Run(){return true;};
	////
	virtual void Draw(){};
};