#pragma once
#include "../HalfEdgeClass/HEDS.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
struct Triangle {
	glm::vec3 vertex[3];
	glm::vec3 normal[3];
};

class Renderer
{
private:
	Solid* solid;
	GLFWwindow* window;
public:
	Renderer();
	void SetSolid(Solid* s) { solid = s; }
	void Draw();
	std::vector<Triangle> triangles;
	static const int WIDTH = 1080;
	static const int HEIGHT = 720;
	bool IsWireFrame;
	bool IsLit;
private:
	void GenTrianglies();
	void Poly2Triangles(Face* face);
};