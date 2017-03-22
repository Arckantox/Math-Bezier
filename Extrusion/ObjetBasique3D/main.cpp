
#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#include <windows.h>
#endif

#include "glew.h"

#include "gle.h"

#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#endif

#include "freeglut.h"

#include "../common/EsgiShader.h"
#include "../common/mat4.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

// format des vertices : X, Y, Z, ?, ?, ?, ?, ? = 8 floats
#include "../data/DragonData.h"

#if _MSC_VER
uint32_t dragonVertexCount = _countof(DragonVertices);
uint32_t dragonIndexCount = _countof(DragonIndices);
#endif

//-------------------------------------------------------------------- TEST extrusion ---------------------------------------------------------------------
double t = 0.75;

double shape[4][2] = 
{
	{-t, -t},
	{-t, t},
	{t, t},
	{t, -t}
};

double path[4][3] = 
{
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0,0.0,2.0},
	{0.0,0.0,3.0}
};

double up[3] = { 0.0,1.0,0.0 };


void extrude()
{

	gleExtrusion(4, shape, NULL, up, 4, path, NULL);
}

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


bool Initialize()
{
	glewInit();
	extrude();
	g_BasicShader.LoadVertexShader("basic.vs");
	g_BasicShader.LoadFragmentShader("basic.fs");
	g_BasicShader.CreateProgram();

	glGenTextures(1, &TexObj);
	glBindTexture(GL_TEXTURE_2D, TexObj);
	int w, h, c; //largeur, hauteur et # de composantes du fichier
	uint8_t* bitmapRGBA = stbi_load("../data/dragon.png",
		&w, &h, &c, STBI_rgb_alpha);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //GL_NEAREST)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, // Destination
		GL_RGBA, GL_UNSIGNED_BYTE, bitmapRGBA);		// Source

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bitmapRGBA);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(1, &VBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, dragonVertexCount * sizeof(float), DragonVertices, GL_STATIC_DRAW);

	// rendu indexe
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dragonIndexCount *  sizeof(GLushort), DragonIndices, GL_STATIC_DRAW);

	// le fait de specifier 0 comme BO desactive l'usage des BOs
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	
	uint32_t texUnit = 0;
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, TexObj);
	auto texture_location = glGetUniformLocation(program, "u_Texture");
	glUniform1i(texture_location, texUnit);
	
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

	// ATTRIBUTES
	auto normal_location = glGetAttribLocation(program, "a_Normal");
	auto position_location = glGetAttribLocation(program, "a_Position");
	auto texcoords_location = glGetAttribLocation(program, "a_TexCoords");
	//glVertexAttrib3f(color_location, 0.0f, 1.0f, 0.0f);

	// Le fait de specifier la ligne suivante va modifier le fonctionnement interne de glVertexAttribPointer
	// lorsque GL_ARRAY_BUFFER != 0 cela indique que les donnees sont stockees sur le GPU
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//le VBO contient des vertex au format suivant :
	//X, Y, Z, ?, ?, ?, U, V

	struct Vertex
	{
		float x, y, z;		// offset = 0
		float nx, ny, nz;	// offset = 3
		float u, v;			// offset = 6
	};

	// 1er cas on � l'adresse du tableau
	Vertex* v = (Vertex*)DragonVertices;

	//size_t rel = offsetof(adresse - DragonVertices)

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void *>(0 * sizeof(float)));
	
	glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void *>(3 * sizeof(float)));
	// on interprete les 3 valeurs inconnues comme RGB alors que ce sont les normales
	glVertexAttribPointer(texcoords_location, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<const void *>(6 * sizeof(float)));

	//glEnableVertexAttribArray(texcoords_location);
	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(normal_location);
	glEnableVertexAttribArray(texcoords_location);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, dragonIndexCount, GL_UNSIGNED_SHORT, nullptr);

	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(normal_location);
	glDisableVertexAttribArray(texcoords_location);
	glUseProgram(0);

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
	// GLUT_DEPTH alloue egalement une zone m�moire pour le depth buffer
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	// positionne et dimensionne la fenetre
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(width, height);
	// creation de la fenetre ainsi que du contexte de rendu
	glutCreateWindow("Transformation");

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