#include "HEDS.h"
#include <iostream>

/*-------------------------Euler operation----------------------*/
//mvfs: construct a new Solid, Face, Loop, Vertex
Solid* HEDS::mvfs(Coord pos, Vertex*& vertex)
{
	Solid* solid = new Solid();
	Face* face = new Face();
	Loop* loop = new Loop();
	vertex = new Vertex(pos);

	solid->sFace = face;
	solid->sFace->fPrev = solid->sFace->fNext = face;
	face->fSolid = solid;
	face->fLoops = loop;
	face->fLoops->lPrev = face->fLoops->lNext = loop;
	loop->lFace = face;
	
	return solid;
}
//mev: construct a new vertex, edge with a given vertex
HalfEdge* HEDS::mev(Vertex* v1, Coord pos2, Loop* loop)
{
	HalfEdge* he1 = new HalfEdge(); //he1 - from v1 to v2(pos2)
	HalfEdge* he2 = new HalfEdge(); //he2 - from v2(pos2) to v1
	Edge* e = new Edge();
	Vertex* vertex = new Vertex(pos2);

	e->he1 = he1;
	e->he2 = he2;
	he1->heEdge = he2->heEdge = e;
	he1->twin = he2;
	he2->twin = he1;

	he1->origin = he2->destination = v1;
	he1->destination = he2->origin = vertex;

	he1->heLoop = he2->heLoop = loop;
	he1->Next = he2;
	he2->Prev = he1;
	//check the given loop
	if (loop->lHEdge == nullptr) {
		//the given loop is empty
		he2->Next = he1;
		he1->Prev = he2;
		loop->lHEdge = he1;
	}
	else {
		//find the origin of new halfedge he1(v1)
		HalfEdge* he = loop->lHEdge;
		while (he->destination != v1) {
			he = he->Next;
			if (he == loop->lHEdge) {
				std::cout << "[Eular operation error]: mev  can't find the given vertex in the given loop" << std::endl;
				exit(-1);
			}
		}
		he1->Prev = he;
		he2->Next = he->Next;
		he->Next = he1;
		he2->Next->Prev = he2;
	}

	//update the edge of solid
	Solid* solid = loop->lFace->fSolid;
	if (solid->sEdge) {
		Edge* e1 = solid->sEdge;
		Edge* e2 = e1->eNext;
		e->eNext = e2;
		e->ePrev = e1;
		e2->ePrev = e;
		e1->eNext = e;
	}
	else {
		solid->sEdge = e;
		e->ePrev = e->eNext = e;
	}

	return he1;
}
//mef: construct a edge, face, loop with two given vertex on the loop
//newLoop - v2->v1  loop - v1->v2
Loop* HEDS::mef(Vertex* v1, Vertex* v2, Loop* loop)
{
	HalfEdge* he1 = new HalfEdge();
	HalfEdge* he2 = new HalfEdge();
	Edge* e = new Edge();

	e->he1 = he1;
	e->he2 = he2;
	he1->heEdge = he2->heEdge = e;
	he1->twin = he2;
	he2->twin = he1;
	he1->origin = he2->destination = v1;
	he2->origin = he1->destination = v2;

	Loop* newLoop = new Loop();
	//find two given vertex in the loop
	HalfEdge* t_he1, *t_he2, *t_he;
	t_he = loop->lHEdge;
	while (t_he->origin != v1) t_he = t_he->Next;
	t_he1 = t_he;
	while (t_he->origin != v2) t_he = t_he->Next;
	t_he2 = t_he;

	he1->Next = t_he2;
	he1->Prev = t_he1->Prev;
	he2->Next = t_he1;
	he2->Prev = t_he2->Prev;
	t_he1->Prev->Next = he1;
	t_he1->Prev = he2;
	t_he2->Prev->Next = he2;
	t_he2->Prev = he1;

	newLoop->lHEdge = he2;
	loop->lHEdge = he1;

	//update the halfedge in the loop
	HalfEdge* he = newLoop->lHEdge;
	do {
		he->heLoop = newLoop;
		he = he->Next;
	} while (he != newLoop->lHEdge);
	he = loop->lHEdge;
	do {
		he->heLoop = loop;
		he = he->Next;
	} while (he != loop->lHEdge);

	//construct a face of the loop & update solid
	Face* face = new Face();
	Solid* solid = loop->lFace->fSolid;
	face->fLoops = newLoop;
	newLoop->lPrev = newLoop->lNext = newLoop;
	newLoop->lFace = face;
	face->fSolid = solid;
	//add face to the solid
	if (solid->sFace) {
		//the solid have face before
		Face* f1 = solid->sFace;
		Face* f2 = f1->fNext;
		face->fPrev = f1;
		face->fNext = f2;
		f1->fNext = face;
		f2->fPrev = face;
	}
	else {
		solid->sFace = face;
		face->fNext = face->fPrev = face;
	}
	//add edge to the solid
	if (solid->sEdge) {
		Edge* e1 = solid->sEdge;
		Edge* e2 = e1->eNext;
		e->eNext = e2;
		e->ePrev = e1;
		e2->ePrev = e;
		e1->eNext = e;
	}
	else {
		solid->sEdge = e;
		e->ePrev = e->eNext = e;
	}

	return newLoop;
}
//kemr: delete a edge, and make an inner loop
//v1 is on the inner loop, v2 is on the outer loop
Loop* HEDS::kemr(Vertex* v1, Vertex* v2, Loop* loop)
{
	Loop* newLoop = new Loop(true);
	HalfEdge* he = loop->lHEdge;
	while (he->origin != v1 || he->destination != v2) he = he->Next;
	Edge* deleteEdge = he->heEdge;
	HalfEdge* adjHEdge = he->twin;

	he->Prev->Next = adjHEdge->Next;
	he->Next->Prev = adjHEdge->Prev;
	adjHEdge->Next->Prev = he->Prev;
	adjHEdge->Prev->Next = he->Next;

	newLoop->lHEdge = he->Prev;
	loop->lHEdge = he->Next;
	//update the halfedge in the loop
	HalfEdge* _he = newLoop->lHEdge;
	do {
		_he->heLoop = newLoop;
		_he = _he->Next;
	} while (_he != newLoop->lHEdge);

	//add loop to face
	Face* face = loop->lFace;
	newLoop->lFace = face;
	Loop* l1 = face->fLoops;
	Loop* l2 = l1->lNext;
	l1->lNext = newLoop;
	l2->lPrev = newLoop;
	newLoop->lNext = l2;
	newLoop->lPrev = l1;
	//delete Edge
	Solid* solid = face->fSolid;
	Edge* e = solid->sEdge;
	while (e != deleteEdge) e = e->eNext;
	Edge* e1 = e->ePrev;
	Edge* e2 = e->eNext;
	e1->eNext = e2;
	e2->ePrev = e1;
	delete e;
	delete he;
	delete adjHEdge;

	return newLoop;
}
//kfmrh: delete a face and make the face into the inner loop of another face
void HEDS::kfmrh(Face* deleteFace, Face* face)//deleteFace should only have one outer loop
{
	if (deleteFace->fLoops == nullptr || deleteFace->fLoops != deleteFace->fLoops->lNext) {
		std::cout << "[Eular operation error]: kfmrh - the given deleteFace have more than one loop" << std::endl;
		return;
	}
	deleteFace->fLoops->IsInnerLoop = true;
	//add inner loop to face
	Loop* loop = deleteFace->fLoops;
	loop->lFace = face;
	Loop* l1 = face->fLoops;
	Loop* l2 = l1->lNext;
	l1->lNext = loop;
	l2->lPrev = loop;
	loop->lNext = l2;
	loop->lPrev = l1;

	//delete the face
	Solid* solid = deleteFace->fSolid;
	Face* sFace = solid->sFace;
	while (sFace != deleteFace) {
		sFace = sFace->fNext;
		if (sFace == solid->sFace) {
			std::cout << "[Eular operation error]: kfmrh - can't find deleteFace in the solid" << std::endl;
			return;
		}
	}
	if (sFace->fNext == sFace) {
		delete sFace;
		sFace = nullptr;
	}
	else {
		Face* f1 = sFace->fNext;
		Face* f2 = sFace->fPrev;
		f1->fPrev = f2;
		f2->fNext = f1;
		delete sFace;
	}
}
/*-------------------sweep-----------------------------------*/
//just by a face with a vector, to creat a solid
Solid* HEDS::sweep(Face* face, glm::vec3 sweepVector)
{
	Solid* solid = face->fSolid;
	
	Loop* loop = face->fLoops;
	/*
	tranvel the face with the loop
	for every vertex, construct a new vertex and a new Edge (mev)
	for every edge, construct a new edge and a new face (mef)
	for every face, construct a new face
	*/
	do {//tranvel every loop
		HalfEdge* he = loop->lHEdge;
		Vertex* firstVertex = he->origin;
		Coord newPos = (firstVertex->Pos).position + sweepVector;
		HalfEdge* firstUpHEdge = mev(firstVertex, newPos, loop);
		Vertex* LastUpVertex = firstUpHEdge->destination;

		he = he->Next;
		Vertex* currentVertex = he->origin;
		while (currentVertex != firstVertex) {//every point can construct a new vertex, a new edge and a new face with 
			Coord newPos = (currentVertex->Pos).position + sweepVector;
			HalfEdge* UpHEdge = mev(currentVertex, newPos, loop);
			Vertex* currentUpVertex = UpHEdge->destination;
			mef(LastUpVertex, currentUpVertex, loop);
			LastUpVertex = currentUpVertex;
			he = he->Next;
			currentVertex = he->origin;
		}
		mef(LastUpVertex, firstUpHEdge->destination, loop);

		loop = loop->lNext;

	} while (loop != face->fLoops);

	return solid;
}

/*---------------------------------DEBUG---------------------------*/
void HEDS::PrintSolid(Solid* solid)
{
	std::cout << "solid: tranvel by Face" << std::endl;
	Face* face = solid->sFace;
	int faceNum = 1;
	do {
		std::cout << "Face" << faceNum << ": " << std::endl;
		Loop* loop = face->fLoops;
		int loopNum = 1;
		do {
			std::cout << "	Loop" << loopNum << ": " << std::endl;
			HalfEdge* he = loop->lHEdge;
			do {
				std::cout << "		" << (he->origin->Pos).position.x << " " << (he->origin->Pos).position.y << " " << (he->origin->Pos).position.z << " " << "		" << (he->destination->Pos).position.x << " " << (he->destination->Pos).position.y << " " << (he->destination->Pos).position.z << " " << std::endl;
				he = he->Next;
			} while (he != loop->lHEdge);
			loopNum++;
			loop = loop->lNext;
		} while (loop != face->fLoops);
		faceNum++;
		face = face->fNext;
	} while (face != solid->sFace);
	Edge* e = solid->sEdge;
	int i = 0;
	do {
		i++;
		e = e->eNext;
	} while (e != solid->sEdge);
	std::cout << "Edge Num: " << i << std::endl;
}