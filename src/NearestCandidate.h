#pragma once

class CNearestCandidate
{
public:
	int index;
	double value;
	void *data;

	CNearestCandidate(int _indx, double _val, void *_p = 0) : index(_indx), value(_val), data(_p) {}
	bool operator<(CNearestCandidate _a) { return value < _a.value; }
};