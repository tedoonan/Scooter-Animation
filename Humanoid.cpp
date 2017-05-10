//  COSC363 Assignment 1
//  Thomas Doonan

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
#include "loadBMP.h"
#define GL_CLAMP_TO_EDGE 0x812F
using namespace std;

const float floorHeight = 0.3;
const float railHeight = 2.3;

//--Globals ---------------------------------------------------------------
int cam_hgt = 4; //Camera height
float angle = 160;  //Rotation angle for viewing
float theta = 0;
bool up = false;
GLUquadricObj*	q;

float ground = floorHeight;

float accelY = 0;
float velocityY = 0;
float velocityZ = 0.25;

float robotX = 0;
float robotY = ground;
float robotZ = -9.5;

float boardAngle = 0;
float spinAngle = 0;
float baseAngle = 0;

float camX = -8;
float camY = 5;
float camZ = -10;

float merryAngle = 0;
float ballY = 1;
float ballSpeed = 0;
float ballAccel = -0.04;
float birdAngle = 0;
float wingAngle = 0;
int flapDirection = 1;

float white[4] = { 1., 1., 1., 1. };
float black[4] = { 0 };
float grey[4] = { 0.5, 0.5, 0.5, 0.5 };

bool isFirstPerson = false;

float legRotation = 0;
bool isPushing = false;

GLuint txId[8];

//--Draws a grid of lines on the floor plane -------------------------------
void drawFloor()
{
	glColor3f(0., 0.5, 0.);			//Floor colour
	for(float i = -50; i <= 50; i ++)
	{
		glBegin(GL_LINES);			//A set of grid lines on the xz-plane
			glVertex3f(-50, 0, i);
			glVertex3f(50, 0, i);
			glVertex3f(i, 0, -50);
			glVertex3f(i, 0, 50);
		glEnd();
	}
}

