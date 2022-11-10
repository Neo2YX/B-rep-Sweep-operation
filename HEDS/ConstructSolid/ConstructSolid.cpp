#include "ConstructSolid.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void ConstructSolid::constructFromFile(const char* path)
{
	std::ifstream input(path);
	if (!input.is_open()) {
		std::cout << "[Construct Solid Error]: constructFromFile - failed to reaed the file!" << std::endl;
		exit(-1);
	}
	std::stringstream ss;
	std::string line;
	//get the face information (loopNum, vertexNum)
	std::getline(input, line);
	int loopNum;
	ss << line;
	ss >> loopNum;
	int* vertexNum = (int*)malloc(sizeof(int) * loopNum);
	for (int i = 0; i < loopNum; ++i) ss >> vertexNum[i];
	ss.clear();
	ss.str("");

	//main loop
	Vertex* firstVertex = nullptr;
	Vertex* lastVertex = nullptr;
	Loop* topLoop = nullptr;
	Loop* downLoop;
	for (int i = 0; i < vertexNum[0]; ++i) {
		std::getline(input, line);
		ss << line;
		float x, y, z;
		ss >> x >> y >> z;
		if (!i) {
			solid = HEDS::mvfs(glm::vec3(x, y, z), firstVertex);
			topLoop = solid->sFace->fLoops;
			lastVertex = firstVertex;
		}
		else {
			HalfEdge* tmp = HEDS::mev(lastVertex, glm::vec3(x, y, z), topLoop);
			lastVertex = tmp->destination;
		}
		ss.clear();
		ss.str("");
	}
	downLoop = HEDS::mef(lastVertex, firstVertex, topLoop);

	//inner loop
	Loop* downInnerLoop = nullptr;
	Loop* topInnerLoop = nullptr;
	for (int i = 1; i < loopNum; ++i) {
		lastVertex = firstVertex;
		Vertex* innerFirstVertex = nullptr;
		for (int j = 0; j < vertexNum[i]; ++j) {
			std::getline(input, line);
			ss << line;
			float x, y, z;
			ss >> x >> y >> z;
			HalfEdge* tmp = HEDS::mev(lastVertex, glm::vec3(x, y, z), topLoop);
			lastVertex = tmp->destination;
			if (!j) innerFirstVertex = lastVertex;
			ss.clear();
			ss.str("");
		}
		downInnerLoop = HEDS::mef(lastVertex, innerFirstVertex, topLoop);
		topInnerLoop = HEDS::kemr(innerFirstVertex, firstVertex, topLoop);
		HEDS::kfmrh(downInnerLoop->lFace, downLoop->lFace);
	}

	//sweep
	std::getline(input, line);
	ss << line;
	float x, y, z;
	ss >> x >> y >> z;
	HEDS::sweep(downLoop->lFace, glm::vec3(x, y, z));
}

Solid* ConstructSolid::CreateCube()
{
	Solid* solid;
	Vertex* top_upper_left;
	solid = HEDS::mvfs(glm::vec3(-1.0, 0.5, 2.0), top_upper_left);
	Loop* top_loop = solid->sFace->fLoops;

	HalfEdge* top_upper_halfedge = HEDS::mev(top_upper_left, glm::vec3(1.0, 0.5, 2.0), top_loop);
	Vertex* top_upper_right = top_upper_halfedge->destination;

	HalfEdge* top_left_halfedge = HEDS::mev(top_upper_left, glm::vec3(-1.0, 0.5, -2.0), top_loop);
	Vertex* top_bottom_left = top_left_halfedge->destination;

	HalfEdge* top_right_halfedge = HEDS::mev(top_upper_right, glm::vec3(1.0, 0.5, -2.0), top_loop);
	Vertex* top_bottom_right = top_right_halfedge->destination;

	Loop* down_loop = HEDS::mef(top_bottom_left, top_bottom_right, top_loop);

	HEDS::sweep(down_loop->lFace, glm::vec3(0, -1, 0));

	this->solid = solid;

	return solid;
}

Solid* ConstructSolid::CreateZero()
{
	Solid* solid;
	Vertex* top_upper_left;
	solid = HEDS::mvfs(glm::vec3(-1.0, 0.5, 2.0), top_upper_left);
	Loop* top_loop = solid->sFace->fLoops;

	HalfEdge* top_upper_halfedge = HEDS::mev(top_upper_left, glm::vec3(1.0, 0.5, 2.0), top_loop);
	Vertex* top_upper_right = top_upper_halfedge->destination;

	HalfEdge* top_left_halfedge = HEDS::mev(top_upper_left, glm::vec3(-1.0, 0.5, -2.0), top_loop);
	Vertex*  top_bottom_left = top_left_halfedge->destination;

	HalfEdge* top_right_halfedge = HEDS::mev(top_upper_right, glm::vec3(1.0, 0.5, -2.0), top_loop);
	Vertex* top_bottom_right = top_right_halfedge->destination;

	Loop* down_loop = HEDS::mef(top_bottom_left, top_bottom_right, top_loop);
	
	
	HalfEdge* top_bridge = HEDS::mev(top_upper_left, glm::vec3(-0.5, 0.5, 1.5), top_loop);
	Vertex* top_inner_front_left = top_bridge->destination;

	HalfEdge* top_inner_left_halfedge = HEDS::mev( top_inner_front_left, glm::vec3(-0.5, 0.5, -1.5), top_loop);
	Vertex* top_inner_back_left = top_inner_left_halfedge->destination;

	HalfEdge* top_inner_back_halfedge = HEDS::mev(top_inner_back_left, glm::vec3(0.5, 0.5, -1.5), top_loop);
	Vertex* top_inner_back_right = top_inner_back_halfedge->destination;

	HalfEdge* top_inner_right_halfedge = HEDS::mev(top_inner_back_right, glm::vec3(0.5, 0.5, 1.5), top_loop);
	Vertex* top_inner_front_right = top_inner_right_halfedge->destination;
	
	//change v1 and v2's order to return the main_loop
	Loop* down_inner_loop = HEDS::mef(top_inner_front_left, top_inner_front_right, top_loop);
	
	Loop* top_inner_loop = HEDS::kemr(top_upper_left, top_inner_front_left, top_loop);
	
	//swap(top_loop, top_inner_loop);
	HEDS::kfmrh(down_inner_loop->lFace, down_loop->lFace);
	solid = HEDS::sweep(down_loop->lFace, glm::vec3(0.0, -1.0, 0.0));

	this->solid = solid;
	return solid;
}