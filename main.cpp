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
#include <stack>


#define NUMBER_OF_PLANETS 3

#define numVAOs 1
#define numVBOs 3


void setupVertices();
void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void GenerateBuffers(GLuint VAO_ID, GLuint VBO_INITIAL_ID);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// allocate variables use in display() function, so that they won't need to be
// allocated during rendering
GLuint mLoc,vLoc, mvLoc, projLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

float tf = 0.0f;
GLuint sunTexture, earthTexture, moonTexture;


std::vector<int> ind;
std::vector<glm::vec3> vert;
std::vector<glm::vec2> tex;
std::vector<glm::vec3> norm;

std::vector<float> pvalues; // Vertex Positions
std::vector<float> tvalues; // Texture Coordinates
std::vector<float> nvalues; // Normal Vectors

std::stack<glm::mat4> mStack;

Sphere sphere(156);

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

	GenerateBuffers(vao[0], vbo[0]);
}

void GenerateBuffers(GLuint VAO_ID, GLuint VBO_INITIAL_ID)
{
	glBindVertexArray(VAO_ID);
	
	// Put the vertices into buffer #0
	glBindBuffer(GL_ARRAY_BUFFER, VBO_INITIAL_ID);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Put the Texture Coordinates into buffer #1
	glBindBuffer(GL_ARRAY_BUFFER, VBO_INITIAL_ID + 1);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Put the Normals into buffer #2
	glBindBuffer(GL_ARRAY_BUFFER, VBO_INITIAL_ID + 2);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 3.0f;
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = 0.0f;

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	setupVertices();


	sunTexture = Utils::loadTexture("sun.jpg");
	earthTexture = Utils::loadTexture("earth.jpg");
	moonTexture = Utils::loadTexture("moon.jpg");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

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

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// camera/view transformation
    glm::mat4 vMat = camera.GetViewMatrix();

	// Push View Matrix onto the stack
	mStack.push(vMat);

	// --- Sun
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Sun Position
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f)); // Sun Position and Scaling


	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//mMat = mMat * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0));
	//mMat = mMat * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
	//mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());

	mStack.pop();

	// --- Earth
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime*0.2f)*15.0f, 0.0f, cos((float)currentTime*0.2f)*15.0f));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation
	// --- Earth
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f));
	//mMat = mMat * glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0));
	//mMat = mMat * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	//mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();

	// --- Moon
	/*
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime)*1.0f, 0.0f, cos((float)currentTime)*1.0f));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)); // Planet Rotation

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();
	*/

	// Remove Moon's Scale/Rotation/Position, Planet's Position, Sun's Position and View Matrix
	//mStack.pop();
	mStack.pop();
	mStack.pop();
	mStack.pop();
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