#include "glew.h"
#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib") 
#endif
#include <iostream>

#include "freeglut.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm> 
#define PI 3.14159265


typedef struct Point
{
	float x;
	float y;
} Point;


std::vector<Point> points;


std::vector<Point> p;
int n = 10;
int menuPrincipal = 0;
int isDrawing;
int bezierfin;
std::vector<Point> neew;
int isRotating;
int isTranslating;
std::vector<Point> translate;
std::vector<Point> rotate;

void dessinBezier();
void bezier();
void mouse(int button, int state, int x, int y);
void SpecialInput(int key, int x, int y);
void showMenu();
void mainMenuCallback(int);
void drawingMenuCallback(int);
void transformMenuCallback(int);
void rMenuCallback(int);
void tMenuCallback(int);
void translating();
void rotating();



int main(int argc, char **argv)
{
	isDrawing = 0;
	bezierfin = 0;
	isRotating = 0;
	isTranslating = 0;
	// Initialisation de glut et creation de la fenetre
	// ------------------------------------------------
	glutInit(&argc, argv);											// Initialisation
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);		// Mode d'affichage RGB, et test prafondeur
	glutInitWindowSize(500, 500);									// Dimension fenêtre
	glutInitWindowPosition(100, 100);								// Position coin haut gauche
	glutCreateWindow("Bezier");										// Nom
	gluOrtho2D(-250.0, 250.0, -250.0, 250.0);						// Repère 2D délimitant les abscisses et les ordonnées


																	// Initialisation d'OpenGL
																	// -----------------------
	glClearColor(1.0, 1.0, 1.0, 0.50);
	glColor3f(1.0, 1.0, 1.0);			     	 // couleur: blanc
	glPointSize(2.0);							 // taille d'un point: 2px

												 /* Enregistrement des fonctions de rappel
												 => initialisation des fonctions callback appelées par glut */
	

	showMenu();
	glutMouseFunc(mouse);
	glutSpecialFunc(SpecialInput);
	glutDisplayFunc(dessinBezier);


	/* rq: le callback de fonction (fonction de rappel) est une fonction qui est passée en argument à une
	autre fonction. Ici, le main fait usage des deux fonctions de rappel (qui fonctionnent en même temps)
	alors qu'il ne les connaît pas par avance.*/



	/* Entrée dans la boucle principale de glut, traitement des évènements */
	glutMainLoop();								  // lancement de la boucle de réception des évènements
	return 0;
}


void dessinBezier()
{
	glClearColor(1.0, 1.0, 1.0, 0.50);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0, 0.0f);

	for (int i = 0; i < points.size(); i++)
	{
		glBegin(GL_POINTS);
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
	glutSwapBuffers();
	glColor3f(0.0f, 0,1.0f);
	
		for (int k = 0; k < p.size(); k++)
		{
			
			glBegin(GL_POLYGON);
			glBegin(GL_LINE_STRIP);
			
			glVertex2f(p[k].x, p[k].y);

		}
	

	glEnd();
	glutSwapBuffers();
}


void bezier()
{
	float t;
	Point tmp[50];
	
	p = neew;
	for (int k = 0; k < n; k++)
	{
		for (int ij = 0; ij < points.size(); ij++)
		{
			tmp[ij] = points[ij];
		}


		t = (float)k / n;
		for (int j = 1; j <= points.size() -1; j++)
		{
			for (int i = 0; i <= points.size() - 1 - j; i++)
			{
				tmp[i].x = (1-t)*tmp[i].x + t*tmp[i + 1].x;
				tmp[i].y = (1 - t)*tmp[i].y + t*tmp[i + 1].y;

			}
		}

		p.push_back(tmp[0]);


	}
	
	p.push_back(points.at(points.size()-1));
	bezierfin = 1;
}


void mouse(int button, int state, int x, int y)
{
	Point tmp;

	// Si on appuie sur le bouton de gauche

	
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && bezierfin == 0 && isDrawing ==1)
		{
			tmp.x = x - 250.0f;
			tmp.y = -y + 250.0f;
			points.push_back(tmp);
		}
		else
		{
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP &&bezierfin == 1 && isTranslating == 1)
			{
				tmp.x = x - 250.0f;
				tmp.y = -y + 250.0f;
				translate.push_back(tmp);
			}
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP &&bezierfin == 1 && isRotating == 1)
			{
				tmp.x = x - 250.0f;
				tmp.y = -y + 250.0f;
				rotate.push_back(tmp);
			}

		}
	
	


}

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		n++;
		
		bezier();
		break;
	case GLUT_KEY_DOWN:
		n--;
		bezier();
		break;
	}

	glutPostRedisplay();
}



