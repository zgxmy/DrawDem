#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<cstdio>
#include<cstring>
#include<fstream>
#include<sstream>
#include "DemData.h"
#include "MatchingBox.h"
#include "Shader.h"
#include "camera.h"
using namespace std;
glm::vec3 HSVtoRGB(float h, float s, float v);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float offset = 0.01f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void GetVerticeColor(const DemData* d,float* p,const int r, const int c) {
	float z = d->data[r][c];
	int H_Max = 0;
	int H_Min = 200;
	int S = 139;
	int V = 247;
	float fh = 200 - ((z- d->minHeight) / d->deltaHeight) * 200.0f;
	
	glm::vec3 color = HSVtoRGB(fh, S/256.0f, V/256.0f);
	*p = color.r;
	*(p + 1) = color.g;
	*(p + 2) = color.b;
}

unsigned int demVBO, demVAO, demEBO;
unsigned int *contourVBO, *contourVAO;
unsigned int gridVBO[2];
unsigned int gridVAO[2];

float* rowLineVertice;
float* colLineVertice;
float* demVertice;
int* demIndice;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f), -90.0f, 60.0f);


void RaiseContour(DemData* d,float raiseAt,float raiseHeight) {
	for (int r = 0; r < d->nrows; r++)
		for (int c = 0; c < d->ncols; c++)
		{	
			int cur = c + r * d->ncols;
			//calculate new height
			if (d->data[r][c] > raiseAt)
				d->data[r][c] = d->data[r][c] + raiseHeight;
			else
				d->data[r][c] = (d->data[r][c] - d->minHeight) *(raiseAt + raiseHeight - d->minHeight) / (raiseAt - d->minHeight) + d->minHeight;
			demVertice[cur * 6 + 2] = (d->data[r][c] - d->averHeight)*offset*0.1f;
			//re-calculate rgb	
			GetVerticeColor(d, &demVertice[cur * 6 + 3], r, c);
		}
}


