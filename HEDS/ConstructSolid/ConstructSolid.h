#pragma once
#include "../HalfEdgeClass/HEDS.h"
#include <vector>

class ConstructSolid
{
	Solid* solid;
	std::vector<Vertex*> vertices;
	char* FilePath;
public:
	ConstructSolid() : solid(nullptr), FilePath(nullptr) {}
	void constructFromFile(const char* path); //only support one face input and auto construct a solid with
	Solid* GetSolid() const { return solid; }
	//show case
	Solid* CreateCube();
	Solid* CreateZero();

	void swap(Loop*& l1, Loop*& l2) {
		Loop* l = l1;
		l1 = l2;
		l2 = l;
	}
};