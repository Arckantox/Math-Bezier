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

float r0;
float r1;



typedef struct Point
{
	float x;
	float y;
} Point;


std::vector<Point> points;
std::vector<Point> points1;
std::vector<Point> p;
std::vector<Point> p1;

int bezier2;
int typeRaccord;
int isTransvecting;

void bezier1();
void raccord();
int n = 10;
int menuPrincipal = 0;
int isDrawing;
int bezierfin;
std::vector<Point> neew;
int isRotating;
int isTranslating;
std::vector<Point> translate;
std::vector<Point> rotate;
std::vector<Point> transvect;
int isCreating2ndCurve;
void dessinBezier();
void bezier();
void mouse(int button, int state, int x, int y);
void SpecialInput(int key, int x, int y);
void showMenu();
void mainMenuCallback(int);
void drawingMenuCallback(int);
void secondBezierMenuCallback(int);
void transformMenuCallback(int);
void rMenuCallback(int);
void tMenuCallback(int);
void cMenuCallback(int);
void translating();
void rotating();
void mouse_activeFunc(int, int);
void raccordBezierMenuCallback(int);
int inBezier2;
void colorMenuCallBack(int);
int colorSubMenu;
std::vector<float> r;
std::vector<float> g;
std::vector<float> b;
float colorr=0.0f;
float colorg = 0.0f;
float colorb = 0.0f;
void transvecting();

int main(int argc, char **argv)
{
	r.push_back(0.0f);

	g.push_back(0.0f);

	b.push_back(0.0f);

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
	glutMotionFunc(mouse_activeFunc);
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
	glColor3f(colorr, colorg,colorb);
	
		for (int k = 0; k < p.size(); k++)
		{
			
			glBegin(GL_POLYGON);
			glBegin(GL_LINE_STRIP);
			
			glVertex2f(p[k].x, p[k].y);

		}

		glEnd();
		glutSwapBuffers();
		glColor3f(1.0f, 0, 0.0f);
		for (int i = 0; i < points1.size(); i++)
		{
			glBegin(GL_POINTS);
			glVertex2f(points1[i].x, points1[i].y);
		}
		glEnd();
		glutSwapBuffers();
		if(r.size()>=1)
		glColor3f(r[r.size()-1], g[g.size() - 1], b[b.size() - 1]);

		for (int k = 0; k < p1.size(); k++)
		{

			glBegin(GL_POLYGON);
			glBegin(GL_LINE_STRIP);

			glVertex2f(p1[k].x, p1[k].y);

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

void mouse_activeFunc(int x, int y)
{
	float delta=2.0f;
	for (int i = 0; i < points.size(); i++)
	{
		if (x - 250.0f <= points[i].x + delta && x - 250.0f >= points[i].x - delta && -y + 250.0f <= points[i].y + delta &&-y + 250.0f >= points[i].y - delta)
		{
			points[i].x = x - 250.0f;

			points[i].y = -y + 250.0f;
			bezier();
			dessinBezier();

		}
	}
	for (int i = 0; i < points1.size(); i++)
	{
		if (x - 250.0f <= points1[i].x + delta && x - 250.0f >= points1[i].x - delta && -y + 250.0f <= points1[i].y + delta &&-y + 250.0f >= points1[i].y - delta)
		{
			points1[i].x = x - 250.0f;

			points1[i].y = -y + 250.0f;
			bezier1();
			dessinBezier();

		}
	}
}

void mouse(int button, int state, int x, int y)
{
	Point tmp;

	// Si on appuie sur le bouton de gauche

	
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && bezierfin == 0 && isDrawing == 1)
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
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP &&bezierfin == 1 && isTransvecting == 1)
			{
				tmp.x = x - 250.0f;
				tmp.y = -y + 250.0f;
				transvect.push_back(tmp);
			}
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP &&bezierfin == 1 && inBezier2 == 1)
			{
				
				tmp.x = x - 250.0f;
				tmp.y = -y + 250.0f;
				points1.push_back(tmp);
				Point tmp1;
				if (points1.size() == 1)
				{

					switch (typeRaccord)
					{
						case 1:
							std::cout << "Enter r0 and r1 :" << std::endl;
							std::cin >> r0 >> r1;
							std::cout << r0 << std::endl;
							std::cout << r1 << std::endl;
							tmp1.x = points1[0].x + (points[points.size()-1].x-points[points.size()-2].x)*r1/r0;
							tmp1.y = points1[0].y + (points[points.size() - 1].y - points[points.size() - 2].y)*r1 / r0;
							points1.push_back(tmp1);
							break;
						case 2:
							std::cout << "Enter r0 and r1 :" << std::endl;
							std::cin >> r0 >> r1;
							std::cout << r0 << std::endl;
							std::cout << r1 << std::endl;
							tmp1.x = points1[0].x + (points[points.size() - 1].x - points[points.size() - 2].x)*r1 / r0;
							tmp1.y = points1[0].y + (points[points.size() - 1].y - points[points.size() - 2].y)*r1 / r0;
							points1.push_back(tmp1);

							Point D;
							D.x = points[points.size() - 2].x + (points[points.size() - 2].x - points[points.size() - 3].x)*r1 / r0 - points[points.size() - 1].x + points1[0].x;
							D.y = points[points.size() - 2].y + (points[points.size() - 2].y - points[points.size() - 3].y)*r1 / r0 - points[points.size() - 1].y + points1[0].y;

							Point tmp2;
							tmp2.x = points1[1].x + (points1[1].x - D.x)*r1 / r0;
							tmp2.y = points1[1].y + (points1[1].y - D.y)*r1 / r0;
							points1.push_back(tmp2);

							break;
					}
				}
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
		if(bezier2==1)bezier1();
		break;
	case GLUT_KEY_DOWN:
		n--;
		bezier();
		if (bezier2 == 1)bezier1();
		break;
	case GLUT_KEY_SHIFT_L:
		for (int i = 0; i < points.size(); i++)
		{
			points[i].x *= 1.1;
			points[i].y *=1.1;
		}
		for (int i = 0; i < points1.size(); i++)
		{
			points1[i].x *= 1.1;
			points1[i].y *= 1.1;
		}
		bezier();
		if (bezier2 == 1)bezier1();
		break;
	case GLUT_KEY_CTRL_L:
		for (int i = 0; i < points.size(); i++)
		{
			points[i].x /=1.1;
			points[i].y /= 1.1;
		}
		for (int i = 0; i < points1.size(); i++)
		{
			points1[i].x /= 1.1;
			points1[i].y /= 1.1;
		}
		bezier();
		if (bezier2 == 1)bezier1();
		break;
	}

	glutPostRedisplay();
}

