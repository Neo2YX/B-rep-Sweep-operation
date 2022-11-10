#pragma once
#include "glm/glm.hpp"
class HalfEdge;


struct Coord {
	glm::vec3 position;

	Coord() : position(glm::vec3(0.0f)) {}
	Coord(glm::vec3 pos) : position(pos) {}
};

class Vertex {
public:
	HalfEdge* vHEdge;
	Coord Pos;

	Vertex() :  vHEdge(nullptr), Pos() {}
	Vertex(Coord pos) :  vHEdge(nullptr), Pos(pos) {}
};