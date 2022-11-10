#pragma once

class HalfEdge;

class Edge
{
public:
	Edge* ePrev;
	Edge* eNext;
	HalfEdge* he1;
	HalfEdge* he2;

	Edge() : ePrev(nullptr), eNext(nullptr), he1(nullptr), he2(nullptr) {}
};