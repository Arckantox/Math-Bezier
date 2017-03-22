
#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#include <windows.h>
#endif

#include <vector>

#include "glew.h"

#include <stdio.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#endif

#include "freeglut.h"

#include "../common/EsgiShader.h"
#include "../common/mat4.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"


//-------------------------------------------------------------------- TEST extrusion ---------------------------------------------------------------------

struct Vec2 {
	float x;
	float y;

	Vec2(float a, float b)
	{
		x = a;
		y = b;
	}

	Vec2 operator+(const Vec2& vec) const {
		return Vec2{ x + vec.x, y + vec.y };
	}

	Vec2 operator-(const Vec2& vec) const {
		return Vec2{ x - vec.x, y - vec.y };
	}

	Vec2 operator*(float n) {
		return Vec2{ n * x, n * y };
	}
};

struct Vec3 {
	float x;
	float y;
	float z;

	Vec3(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	Vec3 operator+(const Vec3& vec) const {
		return Vec3{ x + vec.x, y + vec.y, z + vec.z };
	}

	Vec3 operator-(const Vec3& vec) const {
		return Vec3{ x - vec.x, y - vec.y, z - vec.z };
	}

	Vec3 operator*(float n) {
		return Vec3{ n * x, n * y, n * z };
	}
};

float t = 0.75;

bool canDisplay = false;
bool lineMode = false;

int sizetab;
int sizeind;

std::vector<Vec3> shape;

void MakeVectors()
{
	shape.push_back(Vec3(-t, -t, 0));
	shape.push_back(Vec3(-t, t, 0 ));
	shape.push_back(Vec3(t, t, 0 ));
	shape.push_back(Vec3(t, -t, 0 ));
}

double path[4][3] = 
{
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0,0.0,2.0},
	{0.0,0.0,3.0}
};

float up[3] = { 0.0,1.0,0.0 };

//--------------------------------------------------------------------------------------------------------------------------------------------------------
EsgiShader g_BasicShader;

// alternativement unsigned short, GLushort, uint16_t
static const GLushort g_Indices[] = { 0, 1, 2, 3 };

int width = 1000;
int height = 1000;

GLuint VBO;	// identifiant du Vertex Buffer Object
GLuint IBO;	// identifiant du Index Buffer Object
GLuint TexObj; // identifiant du Texture Object

// -----------------------------------------------------------------------------------------------TYPE DE CAMERA----------------------------------------------------------------------------------------------
int CamType = 0;
// FPS = 0
// Orbit = 1
// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Camera variables
float posX = 0.0f;
float posY = -4.7f;
float posZ = -17.0f;
float moveSpeed = 50.0f;
float rotSpeed = 0.3f;
float rotX = 0.0f;
float rotY = 0.0f;
float distance = -60.0f;

int lastposX = height * 0.5f;
int lastposY = width * 0.5f;

float deltaAngle = 0.0f;
int xOrigin = -1;


float TimeInSeconds;
int TimeSinceAppStartedInMS;
int OldTime = 0;
float DeltaTime;

void ChangeCam(int type)
{
	switch (type)
	{
	case 0:
		CamType = 0;
		rotX = 0.0f;
		rotY = 0.0f;
		posX = 0.0f;
		posY = -0.5f;
		posZ = -10.0f;
		rotSpeed = 0.3f;
		break;
	case 1:
		CamType = 1;
		rotX = 0.0f;
		rotY = 0.0f;
		posX = 0.0f;
		posY = -0.5f;
		posZ = 0.0f;
		distance = -10.0f;
		break;
	}

}

