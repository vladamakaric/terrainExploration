#pragma once
#include <queue>
using namespace std;


#include "ConsciousEntity.h"

class CAIState;
class CAIBehaviour
{
public:
	CAIState *currentState;
	CAIState *newState;
	queue<CAIState*> stateQueue;
	CConsciousEntity *CE;

	void ChangeState(int _STATE_ID);
	void PopState();
	virtual void Run(){};
	virtual void Draw(){};
};