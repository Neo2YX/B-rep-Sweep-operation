#pragma once

#include "Solid.h"
#include "Face.h"
#include "Loop.h"
#include "Edge.h"
#include "HalfEdge.h"
#include "Vertex.h"

class HEDS
{
public:
	//Euler operation
	static Solid* mvfs(Coord pos, Vertex*& vertex);
	static HalfEdge* mev(Vertex* v1, Coord pos2, Loop* loop);
	static Loop* mef(Vertex* v1, Vertex* v2, Loop* loop);
	static Loop* kemr(Vertex* v1, Vertex* v2, Loop* loop);
	static void kfmrh(Face* deleteFace, Face* face);

	//sweep
	static Solid* sweep(Face* face, glm::vec3 sweepVector);

	//Debug
	static void PrintSolid(Solid* solid);


	HEDS()  {}
};