bool LineExtrusion(std::vector<Vec3> &shape, float length, float coefEnd = 1)
{
	MakeVectors();
	std::vector<Vec3> shape3D;

	int ssize = shape.size();

	std::cout << "Points in shape = " << ssize << std::endl;

	for (int i = 0; i < ssize * 2; i++)	//Remplissage vector des points de la shape 3D, base puis end
	{
		if (i < ssize) // Base shape
		{
			shape3D.push_back(Vec3( shape[i].x, shape[i].y, shape[i].z));
		}
		else	// End shape
		{
			shape3D.push_back(Vec3(shape[i - ssize].x * coefEnd, shape[i - ssize].y * coefEnd, shape[i - ssize].z + length ));
		}
	}

	sizetab = ssize * 6;

	float *vertices = new float[sizetab];

	for (int i = 0; i < ssize*2; i++)		// Remplissage tableau 1D des valeurs de points de la shape 3D pour traitement
	{
		vertices[3 * i] = shape3D[i].x;
		vertices[3 * i + 1] = shape3D[i].y;
		vertices[3 * i + 2] = shape3D[i].z;
		std::cout << "Coord Point " << i << " : (" << vertices[3 * i] << ", " << vertices[3 * i + 1] << ", "<< vertices[3 * i + 2] << ")" << std::endl;
	}

	/* GL_TRIANGLES
	sizeind = 6 * (ssize - 1);
	GLushort *indices = new GLushort[sizeind];

	for (int i = 0; i < ssize-1; i++)	// Remplissage tableau 1D des valeurs d'indices des faces de la shape 3D pour traitement
	{
	indices[6 * i] = i;
	indices[6 * i + 1] = i + 1;
	indices[6 * i + 2] = i + ssize;
	indices[6 * i + 3] = i + ssize;
	indices[6 * i + 4] = i + 1;
	indices[6 * i + 5] = i + ssize + 1;
	}

	for (int i = 0; i < ssize - 1; i++)	// Affichage des infos de triangles
	{
		std::cout << "Triangle " << 2*i << " : { (" << vertices[3 * indices[6 * i]] << ", " << vertices[3 * indices[6 * i] + 1] << ", " << vertices[3 * indices[6 * i] + 2] << ") | ("
		<< vertices[3 * indices[6 * i + 1]] << ", " << vertices[3 * indices[6 * i + 1] + 1] << ", " << vertices[3 * indices[6 * i + 1] + 2] << ") | ("
		<< vertices[3 * indices[6 * i+2]] << ", " << vertices[3 * indices[6 * i+2] + 1] << ", " << vertices[3 * indices[6 * i+2] + 2] << ") }" << std::endl;
		std::cout << "Triangle " << 2*i + 1<< " : { (" << vertices[3 * indices[6 * i+3]] << ", " << vertices[3 * indices[6 * i + 3] + 1] << ", " << vertices[3 * indices[6 * i + 3] + 2] << ") | ("
		<< vertices[3 * indices[6 * i + 4]] << ", " << vertices[3 * indices[6 * i + 4] + 1] << ", " << vertices[3 * indices[6 * i + 4] + 2] << ") | ("
		<< vertices[3 * indices[6 * i + 5]] << ", " << vertices[3 * indices[6 * i + 5] + 1] << ", " << vertices[3 * indices[6 * i + 5] + 2] << ") }" << std::endl;
	}

	*/

	//GL_QUADS
	sizeind = 4 * (ssize - 1);
	GLushort *indices = new GLushort[sizeind];

	for (int i = 0; i < ssize - 1; i++)	// Remplissage tableau 1D des valeurs d'indices des faces de la shape 3D pour traitement
	{
		indices[4 * i] = i;
		indices[4 * i + 1] = i + 1;
		indices[4 * i + 2] = i + ssize + 1;
		indices[4 * i + 3] = i + ssize;

	}

	/*for (int i = 0; i < sizeind; i++)
	{
		std::cout << i << " : " << indices[i] << std::endl;
	}*/
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizetab * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeind * sizeof(GLushort), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertices;
	delete[] indices;

	return true;
}


void DisplayExtrusion()
{
	auto program = g_BasicShader.GetProgram();
	glUseProgram(program);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	auto position_location = glGetAttribLocation(program, "a_Position");

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<const void *>(0 * sizeof(float)));


	//glEnableVertexAttribArray(texcoords_location);
	glEnableVertexAttribArray(position_location);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_QUADS, sizeind, GL_UNSIGNED_SHORT, nullptr);

	glDisableVertexAttribArray(position_location);

	glUseProgram(0);
}

