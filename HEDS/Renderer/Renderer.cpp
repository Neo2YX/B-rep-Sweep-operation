#include "glad/glad.h"
#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "../poly2tri/poly2tri.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


void processInput(GLFWwindow* window);
//glfwCallBackFunction
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

//camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = (float)Renderer::WIDTH / 2.0f, lastY = (float)Renderer::HEIGHT / 2.0f;
float lastFrame = 0.0f;
float deltaTime;
bool firstGetCursor = true;

class Plane2D {
public:
	glm::vec3 o;
	glm::vec3 x;
	glm::vec3 y;
	glm::vec3 z;
	Plane2D(Face* face);
	auto Space3DToPlane2D(const glm::vec3&)->glm::vec2;
	auto Plane2DToSpace3D(const glm::vec2&)->glm::vec3;
};
inline Plane2D::Plane2D(Face* face)
{
	HalfEdge* he = face->fLoops->lHEdge;
	auto head = he;
	glm::vec3 v1 = (he->origin->Pos).position;
	he = he->Next;
	glm::vec3 v2 = (he->origin->Pos).position;
	he = he->Next;
	this->o = v1;
	this->x = glm::normalize(v2 - v1);
	while (he != head) {
		glm::vec3 v3 = (he->origin->Pos).position;
		if (glm::length(glm::cross(v3 - v1, v3 - v2)) > 0.01f) {
			this->z = glm::normalize(glm::cross(x, v3 - v2));
			this->y = glm::normalize(glm::cross(z, x));
			break;
		}
	}
}
inline glm::vec2 Plane2D::Space3DToPlane2D(const glm::vec3& p)
{
	if (glm::dot(p - o, z) > 0.01f)
		std::cout << "ERROR! p is not on the plane! " << glm::dot(p - o, z) << std::endl;
	glm::vec3 vec = p - o;
	return glm::vec2(glm::dot(vec, x), glm::dot(vec, y));
}
inline glm::vec3 Plane2D::Plane2DToSpace3D(const glm::vec2& p)
{
	return o + p.x * x + p.y * y;
}

