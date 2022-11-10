#pragma once

class Loop;
class Solid;

class Face {
public:
	Face* fPrev;
	Face* fNext;
	Loop* fLoops;
	Solid* fSolid;

	Face() : fPrev(nullptr), fNext(nullptr), fLoops(nullptr), fSolid(nullptr) {}
};