/*
bool PathExtrusion(std::vector<Vec3> &shape, std::vector<Vec3> &path)
{
	MakeVectors();
	std::vector<Vec3> shape3D;

	int ssize = shape.size();

	std::cout << "Points in shape = " << ssize << std::endl;

	for (int i = 0; i < ssize * 2; i++)	//Remplissage vector des points de la shape 3D, base puis end
	{
		if (i < ssize) // Base shape
		{
			shape3D.push_back(Vec3(shape[i].x, shape[i].y, shape[i].z));
		}
		else	// End shape
		{
			shape3D.push_back(Vec3(shape[i - ssize].x * coefEnd, shape[i - ssize].y * coefEnd, shape[i - ssize].z + length));
		}
	}

	sizetab = ssize * 6;

	float *vertices = new float[sizetab];

	for (int i = 0; i < ssize * 2; i++)		// Remplissage tableau 1D des valeurs de points de la shape 3D pour traitement
	{
		vertices[3 * i] = shape3D[i].x;
		vertices[3 * i + 1] = shape3D[i].y;
		vertices[3 * i + 2] = shape3D[i].z;
		std::cout << "Coord Point " << i << " : (" << vertices[3 * i] << ", " << vertices[3 * i + 1] << ", " << vertices[3 * i + 2] << ")" << std::endl;
	}

	//GL_QUADS
	sizeind = 4 * (ssize - 1);
	GLushort *indices = new GLushort[sizeind];

	for (int i = 0; i < ssize - 1; i++)	// Remplissage tableau 1D des valeurs d'indices des faces de la shape 3D pour traitement
	{
		indices[4 * i] = i;
		indices[4 * i + 1] = i + 1;
		indices[4 * i + 2] = i + ssize + 1;
		indices[4 * i + 3] = i + ssize;

	}

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizetab * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeind * sizeof(GLushort), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertices;
	delete[] indices;

	return true;
}
*/

bool Initialize()
{
	// Test extrusion
	LineExtrusion(shape, 5, 0.3);

	glewInit();
	g_BasicShader.LoadVertexShader("basic.vs");
	g_BasicShader.LoadFragmentShader("basic.fs");
	g_BasicShader.CreateProgram();

	ChangeCam(CamType);


	return true;
}

void Terminate()
{
	glDeleteTextures(1, &TexObj);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);

	g_BasicShader.DestroyProgram();
}

void update()
{
	glutPostRedisplay();
}

void animate()
{
	if (lineMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	

	// afin d'obtenir le deltatime actuel
	TimeSinceAppStartedInMS = glutGet(GLUT_ELAPSED_TIME);
	TimeInSeconds = TimeSinceAppStartedInMS / 1000.0f;
	DeltaTime = (TimeSinceAppStartedInMS - OldTime )/ 1000.0f;
	OldTime = TimeSinceAppStartedInMS;

	glViewport(0, 0, width, height);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	//glColorMask(GL_TRUE, GL_FALSE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	auto program = g_BasicShader.GetProgram();
	glUseProgram(program);
	
	// UNIFORMS
	Esgi::Mat4 worldMatrix;
	worldMatrix.MakeScale(1.0f, 1.0f, 1.0f);

	//  Camera Matrix
	Esgi::Mat4 cameraMatrix;
	switch (CamType)
	{
	case 0:	//FPS
		cameraMatrix = FPSCamera(posX, posY, posZ, rotX, rotY);
		break;
	case 1:	//Orbit
		cameraMatrix = OrbitCamera(posX, posY, posZ, distance, rotX, rotY);
		break;
	}
	

	//

	auto world_location = glGetUniformLocation(program, "u_WorldMatrix");
	glUniformMatrix4fv(world_location, 1, GL_FALSE, worldMatrix.m);

	Esgi::Mat4 projectionMatrix;
	float w = glutGet(GLUT_WINDOW_WIDTH), h = glutGet(GLUT_WINDOW_HEIGHT);
	// ProjectionMatrix
	float aspectRatio = w / h;			// facteur d'aspect
	float fovy = 45.0f;					// degree d'ouverture
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	projectionMatrix.Perspective(fovy, aspectRatio, nearZ, farZ);

	//projectionMatrix.MakeScale(1.0f / (0.5f*w), 1.0f / (0.5f*h), 1.0f);

	auto projection_location = glGetUniformLocation(program, "u_ProjectionMatrix");
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projectionMatrix.m);

	auto camera_location = glGetUniformLocation(program, "u_CameraMatrix");
	glUniformMatrix4fv(camera_location, 1, GL_FALSE, cameraMatrix.m);

	auto time_location = glGetUniformLocation(program, "u_Time");
	glUniform1f(time_location, TimeInSeconds);


	DisplayExtrusion();

	//Repositionnement du curseur 
	//glutWarpPointer(width*0.5f, height*0.5f);

	glutSwapBuffers();
}

