#pragma once

class CBaseEntity
{
public:
	int ID;
	static int IDCounter;
	CBaseEntity() : ID(IDCounter++) {}
	bool operator ==(const CBaseEntity &_be) const { return _be.ID == ID; }
	bool operator !=(const CBaseEntity &_be) const { return _be.ID != ID; }
};