int main()
{
	DemData d("Data\\1.asc");
	
	demVertice = new float[d.ncols*d.nrows * 2 * 3];
	demIndice = new int[d.ncols*d.nrows * 2 * 3];
	for (int r = 0; r < d.nrows ; r++)
		for (int c = 0; c < d.ncols ; c++)
		{
			int cur = c + r * d.ncols;
			//position
			demVertice[cur * 6] = offset * c;
			demVertice[cur * 6 + 1] = offset * r;
			demVertice[cur * 6 + 2] = (d.data[r][c] - d.averHeight)*offset*0.1f;
			//rgb	
			GetVerticeColor(&d,&demVertice[cur * 6 + 3],r,c);		
			//indice
			if (r < d.nrows - 1 && c < d.ncols - 1) {
				demIndice[cur * 6] = cur;
				demIndice[cur * 6 + 1] = cur + d.ncols;
				demIndice[cur * 6 + 2] = cur + d.ncols + 1;
				demIndice[cur * 6 + 3] = cur;
				demIndice[cur * 6 + 4] = cur + 1;
				demIndice[cur * 6 + 5] = cur + d.ncols + 1;
			}		
		}
	
	RaiseContour(&d, 920, -100);

	int contourSize = 1; float contourOffset = 50.0f;
	float contourStart = 820;
	MatchingBox* boxArray = new MatchingBox[contourSize];
	for (int i = 0; i < contourSize; i++) {
		boxArray[i] = MatchingBox(contourStart + i*contourOffset, &d);
	}
	

	colLineVertice = new float[d.ncols * 3];
	for (int i = 0; i < d.ncols; i++) {
		colLineVertice[i * 3] = demVertice[i * 6];
		colLineVertice[i * 3 + 1] = demVertice[i * 6 + 1];
		colLineVertice[i * 3 + 2] = (d.minHeight - d.averHeight)*offset*0.1f - 0.2f;
	}

	rowLineVertice = new float[d.nrows * 3];
	for (int i = 0; i < d.nrows; i++) {
		rowLineVertice[i * 3] = demVertice[i * d.ncols * 6];
		rowLineVertice[i * 3 + 1] = demVertice[i * d.ncols * 6 + 1];
		rowLineVertice[i * 3 + 2] = (d.minHeight - d.averHeight)*offset*0.1f - 0.2f;
	}


	
	//cout << d.minHeight << d.maxHeight << endl;
	/*
	for (int i = 0; i < box.lineSize*2; i++)
	{
		cout << "X:" << box.lines[i * 3] << " Y:" << box.lines[i * 3 + 1] << " Z:" << box.lines[i * 3 + 2] << endl;
	}
	cout << box.lineSize << endl;
	*/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	Shader ourShader("FirstShader.vert","FirstShader.frag");
	Shader lineShader("Line.vs", "Line.fs");
	

	contourVAO = new unsigned int[contourSize];
	contourVBO = new unsigned int[contourSize];
	glGenBuffers(1, &demVBO);
	glGenVertexArrays(1, &demVAO);
	glGenBuffers(1, &demEBO);

	glGenBuffers(2, gridVBO);
	glGenVertexArrays(2, gridVAO);

	glGenBuffers(contourSize, contourVBO);
	glGenVertexArrays(contourSize, contourVAO);
	
	glBindVertexArray(demVAO);
	glBindBuffer(GL_ARRAY_BUFFER, demVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(demVertice)* 6 * d.ncols*d.nrows, &demVertice[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, demEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * (d.ncols - 1) * (d.nrows -1) , &demIndice[0], GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(gridVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colLineVertice) * 3 * d.ncols, &colLineVertice[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(gridVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rowLineVertice) * 3 * d.nrows, &rowLineVertice[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	for (int i = 0; i < contourSize; i++) {
		glBindVertexArray(contourVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, contourVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(boxArray[i].lines) * 3 * boxArray[i].lineSize * 2, boxArray[i].lines, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	
	}
	



	glViewport(0, 0, 800, 600);
	
	glEnable(GL_DEPTH_TEST);
	
	camera.SetPosition(glm::vec3(d.ncols*offset/2, d.ncols*offset / 2 - 1.5f , 0.8f));

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLineWidth(0.1f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//glm::mat4 model;
		glm::mat4 model = glm::mat4(1.0f);


		//draw dem
		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setMat4("model", model);
		glBindVertexArray(demVAO);
		glDrawElements(GL_TRIANGLES, (d.ncols - 1)*(d.nrows - 1) * 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


		//draw grid
		lineShader.use();
		lineShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
		lineShader.setMat4("projection", projection);
		lineShader.setMat4("view", view);
		glBindVertexArray(gridVAO[0]);
		model = glm::mat4(1.0f);
		for (unsigned int i = 0; i < d.nrows / 3.0f; i++)
		{
			lineShader.setMat4("model", model);
			model = glm::translate(model, glm::vec3(0.0f, offset * 3, 0.0f));
			glDrawArrays(GL_LINE_STRIP, 0, d.ncols);
		}
		glBindVertexArray(gridVAO[1]);
		model = glm::mat4(1.0f);
		for (unsigned int i = 0; i < d.ncols / 3.0f; i++)
		{
			lineShader.setMat4("model", model);
			model = glm::translate(model, glm::vec3(offset * 3, 0.0f, 0.0f));
			glDrawArrays(GL_LINE_STRIP, 0, d.nrows);
		}

		for (int i = 0; i < contourSize; i++) {
			//TODO: Fade Color by HSV
			lineShader.setVec3("color", glm::vec3(0.8f, 0.6f, 1.0f));
			glBindVertexArray(contourVAO[i]);
			model = glm::mat4(1.0f);
			lineShader.setMat4("model", model);
			glLineWidth(5.0f);
			glDrawArrays(GL_LINES, 0, boxArray[i].lineSize * 2);
			
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &demVAO);
	glDeleteBuffers(1, &demVBO);
	glDeleteBuffers(1, &demEBO);

	glDeleteVertexArrays(2,gridVAO);
	glDeleteBuffers(2, gridVBO);



	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

glm::vec3 HSVtoRGB( float h,float s,float v)
{
	unsigned int r, g, b;
	// R,G,B from 0-1, H from 0-360, S,V from 0-1
	float f, p, q, t;
	h /= 60;
	int i = floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	
	switch (i) {
	case 0:
		return glm::vec3(v, t, p);
		break;
	case 1:
		return glm::vec3(q, v, p);
		break;
	case 2:
		return glm::vec3(p, v, t);
		break;
	case 3:
		return glm::vec3(p, q, v);
		break;
	case 4:
		return glm::vec3(t, p, v);
		break;
	default:
		return glm::vec3(v, p, q);
		break;
	}
}