float COS(float angle)
{
	return cos(angle * 2 * 0.00872665);
}
float SIN(float angle)
{
	return sin(angle * 2 * 0.00872665);
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 'z')
	{
		if (CamType == 0)
		{
			posZ += moveSpeed *  DeltaTime * COS(rotY);
			posX += moveSpeed *  DeltaTime * -SIN(rotY);
			posY += moveSpeed *  DeltaTime * SIN(rotX);
		}
		else
		{
			distance += moveSpeed *  DeltaTime;
		}
	}
	if (key == 'q')
	{
		posZ += moveSpeed *  DeltaTime * COS(rotY - 90);
		posX += moveSpeed *  DeltaTime * -SIN(rotY - 90);

	}
	if (key == 's')
	{
		if (CamType == 0)
		{
			posZ -= moveSpeed *  DeltaTime * COS(rotY);
			posX -= moveSpeed *  DeltaTime * -SIN(rotY);
			posY -= moveSpeed *  DeltaTime * SIN(rotX);
		}
		else
		{
			distance -= moveSpeed *  DeltaTime;
		}
	}
	if (key == 'd')
	{
		posZ -= moveSpeed *  DeltaTime * COS(rotY - 90);
		posX -= moveSpeed *  DeltaTime * -SIN(rotY - 90);
	}
	if (key == 27)	// Echap
	{
		exit(0);
	}
	if (key == 32)	// Space Bar
	{
		posY -= moveSpeed *  DeltaTime;
	}
	if (key == 'f')
	{
		posY += moveSpeed *  DeltaTime;
	}
	if (key == '&')	 // 1 Mode FPS
	{
		ChangeCam(0);
	}
	if (key == 233)	 // 2 Mode Orbit
	{
		ChangeCam(1);
	}
	if (key == 'l')
	{
		lineMode = !lineMode;
	}
}

void mouse(int x, int y) 
{
	
	//rotX = (float)(y-height*0.5f) * rotSpeed;
	//rotY = (float)(x-width*0.5f) * rotSpeed;
	rotX += (y - lastposY)* rotSpeed;
	rotY += (x - lastposX)* rotSpeed;
	lastposX = x;
	lastposY = y;
}

int main(int argc, const char* argv[])
{
	// passe les parametres de la ligne de commande a glut
	glutInit(&argc, (char**)argv);
	// defini deux color buffers (un visible, un cache) RGBA
	// GLUT_DEPTH alloue egalement une zone mémoire pour le depth buffer
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	// positionne et dimensionne la fenetre
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(width, height);
	// creation de la fenetre ainsi que du contexte de rendu
	glutCreateWindow("Extrusion");

#ifdef FREEGLUT
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

#ifdef NO_GLEW
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)
		wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glDisableVertexAttribArray");
#else
	glewInit();
#endif

	if (Initialize() == false)
		return -1;

	glutIdleFunc(update);
	glutDisplayFunc(animate);
	glutPassiveMotionFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	Terminate();

	return 1;
}