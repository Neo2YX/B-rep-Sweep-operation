#pragma once

class Face;
class HalfEdge;

class Loop
{
public:
	Loop* lPrev;
	Loop* lNext;
	HalfEdge* lHEdge;
	Face* lFace;
	bool IsInnerLoop;

	Loop() : lPrev(nullptr), lNext(nullptr), lHEdge(nullptr), lFace(nullptr), IsInnerLoop(false) {}
	Loop(bool flag) : lPrev(nullptr), lNext(nullptr), lHEdge(nullptr), lFace(nullptr), IsInnerLoop(flag) {}
};