void showMenu()
{
	glutDestroyMenu(menuPrincipal);
	if (isDrawing == 0)
	{
		menuPrincipal = glutCreateMenu(mainMenuCallback);

		glutAddMenuEntry("Points de controle", 1);
	}
	else if (isDrawing == 1 && bezierfin == 0)
	{
		menuPrincipal = glutCreateMenu(drawingMenuCallback);
		glutAddMenuEntry("Terminer le tracé", 1);
		glutAddMenuEntry("Annuler le tracé", 2);
	}
	else
	{
		if (bezierfin == 1 && isRotating == 0 && isTranslating == 0)
		{
			menuPrincipal = glutCreateMenu(transformMenuCallback);
			glutAddMenuEntry("Rotation", 1);
			glutAddMenuEntry("Translation", 2);
		}
		if (bezierfin == 1 && isRotating == 1 && isTranslating == 0)
		{
			menuPrincipal = glutCreateMenu(rMenuCallback);
			glutAddMenuEntry("Terminer le tracé", 1);
			glutAddMenuEntry("Annuler le tracé", 2);
		}
		if (bezierfin == 1 && isRotating == 0 && isTranslating == 1)
		{
			menuPrincipal = glutCreateMenu(tMenuCallback);
			glutAddMenuEntry("Terminer le tracé", 1);
			glutAddMenuEntry("Annuler le tracé", 2);
		}

	}

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mainMenuCallback(int menuItem)
{


	isDrawing = 1;
	showMenu();
}


void drawingMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		bezier();
		dessinBezier();
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		points = neew;
		glEnd();
		glutSwapBuffers();
		break;
	}
	showMenu();
}

void transformMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		isRotating = 1;
		isTranslating = 0;
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		isRotating = 0;
		isTranslating = 1;
		glEnd();
		glutSwapBuffers();
		break;
	}
	showMenu();
}

void tMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		isTranslating = 0;
		translating();
		dessinBezier();
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		translate = neew;
		isTranslating = 0;
		glEnd();
		glutSwapBuffers();
		break;
	}
	showMenu();
}

void rMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		isRotating = 0;
		rotating();
		dessinBezier();
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		rotate = neew;
		isRotating = 0;
		glEnd();
		glutSwapBuffers();
		break;
	}
	showMenu();
}

void translating()
{
	Point vecTranslate;
	
	vecTranslate.x = translate[translate.size()-1].x- translate[translate.size() - 2].x;
	vecTranslate.y = translate.at(translate.size() - 1).y - translate.at(translate.size() - 2).y;

	for (int i = 0; i < points.size(); i++)
	{
		points.at(i).x += vecTranslate.x;
		points.at(i).y += vecTranslate.y;
	}
	bezier();

}

void rotating()
{
	Point oa;
	Point ob;
	Point tmpp;
	float co;
	float theta;

	oa.x = rotate[rotate.size() - 2].x;
	oa.y = rotate[rotate.size() - 2].y;
	
	ob.x = rotate[rotate.size() - 1].x;
	ob.y = rotate[rotate.size() - 1].y;
	
	co = (oa.x*ob.x + oa.y*ob.y) / (sqrt(pow(oa.x, 2) + pow(oa.y, 2))*sqrt(pow(ob.x, 2) + pow(ob.y, 2)));
	theta= acos(co);
	std::cerr << theta * 180 / PI << " "<< theta<<std::endl;
	std::vector<Point> tab;
	for (int j = 0; j < points.size(); j++)
	{
		tab.push_back(points[j]);
	}

	for (int k = 0; k < theta * 180 / PI; k++)
	{
		float t= k * PI / 180.0f;
		for (int i = 0; i < points.size(); i++)
		{
			tmpp.x = tab[i].x;
			tmpp.y = tab[i].y;
			points.at(i).x = cos(t)*tmpp.x - sin(t)*tmpp.y;
			points.at(i).y = sin(t)*tmpp.x + cos(t)*tmpp.y;



		}
		bezier();
		dessinBezier();
		Sleep(10);
	}
	

}