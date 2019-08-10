
#define GLEW_STATIC
#include <GL/glew.h>
//#include<glad/glad.h>
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
//#include "d8.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#include "SOIL.h"
//#include "ParticleSystem.h"
//#include "ResourceManager.h"
using namespace std;
glm::vec3 HSVtoRGB(float h, float s, float v);
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//unsigned int loadTexture(char const * path);
vector<std::string> faces
{
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"front.jpg",
	"back.jpg"
};


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
//unsigned int loadTexture(const char *path);
//unsigned int loadCubemap(vector<std::string> faces);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool contourMode = true;

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

DemData* dataPtr;
DemData* dataRaw;

MatchingBox* contour;

float curHeight = 900;

//ParticleSystem  *Particles;

void RaiseContour(float raiseAt,float raiseHeight) {
	
	if (curHeight + raiseHeight < dataPtr->minHeight )
		return;

	//float* tempHeight = new float[dataPtr->nrows*dataPtr->ncols];
	for (int r = 0; r < dataPtr->nrows; r++)
		for (int c = 0; c < dataPtr->ncols; c++)
		{	
			int cur = c + r * dataPtr->ncols;
			
			//calculate new height
			if (dataPtr->data[r][c] > raiseAt)
				dataPtr->data[r][c] = dataPtr->data[r][c] + raiseHeight;
			else
				dataPtr->data[r][c] = (dataPtr->data[r][c] - dataPtr->minHeight) *(raiseAt + raiseHeight - dataPtr->minHeight) / (raiseAt - dataPtr->minHeight) + dataPtr->minHeight;
			demVertice[cur * 8 + 2] = (dataPtr->data[r][c] - dataPtr->averHeight)*offset*0.1f;
			//tempHeight[cur] = dataPtr->data[r][c];
			//re-calculate rgb	
			GetVerticeColor(dataPtr, &demVertice[cur * 8 + 3], r, c);
		}

	curHeight += raiseHeight;
	glBindBuffer(GL_ARRAY_BUFFER, demVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * dataPtr->ncols*dataPtr->nrows, &demVertice[0], GL_STATIC_DRAW);

	for (int i = 0; i <2 * contour->lineSize; i++) {
		contour->lines[i * 3 + 2] = contour->lines[i * 3 + 2] + raiseHeight*offset*0.1f;
	}
	if (contourMode) {		
		glBindBuffer(GL_ARRAY_BUFFER, contourVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * contour->lineSize * 2, &contour->lines[0], GL_STATIC_DRAW);
	}
	else {
		MatchingBox c = MatchingBox(raiseAt + raiseHeight, dataPtr);
		glBindBuffer(GL_ARRAY_BUFFER, contourVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * c.lineSize * 2, c.lines, GL_STATIC_DRAW);
	}

	
	//delete[] tempHeight;
}



enum RenderType{
	lineFrame,
	solidColor,
	texture,
	textureMixedHeight
};

