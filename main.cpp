#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "sphere.h"
#include "camera.h"
#include "Torus.h"
#include <stack>
#include "Constants.h"

enum Planets
{
	MERCURY,
	VENUS,
	EARTH,
	MARS,
	JUPITER,
	SATURN,
	URNAUS,
	NEPTUNE
};

#define numVAOs 3
#define numVBOs 8
#define NUMBER_OF_PLANETS 8


void setupVertices();
void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void GenerateBuffers(GLuint* VAO, GLuint* VBO, GLuint VAO_INITIAL_INDEX, GLuint VBO_INITIAL_INDEX, bool is_element_array_buffer=false);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void GenerateBuffersOrbit(GLuint* VAO, GLuint* VBO);
void DrawOrbits(glm::mat4& vMat);

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 200.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLuint renderingProgram, renderingOrbitProgram, skyboxShader;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint skyboxVAO, skyboxVBO;

// allocate variables use in display() function, so that they won't need to be
// allocated during rendering
GLuint mLoc,vLoc, mvLoc, projLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

float tf = 0.0f;
GLuint sunTexture, earthTexture, moonTexture, mercuryTexture;
GLuint venusTexture, marsTexture, jupiterTexture, saturnTexture;
GLuint urnausTexture, neptuneTexture;
GLuint cubemapTexture;


std::vector<int> ind;
std::vector<glm::vec3> vert;
std::vector<glm::vec2> tex;
std::vector<glm::vec3> norm;

std::vector<float> pvalues; // Vertex Positions
std::vector<float> tvalues; // Texture Coordinates
std::vector<float> nvalues; // Normal Vectors

std::stack<glm::mat4> mStack;

Sphere sphere(156);




std::vector<float> RandomOrbitLocationMultiplier;

Torus orbit(Constants::earth_distance, 1.0f, 150);

int main()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to Initialize GLAD\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);

	init(window);


	while(!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);


}

void setupVertices()
{
	ind = sphere.getIndices();
	vert = sphere.getVertices();
	tex = sphere.getTexCoords();
	norm = sphere.getNormals();

	int numIndices = sphere.getNumIndices();
	for(int i = 0; i < numIndices; i++)
	{
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);

		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);

		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(numVAOs, vao);
	glGenBuffers(numVBOs, vbo);

	GenerateBuffers(vao, vbo, 0, 0, true);


	ind.clear();
	vert.clear();
	tex.clear();
	norm.clear();
	pvalues.clear();
	tvalues.clear();
	nvalues.clear();



	ind = orbit.getIndices();
	vert = orbit.getVertices();
	tex = orbit.getTexCoords();
	norm = orbit.getNormals();



	for (int i = 0; i < orbit.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}


	GenerateBuffers(vao, vbo, 1, 3, true);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

    glGenVertexArrays(1, &vao[2]);
    glGenBuffers(1, &vbo[7]);
    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void GenerateBuffersOrbit(GLuint* VAO, GLuint* VBO)
{
	glBindVertexArray(VAO[1]);
	
	// Put the vertices into buffer #0
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Put the Texture Coordinates into buffer #1
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Put the Normals into buffer #2
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
	
}

void GenerateBuffers(GLuint* VAO, GLuint* VBO, GLuint VAO_INITIAL_INDEX, GLuint VBO_INITIAL_INDEX, bool is_element_array_buffer)
{

	glBindVertexArray(*(VAO + (VAO_INITIAL_INDEX + 0)));
	

	// Put the vertices into buffer #0
	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + (VBO_INITIAL_INDEX + 0)));
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Put the Texture Coordinates into buffer #1
	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + (VBO_INITIAL_INDEX + 1)));
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Put the Normals into buffer #2
	glBindBuffer(GL_ARRAY_BUFFER, *(VBO + (VBO_INITIAL_INDEX + 2)));
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);

	if(is_element_array_buffer == true)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(VBO + (VBO_INITIAL_INDEX + 3)));
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

	}

	glBindVertexArray(0);
}

void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	renderingOrbitProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader_Orbit.glsl");
	skyboxShader = Utils::createShaderProgram("vertShader_Skybox.glsl", "fragShader_Skybox.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 50000.0f);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.0f;
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = 0.0f;

	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	setupVertices();


	sunTexture = Utils::loadTexture("./textures/sun.jpg");
	earthTexture = Utils::loadTexture("./textures/earth.jpg");
	moonTexture = Utils::loadTexture("./textures/moon.jpg");
	mercuryTexture = Utils::loadTexture("./textures/mercury.jpg");
	venusTexture = Utils::loadTexture("./textures/venus.jpg");
	marsTexture = Utils::loadTexture("./textures/mars.jpg");
	jupiterTexture = Utils::loadTexture("./textures/jupiter.jpg");
	saturnTexture = Utils::loadTexture("./textures/saturn.jpg");
	urnausTexture = Utils::loadTexture("./textures/uranus.jpg");
	neptuneTexture = Utils::loadTexture("./textures/neptune.jpg");

	/*
	std::vector<std::string> faces
	{
		"./textures/skybox/Stars/Stars_right.jpg",
		"./textures/skybox/Stars/Stars_left.jpg",
		"./textures/skybox/Stars/Stars_top.jpg",
		"./textures/skybox/Stars/Stars_bottom.jpg",
		"./textures/skybox//Stars/Stars_front.jpg",
		"./textures/skybox/Stars/Stars_back.jpg"
	};
	*/
	
	std::vector<std::string> faces
	{
		"./textures/skybox/Nebula/Nebula_right.jpg",
		"./textures/skybox/Nebula/Nebula_left.jpg",
		"./textures/skybox/Nebula/Nebula_top.jpg",
		"./textures/skybox/Nebula/Nebula_bottom.jpg",
		"./textures/skybox//Nebula/Nebula_front.jpg",
		"./textures/skybox/Nebula/Nebula_back.jpg"
	};
	

	cubemapTexture = Utils::loadCubemap(faces);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	srand (static_cast <unsigned> (time(0)));
	float r;

	for(int i = 0; i < NUMBER_OF_PLANETS; i++)
	{
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/7.0f);
		RandomOrbitLocationMultiplier.push_back(r);
	}

	glUseProgram(skyboxShader);
	glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0); 
}