void showMenu()
{
	glutDestroyMenu(menuPrincipal);
	if (isDrawing == 0)
	{
		
		colorSubMenu = glutCreateMenu(colorMenuCallBack);
		glutAddMenuEntry("Blanc", 11);
		glutAddMenuEntry("Jaune", 12);
		glutAddMenuEntry("Vert", 13);
		glutAddMenuEntry("Noir", 14);
		glutAddMenuEntry("Bleu", 15);
		glutAddMenuEntry("Orange", 16);
		glutAddMenuEntry("Violet", 17);

		menuPrincipal = glutCreateMenu(mainMenuCallback);

		glutAddSubMenu("Couleur", colorSubMenu);
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
			glutAddMenuEntry("Transvection", 3);
			if (bezier2 == 0)
			{
			glutAddSubMenu("Couleur", colorSubMenu);
			glutAddMenuEntry("Tracer C0", 4);
			glutAddMenuEntry("Tracer C1", 5);
			glutAddMenuEntry("Tracer C2", 6);
			}
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
		if (bezierfin == 1 && isRotating == 0 && isTranslating == 0 && isTransvecting==1)
		{
			menuPrincipal = glutCreateMenu(cMenuCallback);
			glutAddMenuEntry("Terminer le tracé", 1);
			glutAddMenuEntry("Annuler le tracé", 2);
		}
		if (bezierfin == 1 && bezier2 == 0 && isRotating == 0 && isTranslating == 0 && inBezier2 == 1) {
			
			menuPrincipal = glutCreateMenu(secondBezierMenuCallback);
			
			glutAddMenuEntry("Terminer le tracé", 1);
			glutAddMenuEntry("Annuler le tracé", 2);
		}
		if (bezierfin == 1 && bezier2 == 1 && isRotating == 0 && inBezier2 == 1) {
			menuPrincipal = glutCreateMenu(raccordBezierMenuCallback);
			glutAddMenuEntry("Raccord", 1);
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
		if (inBezier2 == 0) {
			colorr = r[r.size() - 1];
			colorg = g[g.size() - 1];
			colorb = b[b.size() - 1];
		}
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
		isTransvecting = 0;
		isRotating = 1;
		isTranslating = 0;
		isCreating2ndCurve = 0;
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		isTransvecting = 0;
		isRotating = 0;
		isTranslating = 1;
		isCreating2ndCurve = 0;
		glEnd();
		glutSwapBuffers();
		break;
	case 4:
		inBezier2 = 1;
		isRotating = 0;
		isTranslating = 0;
		isTransvecting = 0;
		isCreating2ndCurve = 1;
		typeRaccord = 0;
		glEnd();
		glutSwapBuffers();
		break;
	case 5:
		isTransvecting = 0;
		inBezier2 = 1;
		isRotating = 0;
		isTranslating = 0;
		isCreating2ndCurve = 1;
		typeRaccord = 1;
		glEnd();
		glutSwapBuffers();
		break;
	case 6:
		isTransvecting = 0;
		inBezier2 = 1;
		isRotating = 0;
		isTranslating = 0;
		isCreating2ndCurve = 1;
		typeRaccord = 2;
		glEnd();
		glutSwapBuffers();
		break;
	case 3:
		isRotating = 0;
		isTranslating = 0;
		isTransvecting = 1;
		isCreating2ndCurve = 0;
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

void cMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		isTransvecting = 0;
		transvecting();
		dessinBezier();
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		transvect = neew;
		isTransvecting = 0;
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




void secondBezierMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		bezier1();
		dessinBezier();
		glEnd();
		glutSwapBuffers();
		break;
	case 2:
		points1 = neew;
		glEnd();
		glutSwapBuffers();
		break;
	}
	showMenu();
}

void raccordBezierMenuCallback(int menuItem)
{
	switch (menuItem)
	{
	case 1:
		raccord();
		dessinBezier();
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
	for (int i = 0; i < points1.size(); i++)
	{
		points1.at(i).x += vecTranslate.x;
		points1.at(i).y += vecTranslate.y;
	}
	bezier();
	if (bezier2 == 1) bezier1();
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
	std::vector<Point> tab,tab1;
	for (int j = 0; j < points.size(); j++)
	{
		tab.push_back(points[j]);
		
	}

	for (int j = 0; j < points1.size(); j++)
	{
		tab1.push_back(points1[j]);

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
	
		for (int i = 0; i < points1.size(); i++)
		{
			tmpp.x = tab1[i].x;
			tmpp.y = tab1[i].y;
			points1.at(i).x = cos(t)*tmpp.x - sin(t)*tmpp.y;
			points1.at(i).y = sin(t)*tmpp.x + cos(t)*tmpp.y;



		}
		if(bezier2==1) bezier1();
		bezier();
		dessinBezier();
		Sleep(10);
	}
	

}

void raccord()
{
	inBezier2 = 0;
	int i = points.size();
	for (int j = 1; j < points1.size(); j++)
	{
		Point tmp;
		tmp.x = points1[j].x + points[i - 1].x - points1[0].x;

		tmp.y = points1[j].y + points[i - 1].y - points1[0].y;
		
		points1[j] = tmp;
	}

	

	for (int j = 0; j < p1.size(); j++)
	{
		Point tmp;
		tmp.x = p1[j].x + points[i - 1].x - points1[0].x;

		tmp.y = p1[j].y + points[i - 1].y - points1[0].y;

		p1[j] = tmp;
	}
	points1[0].x = points[i - 1].x;

	points1[0].y = points[i - 1].y;

	dessinBezier();
}

void bezier1() {
	
		float t;
		Point tmp[50];

		p1 = neew;
		for (int k = 0; k < n; k++)
		{
			for (int ij = 0; ij < points1.size(); ij++)
			{
				tmp[ij] = points1[ij];
			}


			t = (float)k / n;
			for (int j = 1; j <= points1.size() - 1; j++)
			{
				for (int i = 0; i <= points1.size() - 1 - j; i++)
				{
					tmp[i].x = (1 - t)*tmp[i].x + t*tmp[i + 1].x;
					tmp[i].y = (1 - t)*tmp[i].y + t*tmp[i + 1].y;

				}
			}

			p1.push_back(tmp[0]);


		}


		p1.push_back(points1.at(points1.size() - 1));
		bezier2 = 1;

	
}

void colorMenuCallBack(int a)
{
	
		switch (a)
		{
		case 12:
			
			r.push_back(1.0);
			g.push_back(1.0);
			b.push_back(0.0);
			break;
		case 13:
			
			r.push_back(0.0f);
			g.push_back(1.0f);
			b.push_back(0.0f);
			break;
		case 14:
			r.push_back(0.0f);
			g.push_back(0.0f);
			b.push_back(0.0f);
			break;
		case 15:
			r.push_back(0.0f);
			g.push_back(0.0f);
			b.push_back(1.0f);
			break;
		case 16:
			r.push_back(1.0f);
			g.push_back(0.5f);
			b.push_back(0.0f);
			break;
		case 17:
			r.push_back(1.0f);
			g.push_back(0.0f);
			b.push_back(1.0f);
			break;
		default:
			r.push_back(1.0f);
			g.push_back(1.0f);
			b.push_back(1.0f);
			break;
		}
	}


void transvecting()
{
		Point vecTranslate;
		float tmp;
		vecTranslate.x = transvect[transvect.size() - 1].x - transvect[transvect.size() - 2].x;
		vecTranslate.y = transvect.at(transvect.size() - 1).y - transvect.at(transvect.size() - 2).y;

		for (int i = 0; i < points.size(); i++)
		{
			tmp = points.at(i).x;
			points.at(i).x = points.at(i).x + vecTranslate.y/500*points.at(i).y;
			points.at(i).y = points.at(i).y+ vecTranslate.x/500*tmp;
		}
		for (int i = 0; i < points1.size(); i++)
		{
			tmp = points1.at(i).x;
			points1.at(i).x = points1.at(i).x + vecTranslate.y/500*points1.at(i).y;
			points1.at(i).y = points1.at(i).y + vecTranslate.x/500*tmp;
		}
		bezier();
		if (bezier2 == 1) bezier1();
	

}