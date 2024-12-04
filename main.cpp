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



#define numVAOs 2
#define numVBOs 7


void setupVertices();
void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void GenerateBuffers(GLuint* VAO, GLuint* VBO, GLuint VAO_INITIAL_INDEX, GLuint VBO_INITIAL_INDEX, bool is_element_array_buffer=false);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void GenerateBuffersOrbit(GLuint* VAO, GLuint* VBO);

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 200.0f));
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
GLuint sunTexture, earthTexture, moonTexture, mercuryTexture;
GLuint venusTexture, marsTexture, jupiterTexture, saturnTexture;
GLuint urnausTexture, neptuneTexture;


std::vector<int> ind;
std::vector<glm::vec3> vert;
std::vector<glm::vec2> tex;
std::vector<glm::vec3> norm;

std::vector<float> pvalues; // Vertex Positions
std::vector<float> tvalues; // Texture Coordinates
std::vector<float> nvalues; // Normal Vectors

std::stack<glm::mat4> mStack;

Sphere sphere(156);



constexpr float sun_radius = 695700000;
constexpr float sun_size = 50.0f;

constexpr float earth_radius = 6378.137f;
//constexpr float earth_size = earth_radius * (sun_size / sun_radius);
constexpr float earth_size = 5.0f;
constexpr float earth_distance_from_sun = 150196428.0f;
constexpr float earth_distance = 250.0f;

constexpr float moon_radius = 1738.1f;
constexpr float moon_size = moon_radius * (earth_size / earth_radius);

constexpr float mercury_radius = 2439.137f;
constexpr float mercury_size = mercury_radius * (earth_size / earth_radius);
constexpr float mercury_distance_from_sun = 58000000.0f;
constexpr float mercury_distance = mercury_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float venus_radius = 6051.137f;
constexpr float venus_size = venus_radius * (earth_size / earth_radius);
constexpr float venus_distance_from_sun = 108000000.0f;
constexpr float venus_distance = venus_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float mars_radius = 3396.2f;
constexpr float mars_size = mars_radius * (earth_size / earth_radius);
constexpr float mars_distance_from_sun = 228000000.0f;
constexpr float mars_distance = mars_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float jupiter_radius = 6051.137f;
constexpr float jupiter_size = jupiter_radius * (earth_size / earth_radius);
constexpr float jupiter_distance_from_sun = 108000000.0f;
constexpr float jupiter_distance = jupiter_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float saturn_radius = 6051.137f;
constexpr float saturn_size = saturn_radius * (earth_size / earth_radius);
constexpr float saturn_distance_from_sun = 108000000.0f;
constexpr float saturn_distance = saturn_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float uranus_radius = 6051.137f;
constexpr float uranus_size = uranus_radius * (earth_size / earth_radius);
constexpr float uranus_distance_from_sun = 108000000.0f;
constexpr float uranus_distance = uranus_distance_from_sun * (earth_distance / earth_distance_from_sun);

constexpr float neptune_radius = 6051.137f;
constexpr float neptune_size = neptune_radius * (earth_size / earth_radius);
constexpr float neptune_distance_from_sun = 108000000.0f;
constexpr float neptune_distance = neptune_distance_from_sun * (earth_distance / earth_distance_from_sun);


Torus orbit(mars_distance, 0.5f, 96);

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

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.0f;
	sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = 0.0f;

	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	setupVertices();


	sunTexture = Utils::loadTexture("sun.jpg");
	earthTexture = Utils::loadTexture("earth.jpg");
	moonTexture = Utils::loadTexture("moon.jpg");
	mercuryTexture = Utils::loadTexture("mercury.jpg");
	venusTexture = Utils::loadTexture("venus.jpg");
	marsTexture = Utils::loadTexture("mars.jpg");
	jupiterTexture = Utils::loadTexture("jupiter.jpg");
	saturnTexture = Utils::loadTexture("saturn.jpg");
	urnausTexture = Utils::loadTexture("uranus.jpg");
	neptuneTexture = Utils::loadTexture("neptune.jpg");

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

	// Orbit
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	//mMat = glm::rotate(mMat, toRadians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);


	glDrawElements(GL_TRIANGLES, orbit.getIndices().size(), GL_UNSIGNED_INT, 0);

	// --- Sun
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Sun Position
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), sun_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Sun Position and Scaling

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());

	mStack.pop();

	// --- Earth
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime*0.2f)*earth_distance, 0.0f, cos((float)currentTime*0.2f)*earth_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), earth_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();

	// --- Moon
	mStack.push(mStack.top());
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime)*3.0f, 0.0f, cos((float)currentTime)*3.0f));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), moon_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation

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
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime*0.2f)*mercury_distance, 0.0f, cos((float)currentTime*0.2f)*mercury_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), mercury_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation

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
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime*0.2f)*venus_distance, 0.0f, cos((float)currentTime*0.2f)*venus_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), venus_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation

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
	mStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime*0.2f)*mars_distance, 0.0f, cos((float)currentTime*0.2f)*mars_distance));
	mStack.push(mStack.top());
	mStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0f), mars_size * glm::vec3(1.0f, 1.0f, 1.0f)); // Planet Rotation

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mStack.top()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marsTexture);

	glDrawArrays(GL_TRIANGLES, 0, sphere.getNumIndices());
	mStack.pop();



	// Remove Last Planet's Position, Sun's Position and View Matrix
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