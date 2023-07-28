#include "OBB.h"

OBB::OBB()
{
}

OBB* OBB::Create()
{
	OBB* instance = new OBB();

	instance->pos = Vector3(0, 0, 0);

	instance->normalVector[0] = { 1,0,0 };
	instance->normalVector[1] = { 0,1,0 };
	instance->normalVector[2] = { 0,0,1 };

	instance->length[0] = 1;
	instance->length[1] = 1;
	instance->length[2] = 1;

	return instance;
}