void loadTexture()
{
	glGenTextures(9, txId); 				
	glBindTexture(GL_TEXTURE_2D, txId[0]);		
	loadBMP("ball.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, txId[1]);	
	loadBMP("plywood.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBindTexture(GL_TEXTURE_2D, txId[2]);	
	loadBMP("no.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Skybox

	glBindTexture(GL_TEXTURE_2D, txId[3]);		
	loadBMP("front.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[4]);		
	loadBMP("right.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[5]);		
	loadBMP("back.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadBMP("left.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[7]);
	loadBMP("top.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void floor()
{
	glColor4f(0.7, 0.7, 0.7, 1.0); 
	glNormal3f(0.0, 1.0, 0.0);

	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	glBegin(GL_QUADS);
	for (float i = -100; i < 100; i+=0.5)
	{
		for (float j = -100; j < 100; j+=0.5)
		{
			glVertex3f(i, 0.0, j);
			glVertex3f(i, 0.0, j + 0.5);
			glVertex3f(i + 0.5, 0.0, j + 0.5);
			glVertex3f(i + 0.5, 0.0, j);
		}
	}
	glEnd();
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

void drawFence()
{
	glColor4f(0.5, 0.5, 0.5, 1.0); 
	glNormal3f(-1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	for (float i = -25; i < 25; i += 1)
	{
		for (float j = 0; j < 5; j += 1)
		{
			glTexCoord2f(.5, 0.5);	glVertex3f(25, j, i);
			glTexCoord2f(.8, 0.5);	glVertex3f(25, j + 1, i);
			glTexCoord2f(.8, 0.8);	glVertex3f(25, j + 1, i + 1);
			glTexCoord2f(0.5, 0.8);	glVertex3f(25, j, i + 1);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawSkybox()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, txId[3]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0); 
	glTexCoord2f(0, 0);	 glVertex3f(-500, -500.0, 500.0);
	glTexCoord2f(1, 0); glVertex3f(500.0, -500.0, 500.0);
	glTexCoord2f(1, 1); glVertex3f(500.0, 500.0, 500.0);
	glTexCoord2f(0, 1); glVertex3f(-500.0, 500.0, 500.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, txId[5]);
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0, 0); glVertex3f(500.0, -500.0, -500.0);
	glTexCoord2f(1, 0); glVertex3f(-500.0, -500.0, -500.0);
	glTexCoord2f(1, 1); glVertex3f(-500.0, 500.0, -500.0);
	glTexCoord2f(0, 1); glVertex3f(500.0, 500.0, -500.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, txId[7]);
	glBegin(GL_QUADS);
	glNormal3f(0.0, -1.0, 0.0); 
	glTexCoord2f(0, 0); glVertex3f(500.0, 500.0, -500.0);
	glTexCoord2f(1, 0); glVertex3f(-500.0, 500.0, -500.0);
	glTexCoord2f(1, 1); glVertex3f(-500.0, 500.0, 500.0);
	glTexCoord2f(0, 1); glVertex3f(500.0, 500.0, 500.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, txId[4]);
	glBegin(GL_QUADS);
	glNormal3f(-1., 0., 0.);
	glTexCoord2f(0, 0); glVertex3f(-500.0, -500.0, -500.0); 
	glTexCoord2f(1, 0); glVertex3f(-500.0, -500.0, 500.0);
	glTexCoord2f(1, 1); glVertex3f(-500.0, 500.0, 500.0);
	glTexCoord2f(0, 1); glVertex3f(-500.0, 500.0, -500.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, txId[6]);
	glBegin(GL_QUADS);
	glNormal3f(1.0, 0.0, 0.0);  
	glTexCoord2f(0, 0); glVertex3f(500.0, -500.0, 500.0);
	glTexCoord2f(1, 0); glVertex3f(500.0, -500.0, -500.0);
	glTexCoord2f(1, 1); glVertex3f(500.0, 500.0, -500.0);
	glTexCoord2f(0, 1); glVertex3f(500.0, 500.0, 500.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawBall() {
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor3f(1., 0.5, 0.);
	glPushMatrix();
	  glTranslatef(5, ballY, -3);
	  gluSphere(q, 1, 32, 32);
	glPopMatrix();
}

void moveBall() {
	
	ballSpeed += ballAccel;
	ballY += ballSpeed;

	if (ballY <= 1) {
		ballSpeed = 1;
	}
}

void drawRobot() {

	glPushMatrix();
	  glTranslatef(0, -3, 0);
	  glColor3f(0., 1., 1.);

	glPushMatrix();
	  glTranslatef(-0.8, 6.7, 0.5);
	  glRotatef(20, 1, 0, 0);
	  glScalef(0.5, 0.5, 1.6);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 1., 1.);
	glPushMatrix();
	  glTranslatef(0.8, 6.7, 0.5);
	  glRotatef(20, 1, 0, 0);
	  glScalef(0.5, 0.5, 1.6);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(1., 1., 0.);			//Head
	glPushMatrix();
	  glTranslatef(0, 8, 0);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(1., 0., 0.);			//Torso
	glPushMatrix();
	  glTranslatef(0, 6, 0);
	  glScalef(1.5, 2.5, 0.6);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 1., 1.);
	glPushMatrix();
	  glTranslatef(0.3, 4, 0);
	  glRotatef(-10, 0, 0, 1);
	  glScalef(0.5, 2, 0.5);
	  glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	  
	  if (isPushing) {
		  glTranslatef(-0.3, 4, 0);
		  glTranslatef(0, 1, 0);
		  glRotatef(legRotation, 1, 0, 0);
		  glTranslatef(0, -1, 0);
	  }
	  else {
		  glTranslatef(-0.3, 4, -0.4);
		  glRotatef(-20, 0, 1, 0);
		  glRotatef(20, 1, 0, 0);
	  }
	  glScalef(0.5, 2, 0.5);
	  
	  glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
}

void drawWheel() {
	glColor3f(.1, 0.1, 0.1);
	glRotatef(90, 0, 1, 0);
	glutSolidCylinder(0.3, 0.3, 16, 8);
}

void drawScooter() {

	glPushMatrix();
	  
	  glTranslatef(robotX, robotY, robotZ);
	  glRotatef(boardAngle, 0, 1, 0);
	  glRotatef(spinAngle, 0, 1, 0);

	  drawRobot();
	  glColor3f(1., 0.5, 0.);

	  glPushMatrix(); //Handle bar
	    glTranslatef(-1, 3.5, 1);
	    glRotatef(90, 0, 1, 0);
	    glutSolidCylinder(0.1, 2, 16, 8);
	  glPopMatrix();

	  glPushMatrix();  //Upwards bar
	    glTranslatef(0, 0, 1);
	    glRotatef(270, 1, 0, 0);
	    glutSolidCylinder(0.1, 3.5, 16, 8);
	  glPopMatrix();

	  glPushMatrix();
	    glTranslatef(-0.2, 0, 1);
		drawWheel();
	  glPopMatrix();

	  glPushMatrix();
	  
	    glTranslatef(0, 0, 1);
	    glRotatef(baseAngle, 0, 1, 0);
	    glTranslatef(0, 0, -1);

	    glPushMatrix();
	      glTranslatef(-0.2, 0, -1.3);
	      drawWheel();
	    glPopMatrix();
		glColor3f(1., 0.5, 0.);
	    glScalef(0.7, 0.3, 2);
	    glutSolidCube(1);
	  glPopMatrix();
	glPopMatrix();
}

void drawRail() {

	glPushMatrix();
	
	glTranslatef(15, 0, -9);
	glRotatef(30, 0, 1, 0);
	glScalef(1.5, 1.5, 1.5);

	glPushMatrix(); //Upright
	  glColor3f(1., 1., 1.);
	  glTranslatef(0, 1.8, -2);
	  glRotatef(90, 1, 0, 0);
	  glutSolidCylinder(0.2, 1.8, 16, 8);
	glPopMatrix();

	glPushMatrix(); //Upright
	  glColor3f(1., 1., 1.);
	  glTranslatef(0, 1.8, 2);
	  glRotatef(90, 1, 0, 0);
	  glutSolidCylinder(0.2, 1.8, 16, 8);
	glPopMatrix();
	
	glPushMatrix(); //Crossbar
	  glColor3f(1., 1., 1.);
	  glTranslatef(0, 1.80, -3);
	  glutSolidCylinder(0.2, 6, 16, 8);
	glPopMatrix();

	glPopMatrix();
}

void drawSign()
{
	glColor3f(0.6, 0.4, 0.4);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	glBegin(GL_QUADS);

	glNormal3f(1.0, 0.0, 0.0);   
	glTexCoord2f(0, 0); glVertex3f(0.1, 0, -3);
	glTexCoord2f(1, 0); glVertex3f(0.1, 0, 3);
	glTexCoord2f(1, 1); glVertex3f(0.1, 2, 3);
	glTexCoord2f(0, 1); glVertex3f(0.1, 2, -3);
	
	glTexCoord2f(0, 0); glVertex3f(-0.1, 0, -3);
	glTexCoord2f(1, 0); glVertex3f(-0.1, 0, 3);
	glTexCoord2f(1, 1); glVertex3f(-0.1, 2, 3);
	glTexCoord2f(0, 1); glVertex3f(-0.1, 2, -3);
	glDisable(GL_TEXTURE_2D);

	glNormal3f(0.0, 1.0, 0.0);   
	glVertex3f(-0.1, 2, 3);
	glVertex3f(0.1, 2, 3);
	glVertex3f(0.1, 2, -3);
	glVertex3f(-0.1, 2, -3);

	glNormal3f(0, 0., -1.);
	glVertex3f(-0.1, 0.0, -3);  
	glVertex3f(0.1, 0.0, -3);
	glVertex3f(0.1, 2, -3);
	glVertex3f(-0.1, 2, -3);

	glNormal3f(0, 0., 1.);
	glVertex3f(-0.1, 0.0, 3);   
	glVertex3f(0.1, 0.0, 3);
	glVertex3f(0.1, 2, 3);
	glVertex3f(-0.1, 2, 3);
	glEnd();

	
}

void handrail() {
	glPushMatrix(); // pole
	glColor3f(0., 1., 1.);
	glTranslatef(0, 3.3, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCylinder(0.2, 4.5, 32, 8);
	glPopMatrix();

	glPushMatrix(); // pole
	glColor3f(0., 1., 1.);
	glTranslatef(4.3, 3.3, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidCylinder(0.2, 2, 32, 8);
	glPopMatrix();
}

void moveBird() {
	birdAngle --;
	if (wingAngle > 20 || wingAngle < -20) {
		flapDirection = -flapDirection;
	}
	wingAngle += 1.5 * flapDirection;
}

void drawBird() {

	glMaterialfv(GL_FRONT, GL_SPECULAR, grey);
	glPushMatrix();
	glTranslatef(5, 0, 0);
	  glRotatef(birdAngle, 0, 1, 0);
	  glTranslatef(12, 8, 0);
	  glColor3f(0.2, 0.1, 0);
	  

	  glPushMatrix();
	    glTranslatef(0.5, 3.3, 0);
		glRotatef(20 - wingAngle, 0, 0, 1);
		glTranslatef(0.5, 0, 0);
	    glScalef(1.7, 0.3, 1);
	    glutSolidSphere(0.5, 16, 16);
	  glPopMatrix();

	  glPushMatrix();
	    glTranslatef(-0.5, 3.3, 0);
		glRotatef(wingAngle, 0, 0, 1);
		glTranslatef(-0.5, 0, 0);
	    glScalef(1.7, 0.3, 1);
	    glutSolidSphere(0.5, 16, 16);
	  glPopMatrix();

	  glColor3f(0.7, 0.4, 0.2);

	  glPushMatrix(); //Head
	    
	    glTranslatef(0, 3.3, .9);
	    glScalef(0.8, .8, 1);
	    glutSolidSphere(0.5, 16, 16);
	  glPopMatrix();

	  glPushMatrix(); //Body
	    glTranslatef(0, 3, 0);
	    glScalef(0.5, 0.5, 1);
	    glutSolidSphere(1, 16, 16);
	    glPopMatrix();
	  glPopMatrix();

	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

void merryGoRound() {
	glPushMatrix();
	
	  glTranslatef(8, 0, 3);
	  glRotatef(merryAngle, 0, 1, 0);

	  for (int i = 0; i < 4; i++) {
		  glPushMatrix();
		  glRotatef(i * 90, 0, 1, 0);
		  handrail();
		  glPopMatrix();
	  }

	  glPushMatrix(); //stand
	    glColor3f(1., 1., 0.);
	    glTranslatef(0, 4, 0);
	    glRotatef(90, 1, 0, 0);
	    glutSolidCylinder(0.4, 4, 32, 8);
	  glPopMatrix();

	  glPushMatrix(); //base
	    glColor3f(1., 0., 0.);
	    glTranslatef(0, 1.5, 0);
	    glRotatef(90, 1, 0, 0);
	    glutSolidCylinder(5, 0.3, 48, 16);
	  glPopMatrix();
	glPopMatrix();
}

void trick(float accel) {
	if (robotZ > -8 && robotZ < -2.5 && accel != 0) {
		spinAngle = spinAngle + 5;
	}
	else if (robotZ > 2.5) {
		if (accel != 0) {
			spinAngle = spinAngle - 5.5;
			baseAngle = baseAngle + 17;
		}
		else {
			spinAngle = 0;
			baseAngle = 0;
		}
	}
}

void push() {
	if (robotX == 20 && robotZ > 8) {
		if (!isPushing) {
			isPushing = true;
			legRotation = -20;
		}
	}
	legRotation++;
	if (legRotation == 20) {
		isPushing = false;
	}
}

void myTimer(int value) {

	moveBall();
	merryAngle--;
	moveBird();
	push();
	

	if (robotZ >= 10) {
		boardAngle += 1.25;
		robotX = 10 - 10 * cos(boardAngle * M_PI / 180);
		robotZ = 10 + 10 * sin(boardAngle * M_PI / 180);
		if (velocityZ > 0) {
			velocityZ = -velocityZ;
		}
	}

	else if (robotZ < -10) {
		boardAngle += 1.25;
		robotX = 10 - 10 * cos(boardAngle * M_PI / 180);
		robotZ = -10 + 10 * sin(boardAngle * M_PI / 180);
		if (velocityZ < 0) {
			velocityZ = -velocityZ;
		}
	}

	else {
		if (robotX < 1) {
			trick(accelY);
			if (robotZ == -8) {
				velocityY = 0.23;
			}
			else if (robotZ == -4) {
				ground = railHeight;
			}
			else if (robotZ == 2.5) {
				velocityY = 0.03;
				ground = floorHeight;
			}
		}
		robotZ += velocityZ;
	}

	velocityY += accelY;
	robotY += velocityY;

	if (robotY > ground) {
		accelY = -0.013; //Gravity
	}
	else if (robotY < ground) {
		robotY = ground;
		accelY = 0;
		velocityY = 0;
	}

	

	glutPostRedisplay();
	glutTimerFunc(25, myTimer, 0);
}


//--Display: ---------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display()  
{
	float lpos[4] = {10., 10., 20., 1.0};  //light's position
	float lgt_pos1[] = { 10, 50, 0, 1.0f };
	float spotdir[] = { robotX - 10, -50, robotZ };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (isFirstPerson) {
		glRotatef(-boardAngle - spinAngle, 0, 1, 0);
		gluLookAt(robotX, robotY + 5, robotZ, robotX, robotY + 5, robotZ + 10, 0, 1, 0); //First person
	}
	else {
		glRotatef(angle, 0, 1, 0);
		glTranslatef(-camX, -camY, -camZ);
	}
	
	glLightfv(GL_LIGHT0,GL_POSITION, lpos);   //Set light position

	glLightfv(GL_LIGHT1, GL_POSITION, lgt_pos1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);

	drawBall();

	glDisable(GL_LIGHTING);			//Disable lighting when drawing floor.
	drawSkybox();
	glEnable(GL_LIGHTING);	       //Enable lighting when drawing the model

	drawFence();
	floor();
	
	drawBird();
	drawScooter();
	drawRail();
	merryGoRound();
	
	drawSign();
	
	glutSwapBuffers();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
	q = gluNewQuadric();
	loadTexture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//Background colour

	float lgt_amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	float lgt_dif[4] = { 1.0, 1.0, 1.0, 1.0 };
	float lgt_spe[4] = { 1.0, 1.0, 1.0, 1.0 };

	float mat_amb[4] = { 0.0, 0.0, 1.0, 1.0 };
	float mat_dif [4] = { 0.0, 0.0, 1.0, 1.0 };
	float mat_spe[4] = { 1.0, 1.0, 1.0, 1.0 };
	
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, lgt_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lgt_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lgt_spe);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lgt_amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lgt_dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lgt_spe);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 7.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.01);

	glMaterialfv(GL_FRONT, GL_AMBIENT,mat_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,mat_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spe);
	glMaterialf(GL_FRONT, GL_SHININESS, 40);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(q, GL_TRUE);
 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 1000.0);   //Camera Frustum
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene
void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) angle -= 5;
    else if(key == GLUT_KEY_RIGHT) angle += 5;
	else if (key == GLUT_KEY_UP) {
		camX += sin(angle * M_PI / 180);
		camZ -= cos(angle * M_PI / 180);
	}
	else if (key == GLUT_KEY_DOWN) {
		camX -= sin(angle * M_PI / 180);
		camZ += cos(angle * M_PI / 180);
	}
	else if (key == GLUT_KEY_F1) {
		isFirstPerson = !isFirstPerson;
	}

    glutPostRedisplay();
}

//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize (800, 800); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("Robot");
   initialize();
   glutTimerFunc(50, myTimer, 0);
   glutDisplayFunc(display);
   glutSpecialFunc(special); 
   glutMainLoop();
   return 0;
}