void display(GLFWwindow* window, double currentTime)
{

    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	
	// camera/view transformation
	vMat = camera.GetViewMatrix();
	vMat = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	// draw cube map
	glUseProgram(skyboxShader);
	vLoc = glGetUniformLocation(skyboxShader, "vMat");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	projLoc = glGetUniformLocation(skyboxShader, "pMat");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");


	vMat = camera.GetViewMatrix();	

	// Push View Matrix onto the stack
	mStack.push(vMat);

	// --- Sun
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Sun Position
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), Constants::sun_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Sun Position and Scaling
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();

	// --- Earth
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::EARTH] + currentTime)*Constants::earth_revolution_speed)*Constants::earth_distance, 0.0f, cos((float)((RandomOrbitLocationMultiplier[Planets::EARTH] + currentTime))*Constants::earth_revolution_speed)*Constants::earth_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::earth_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();



	// --- Moon
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime)*Constants::moon_distance, 0.0f, cos((float)currentTime)*Constants::moon_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::moon_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();

	// --- Mercury
	mStack.pop(); // Remove Moon's Position
	mStack.pop(); // Remove Earth's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::MERCURY] + currentTime)*Constants::mercury_revolution_speed)*Constants::mercury_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::MERCURY] + currentTime)*Constants::mercury_revolution_speed)*Constants::mercury_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::mercury_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mercuryTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();


	// --- Venus
	mStack.pop(); // Remove Mercury's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::VENUS] + currentTime)*Constants::venus_revolution_speed)*Constants::venus_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::VENUS] + currentTime)*Constants::venus_revolution_speed)*Constants::venus_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::venus_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, venusTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();


	// --- Mars
	mStack.pop(); // Remove Mercury's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::MARS] + currentTime)*Constants::mars_revolution_speed)*Constants::mars_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::MARS] + currentTime)*Constants::mars_revolution_speed)*Constants::mars_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::mars_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marsTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();


	// --- Jupiter
	mStack.pop(); // Remove Mars's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::JUPITER] + currentTime)*Constants::jupiter_revolution_speed)*Constants::jupiter_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::JUPITER] + currentTime)*Constants::jupiter_revolution_speed)*Constants::jupiter_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::jupiter_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jupiterTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();



	// --- Saturn
	mStack.pop(); // Remove Jupiter's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::SATURN] + currentTime)*Constants::saturn_revolution_speed)*Constants::saturn_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::SATURN] + currentTime)*Constants::saturn_revolution_speed)*Constants::saturn_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::saturn_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturnTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();



	
	// --- Urnaus
	mStack.pop(); // Remove Saturn's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::URNAUS] + currentTime)*Constants::uranus_revolution_speed)*Constants::uranus_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::URNAUS] + currentTime)*Constants::uranus_revolution_speed)*Constants::uranus_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::uranus_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, urnausTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();



	// --- Neptune
	mStack.pop(); // Remove Uranus's Position
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(RandomOrbitLocationMultiplier[Planets::NEPTUNE] + currentTime)*Constants::neptune_revolution_speed)*Constants::neptune_distance, 0.0f, cos((float)(RandomOrbitLocationMultiplier[Planets::NEPTUNE] + currentTime)*Constants::neptune_revolution_speed)*Constants::neptune_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), Constants::neptune_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glBindVertexArray(vao[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neptuneTexture);
	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();

	// Remove Last Planet's Position, Sun's Position and View Matrix
	mStack.pop();
	mStack.pop();
	mStack.pop();

	glUseProgram(renderingOrbitProgram);

	mvLoc = glGetUniformLocation(renderingOrbitProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingOrbitProgram, "proj_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glBindVertexArray(vao[1]);
	DrawOrbits(vMat);
	
	/*
	// Earth Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), Constants::Orbit_Ratios[0] * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);


	// Mercury Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), Constants::Orbit_Ratios[1] * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	
	// Venus Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::venus_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	
	// Mars Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::mars_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	// Jupiter Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::jupiter_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	// Saturn Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::saturn_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	// Uranus Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::uranus_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	// Neptune Orbit
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), (Constants::neptune_distance/Constants::earth_distance) * glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);
	*/
}

void DrawOrbits(glm::mat4& vMat)
{
	glm::mat4 mMat, mvMat;
	for(int i = 0; i < NUMBER_OF_PLANETS; i++)
	{
		// Earth Orbit
		mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0f), Constants::Orbit_Ratios[i] * glm::vec3(1.0f, 1.0f, 1.0f));
		mvMat = vMat * mMat;
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
		glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}