RenderType renderType;
int main()
{


	renderType = (RenderType)0;
	DemData d("Data\\1.asc");
	//DemData::Smooth(d);
	//DemData d1("Data\\1.asc");
	/*for (int r = 0; r < d.nrows; r++){
		for (int c = 0; c < d.ncols; c++)
		{
			if (d.data[r][c] - d1.data[r][c] != 0)
				cout << d.data[r][c];
		}
	}*/
	//D8 d8(d);
	//d8.PrintDataNIP(1);
	//d8.PrintData(10);
	//dataPtr = &d;
	//dataRaw = new DemData("Data\\1.asc");

	demVertice = new float[d.ncols*d.nrows * 8];
	demIndice = new int[d.ncols*d.nrows * 6];

	for (int r = 0; r < d.nrows ; r++)
		for (int c = 0; c < d.ncols ; c++)
		{
			int cur = c + r * d.ncols;
			//position
			demVertice[cur * 8] = offset * c;
			demVertice[cur * 8 + 1] = offset * r;
			demVertice[cur * 8 + 2] = (d.data[r][c] - d.averHeight)*offset*0.1f;
			//rgb	
			GetVerticeColor(&d,&demVertice[cur * 8 + 3],r,c);
			//uv
			demVertice[cur * 8 + 6] = c/(float)d.ncols;
			demVertice[cur * 8 + 7] = r/(float)d.nrows;
			//cout << c / (float)d.ncols << endl;
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
	
	

	int contourSize = 1; float contourOffset = 50.0f;
	float contourStart = 900;
	MatchingBox* boxArray = new MatchingBox[contourSize];
	for (int i = 0; i < contourSize; i++) {
		boxArray[i] = MatchingBox(contourStart + i*contourOffset, &d);
	}
	contour = &boxArray[0];

	colLineVertice = new float[d.ncols * 3];
	for (int i = 0; i < d.ncols; i++) {
		colLineVertice[i * 3] = demVertice[i * 8];
		colLineVertice[i * 3 + 1] = demVertice[i * 8 + 1];
		colLineVertice[i * 3 + 2] = (d.minHeight - d.averHeight)*offset*0.1f - 0.2f;
	}

	rowLineVertice = new float[d.nrows * 3];
	for (int i = 0; i < d.nrows; i++) {
		rowLineVertice[i * 3] = demVertice[i * d.ncols * 8];
		rowLineVertice[i * 3 + 1] = demVertice[i * d.ncols * 8 + 1];
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
	GLFWwindow* window = glfwCreateWindow(800, 600, "DrawDem", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	


	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	//glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// OpenGL configuration
	glViewport(0, 0, 800, 600);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	//{
	//	std::cout << "Failed to initialize GLAD" << std::endl;
	//	return -1;
	//}

	//Shader demShader = ResourceManager::LoadShader("FirstShader.vert", "FirstShader.frag", nullptr, "dem");
	//Shader lineShader = ResourceManager::LoadShader("Line.vs", "Line.fs", nullptr, "line");
	//Shader skyboxShader = ResourceManager::LoadShader("SkyBox.vs", "SkyBox.fs", nullptr, "skybox");

	/*

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(demVertice)* 8 * d.ncols*d.nrows, &demVertice[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, demEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * (d.ncols - 1) * (d.nrows -1) , &demIndice[0], GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);



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


	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	*/

	//unsigned int texture;
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//// 为当前绑定的纹理对象设置环绕、过滤方式
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// 加载并生成纹理

	//
	//int width, height, nrChannels;
	//unsigned char *data = SOIL_load_image("grass1.jpg", &width, &height, &nrChannels, 0);
	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//SOIL_free_image_data(data);

	//unsigned int cubeTexture = loadTexture("resources/textures/marble.jpg");
	//ResourceManager::LoadTexture("grass1.jpg", GL_FALSE, "grass");
	vector<std::string> faces
	{
		"skybox\\right.jpg",
		"skybox\\left.jpg",
		"skybox\\top.jpg",
		"skybox\\bottom.jpg",
		"skybox\\front.jpg",
		"skybox\\back.jpg"
	};
//	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------

	//	skyboxShader.Use();
	//skyboxShader.SetInteger("skybox", 0);


	
	glEnable(GL_DEPTH_TEST);
	
	camera.SetPosition(glm::vec3(d.ncols*offset/2, d.ncols*offset / 2 - 1.5f , 0.8f));

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//RaiseContour(900,  651.0f-900.0f);
	
	while (!glfwWindowShouldClose(window))
	{
		/*

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
		


		//glBindTexture(GL_TEXTURE_2D, texture);
		//draw dem
		demShader.Use();
		demShader.SetMatrix4("projection", projection);
		demShader.SetMatrix4("view", view);
		demShader.SetMatrix4("model", model);
		demShader.SetInteger("type", renderType);
		
		glBindVertexArray(demVAO);
		glDrawElements(GL_TRIANGLES, (d.ncols - 1)*(d.nrows - 1) * 6, GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if(renderType == 0)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		lineShader.Use();
		lineShader.SetVector3f("color", glm::vec3(1.0f, 1.0f, 1.0f));
		lineShader.SetMatrix4("projection", projection);
		lineShader.SetMatrix4("view", view);
		if (renderType == 0) {
			//draw grid
			
			glBindVertexArray(gridVAO[0]);
			model = glm::mat4(1.0f);
			for (unsigned int i = 0; i < d.nrows / 3.0f; i++)
			{
				lineShader.SetMatrix4("model", model);
				model = glm::translate(model, glm::vec3(0.0f, offset * 3, 0.0f));
				glDrawArrays(GL_LINE_STRIP, 0, d.ncols);
			}
			glBindVertexArray(gridVAO[1]);
			model = glm::mat4(1.0f);
			for (unsigned int i = 0; i < d.ncols / 3.0f; i++)
			{
				lineShader.SetMatrix4("model", model);
				model = glm::translate(model, glm::vec3(offset * 3, 0.0f, 0.0f));
				glDrawArrays(GL_LINE_STRIP, 0, d.nrows);
			}
			
		}

		
		for (int i = 0; i < contourSize; i++) {
			//TODO: Fade Color by HSV
			lineShader.SetVector3f("color", glm::vec3(0.8f, 0.6f, 1.0f));
			glBindVertexArray(contourVAO[i]);
			model = glm::mat4(1.0f);
			lineShader.SetMatrix4("model", model);
			glLineWidth(5.0f);
			glDrawArrays(GL_LINES, 0, boxArray[i].lineSize * 2);

		}
		

		//if (renderType != 0) {
		//
		//	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		//	skyboxShader.Use();
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		//	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		//	skyboxShader.SetMatrix4("model", model);
		//	skyboxShader.SetMatrix4("view", view);
		//	skyboxShader.SetMatrix4("projection", projection);
		//	// skybox cube
		//	glBindVertexArray(skyboxVAO);
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//	glBindVertexArray(0);
		//	glDepthFunc(GL_LESS); // set depth function back to default
		//}
		//

		*/
		glfwSwapBuffers(window);
		//glfwPollEvents();
		
	}

	glDeleteVertexArrays(1, &demVAO);
	glDeleteBuffers(1, &demVBO);
	glDeleteBuffers(1, &demEBO);


	glDeleteVertexArrays(2,gridVAO);
	glDeleteBuffers(2, gridVBO);


//	ResourceManager::Clear();
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


	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		renderType = (RenderType)0;
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		renderType = (RenderType)1;
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
		renderType = (RenderType)2;
	if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		renderType = (RenderType)3;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
		RaiseContour(900, 10);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		RaiseContour(900, -10);


	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		contourMode = true;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		contourMode = false;
		
	
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



//unsigned int loadTexture(char const * path)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}

//
//unsigned int loadCubemap(vector<std::string> faces)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	int width, height, nrChannels;
//	for (unsigned int i = 0; i < faces.size(); i++)
//	{
//		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
//		if (data)
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
//			);
//			SOIL_free_image_data(data);
//		}
//		else
//		{
//			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//			SOIL_free_image_data(data);
//		}
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}