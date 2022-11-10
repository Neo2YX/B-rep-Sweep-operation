#pragma once
class Face;
class Edge;

class Solid {
public:
	Solid* sPrev;
	Solid* sNext;
	Face* sFace;
	Edge* sEdge;

	Solid() : sPrev(nullptr), sNext(nullptr), sFace(nullptr), sEdge(nullptr) {}
};