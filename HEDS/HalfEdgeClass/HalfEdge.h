#pragma once

class Vertex;
class Loop;
class Edge;

class HalfEdge {
public:
	HalfEdge* Prev;
	HalfEdge* Next;
	HalfEdge* twin;
	Vertex* origin;
	Vertex* destination;
	Loop* heLoop;
	Edge* heEdge;

	HalfEdge() : Prev(nullptr), Next(nullptr), twin(nullptr), origin(nullptr), destination(nullptr), heLoop(nullptr), heEdge(nullptr) {}
};