Renderer::Renderer() : solid(nullptr), IsWireFrame(false), IsLit(true)
{
	if (!glfwInit()) {
		std::cout << "Failed to init GLFW!" << std::endl;
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Solid Viewport", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD!" << std::endl;
		glfwTerminate();
		return ;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Renderer::Poly2Triangles(Face* face)
{
	Plane2D plane(face);
	std::vector<p2t::Point*> all_points;
	std::vector<p2t::Point*> out_points;
	std::vector<p2t::Point*> in_points;

	//outter loop
	Loop* outerLoop = face->fLoops;
	while (outerLoop->IsInnerLoop) outerLoop = outerLoop->lNext;
	glm::vec3 firstEdge = (outerLoop->lHEdge->destination->Pos).position - (outerLoop->lHEdge->origin->Pos).position;
	glm::vec3 secondEdge = (outerLoop->lHEdge->Next->destination->Pos).position - (outerLoop->lHEdge->Next->origin->Pos).position;
	glm::vec3 norm = glm::normalize(glm::cross(secondEdge, firstEdge));
	HalfEdge* he = outerLoop->lHEdge;
	do {
		glm::vec2 p = plane.Space3DToPlane2D((he->origin->Pos).position);
		p2t::Point* point = new p2t::Point(p.x, p.y);
		all_points.push_back(point);
		out_points.push_back(point);
		he = he->Next;
	} while (he != outerLoop->lHEdge);
	p2t::CDT cdt(std::move(out_points));

	//inner loop
	Loop* innerLoop = face->fLoops;
	do {
		if (!(innerLoop->IsInnerLoop)) {
			innerLoop = innerLoop->lNext;
			continue;
		}

		HalfEdge* he = innerLoop->lHEdge;
		do {
			glm::vec2 p = plane.Space3DToPlane2D((he->origin->Pos).position);
			p2t::Point* point = new p2t::Point(p.x, p.y);
			in_points.push_back(point);
			all_points.push_back(point);
			he = he->Next;
		} while (he != innerLoop->lHEdge);

		cdt.AddHole(std::move(in_points));
		innerLoop = innerLoop->lNext;
	} while (innerLoop != face->fLoops);

	cdt.Triangulate();

	auto tris = cdt.GetTriangles();
	for (auto tri : tris) {
		Triangle triangle;
		for (int i = 0; i < 3; i++) {
			auto _p = tri->GetPoint(i);
			triangle.vertex[i] = plane.Plane2DToSpace3D(glm::vec2(_p->x, _p->y));
			triangle.normal[i] = norm;
		}
		triangles.push_back(triangle);
	}
	for (auto p : all_points)
		delete p;
}
void Renderer::GenTrianglies()
{
	triangles.clear();
	Face* face = solid->sFace;
	do {
		Poly2Triangles(face);
		face = face->fNext;
	} while (face != solid->sFace);
}

void Renderer::Draw()
{
	bool SpotOn = false;
	GenTrianglies();

	glm::vec3 pointLightPos(1.2f, 1.0f, 2.0f);

	Shader WireFrameShader("res/shaders/WireFrameVertex.shader", "res/shaders/WireFrameFragment.shader");
	Shader SolidShader("res/shaders/SolidVertex.shader", "res/shaders/SolidFragment.shader");
	Shader LightingBoxShader("res/shaders/LightingBoxVertex.shader", "res/shaders/LightingBoxFragment.shader");
	Shader UnlitShader("res/shaders/UnlitVertex.shader", "res/shaders/UnlitFragment.shader");

	//light parameter
	SolidShader.Bind();
	//direction light
	SolidShader.SetVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	SolidShader.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	SolidShader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	SolidShader.SetVec3("dirLight.direction", -0.2f, -0.3f, -0.2f);
	//point light
	SolidShader.SetVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	SolidShader.SetVec3("pointLight.diffuse", 3.2f, 3.2f, 3.2f);
	SolidShader.SetVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
	SolidShader.SetFloat("pointLight.constant", 1.0f);
	SolidShader.SetFloat("pointLight.linear", 0.09f);
	SolidShader.SetFloat("pointLight.quadratic", 0.032f);
	//spot light
	SolidShader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	SolidShader.SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	SolidShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	SolidShader.SetFloat("spotLight.constant", 1.0f);
	SolidShader.SetFloat("spotLight.linear", 0.09f);
	SolidShader.SetFloat("spotLight.quadratic", 0.032f);
	SolidShader.SetFloat("spotLight.cutOff", (float)cos(glm::radians(17.5f)));
	SolidShader.SetFloat("spotLight.innerCutOff", (float)cos(glm::radians(12.5f)));
	//material - emerald
	//SolidShader.SetVec3("material.ambient", 0.0215f, 0.1745f, 0.0215f);
	SolidShader.SetVec3("material.ambient", 1.0f, 0.5f, 0.31f);
	//SolidShader.SetVec3("material.diffuse", 0.07568f, 0.61424f, 0.07568f);
	SolidShader.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
	//SolidShader.SetVec3("material.specular", 0.633f, 0.727811f, 0.633f);
	SolidShader.SetVec3("material.specular", 0.5f, 0.5f,0.5f);
	SolidShader.SetFloat("material.shininess", 0.6 * 128);

	//Solid Vertex
	std::vector<float> SolidVertexAttrib;
	for (auto tri : triangles) {
		for (int i = 0; i < 3; ++i) {
			SolidVertexAttrib.push_back(tri.vertex[i].x);
			SolidVertexAttrib.push_back(tri.vertex[i].y);
			SolidVertexAttrib.push_back(tri.vertex[i].z);
			SolidVertexAttrib.push_back(tri.normal[i].x);
			SolidVertexAttrib.push_back(tri.normal[i].y);
			SolidVertexAttrib.push_back(tri.normal[i].z);
		}
	}

	//WireFrame Vertex
	std::vector<float> SolidEdges;
	Edge* e = solid->sEdge;
	int NumEdge = 0;
	do {
		NumEdge++;
		SolidEdges.push_back((e->he1->origin->Pos).position.x);
		SolidEdges.push_back((e->he1->origin->Pos).position.y);
		SolidEdges.push_back((e->he1->origin->Pos).position.z);
		SolidEdges.push_back((e->he1->destination->Pos).position.x);
		SolidEdges.push_back((e->he1->destination->Pos).position.y);
		SolidEdges.push_back((e->he1->destination->Pos).position.z);
		e = e->eNext;
	} while (e != solid->sEdge);

	//LightingBox Vertex
	float Lighting_box[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	};


	unsigned int sVAO, wVAO, bVAO;
	glGenVertexArrays(1, &sVAO);
	glGenVertexArrays(1, &wVAO);
	glGenVertexArrays(1, &bVAO);

	//Soild
	glBindVertexArray(sVAO);
	unsigned int sVBO;
	glGenBuffers(1, &sVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SolidVertexAttrib.size(), &SolidVertexAttrib[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);

	//WirFarame
	glBindVertexArray(wVAO);
	unsigned int wVBO;
	glGenBuffers(1, &wVBO);
	glBindBuffer(GL_ARRAY_BUFFER, wVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * SolidEdges.size(), &SolidEdges[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//lighting Box
	glBindVertexArray(bVAO);
	unsigned int bVBO;
	glGenBuffers(1, &bVBO);
	glBindBuffer(GL_ARRAY_BUFFER, bVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 36, Lighting_box, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glLineWidth(4);
	std::cout << "triangles:" << triangles.size() << std::endl;
	while (!glfwWindowShouldClose(window)) {
		//camera value handle

		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;


		//input
		processInput(window);
		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) { //F1 set the solid lit mode
			IsWireFrame = false;
			IsLit = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) { //F2 set the solid unlit mode
			IsWireFrame = false;
			IsLit = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) { //F3 set the triangle wireframe mode
			IsWireFrame = false;
			IsLit = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) { //F4 set the solid wireframe mode
			IsWireFrame = true;
			IsLit = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !IsWireFrame && IsLit) { //F set the solid lit mode to turn on / off the spot light
			SpotOn = !SpotOn;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		


		//draw
		if (IsWireFrame) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			WireFrameShader.Bind();
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
			WireFrameShader.SetUniformMatrx("model", model);
			WireFrameShader.SetUniformMatrx("view", view);
			WireFrameShader.SetUniformMatrx("projection", projection);
			glBindVertexArray(wVAO);
			glDrawArrays(GL_LINES, 0, NumEdge*2);
		}
		else {
			if (IsLit) {
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				SolidShader.Bind();
				//transform
				glm::mat4 model = glm::mat4(1.0f);
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
				SolidShader.SetUniformMatrx("model", model);
				SolidShader.SetUniformMatrx("view", view);
				SolidShader.SetUniformMatrx("projection", projection);
				SolidShader.SetUniformMatrx("NormalMatrix", normalMatrix);
				//light
				SolidShader.SetVec3("pointLight.position", pointLightPos);
				SolidShader.SetVec3("spotLight.position", camera.GetPosition());
				SolidShader.SetVec3("spotLight.direction", camera.GetFront());
				//camera
				SolidShader.SetVec3("cameraPos", camera.GetPosition());
				glBindVertexArray(sVAO);
				glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);

				//lightCube
				LightingBoxShader.Bind();
				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPos);
				model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
				LightingBoxShader.SetUniformMatrx("model", model);
				LightingBoxShader.SetUniformMatrx("view", view);
				LightingBoxShader.SetUniformMatrx("projection", projection);
				glBindVertexArray(bVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			else {
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				UnlitShader.Bind();
				//transform
				glm::mat4 model = glm::mat4(1.0f);
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
				glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
				UnlitShader.SetUniformMatrx("model", model);
				UnlitShader.SetUniformMatrx("view", view);
				UnlitShader.SetUniformMatrx("projection", projection);
				glBindVertexArray(sVAO);
				glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
			}
		}

		//Swap buffer and Event
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.ProcessKeyboard(LOW, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.ProcessKeyboard(HIGH, deltaTime);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void CursorPosCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (firstGetCursor) {
		lastX = xpos;
		lastY = ypos;
		firstGetCursor = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMove(xoffset, yoffset, GL_TRUE);
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
