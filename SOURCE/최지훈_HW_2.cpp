#include <Windows.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "glSetup.h"

#include "include/GLFW/glfw3.h"
#include "include/GL/gl.h"
#include "include/GL/glut.h"

#include "include/glm/glm.hpp" //OpenGl Mathematics
#include "include/glm/gtc/type_ptr.hpp" //glm::value_ptr()
#include "include/glm/gtc/matrix_transform.hpp"

#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/opengl32.lib")
#pragma comment(lib, "lib/glut32.lib")

using namespace glm;

#include <iostream>
using namespace std;

//Camera configuration
vec3 eye(4, 4, 4);
vec3 center(0, 0, 0);
vec3 up(0, 1, 0);

//Global coordinate frame
float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

//Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

//Selected example
int selection = 1;

vec4 p[36][18];

//Light configuration
vec4 lightInitialP(2.0, 2.0, 0.0, 1); //initial light position

float thetaModel = 0; //Rotation angle around the y-axis
float thetaLight[3];

bool lightOn[3]; //Point = 0, distant = 1, spot = 2 lights

bool exponent = false;
float exponentInitial = 0.0; //[0,128]
float exponentValue = exponentInitial;
float exponentNorm = exponentValue / 128.0; //[0,1]

bool cutoff = false;
float cutoffMax = 60; //[0,90] degree
float cutoffInitial = 30.0; //[0, cutoffMax] degree
float cutoffValue = cutoffInitial;
float cutoffNorm = cutoffValue / cutoffMax; //[0,1]

//Paly configuration
bool pause = true;

float timeStep = 1.0 / 120; //120 fps. 60 fpps using vsync = 1
float period = 4.0;

//Current frame
int frame = 0;

//for shininess
bool shininess = false;
float shininessInitial = 0.0; //[0,128]
float shininessValue = shininessInitial;
float shininessNorm = shininessValue / 128.0; //[0,1]

void
reinitialize()
{
	frame = 0;

	lightOn[0] = false;
	lightOn[1] = false;
	lightOn[2] = false;

	thetaModel = 0;
	for (int i = 0; i<3; i++)
	{
		thetaLight[i] = 0;
	}

	exponentValue = exponentInitial;
	exponentNorm = exponentValue / 128.0;

	cutoffValue = cutoffInitial;
	cutoffNorm = cutoffValue / cutoffMax;

	//for shininess
	shininessValue = shininessInitial;
	shininessNorm = shininessValue / 128.0;
}

void
animate()
{
	frame += 1;

	for (int i = 0; i < 3; i++)
	{
		if (lightOn[i]) thetaLight[i] += 4 / period; //degree
	}

	//Periodically change the exponent and/or cutoff value of the spot light
	if (lightOn[2] && exponent)
	{
		exponentNorm += radians(4.0 / period) / M_PI;
		exponentValue = 128.0 * (acos(cos(exponentNorm * M_PI)) / M_PI);
	}

	if (lightOn[2] && cutoff)
	{
		cutoffNorm += radians(4.0 / period) / M_PI;
		cutoffValue = cutoffMax * (acos(cos(cutoffNorm * M_PI)) / M_PI);
	}

	//for shininess
	if (shininess)
	{
		shininessNorm += radians(4.0 / period) / M_PI;
		shininessValue = 128.0 * (acos(cos(shininessNorm * M_PI)) / M_PI);
	}
}

void
torusFunc()
{
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			if (i == 35)
			{
				glNormal3f(p[i][j].x, p[i][j].y, p[i][j].z);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
				glNormal3f(p[0][j].x, p[0][j].y, p[0][j].z);
				glVertex3f(p[0][j].x, p[0][j].y, p[0][j].z);
				if (j == 17)
				{
					glNormal3f(p[0][0].x, p[0][0].y, p[0][0].z);
					glVertex3f(p[0][0].x, p[0][0].y, p[0][0].z);
					glNormal3f(p[i][0].x, p[i][0].y, p[i][0].z);
					glVertex3f(p[i][0].x, p[i][0].y, p[i][0].z);
				}
				else
				{
					glNormal3f(p[0][j + 1].x, p[0][j + 1].y, p[0][j + 1].z);
					glVertex3f(p[0][j + 1].x, p[0][j + 1].y, p[0][j + 1].z);
					glNormal3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
					glVertex3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
				}
			}
			else
			{
				glNormal3f(p[i][j].x, p[i][j].y, p[i][j].z);
				glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
				glNormal3f(p[i + 1][j].x, p[i + 1][j].y, p[i + 1][j].z);
				glVertex3f(p[i + 1][j].x, p[i + 1][j].y, p[i + 1][j].z);
				if (j == 17)
				{
					glNormal3f(p[i + 1][0].x, p[i + 1][0].y, p[i + 1][0].z);
					glVertex3f(p[i + 1][0].x, p[i + 1][0].y, p[i + 1][0].z);
					glNormal3f(p[i][0].x, p[i][0].y, p[i][0].z);
					glVertex3f(p[i][0].x, p[i][0].y, p[i][0].z);
				}
				else
				{
					glNormal3f(p[i + 1][j + 1].x, p[i + 1][j + 1].y, p[i + 1][j + 1].z);
					glVertex3f(p[i + 1][j + 1].x, p[i + 1][j + 1].y, p[i + 1][j + 1].z);
					glNormal3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
					glVertex3f(p[i][j + 1].x, p[i][j + 1].y, p[i][j + 1].z);
				}
			}
		}
	}
}

void
setTorusPoints()
{
	vec3 cenP(1.2, 1.2, 0.0);
	vec3 sP(0.5, 0, 0);
	vec3 yAxis(0, 1, 0);
	vec3 zAxis(0, 0, 1);

	float rTheta = 0.0f;
	float yrTheta = 0.0f;

	vec4 rV;
	mat4 tM = translate(mat4(1.0), cenP);

	for (int i = 0; i < 18; i++)
	{
		vec4 sP4(0.4, 0.0, 0.0, 1.0);

		mat4 R = rotate(mat4(1.0), radians(20.0f*i), zAxis);

		vec4 rotatedSP = R * sP4;

		rV = tM * rotatedSP;
		p[0][i] = rV;

	}

	for (int j = 0; j < 36; j++)
	{
		for (int i = 0; i < 18; i++)
		{
			mat4 R2 = rotate(mat4(1.0), radians(10.f*j), yAxis);
			vec4 rotateP(p[0][i].x, p[0][i].y, p[0][i].z, 1.0);
			vec4 rotatedCircle = R2 * rotateP;
			p[j][i] = rotatedCircle;
			glNormal3f(p[j][i].x, p[j][i].y, p[j][i].z);
			glVertex3f(p[j][i].x, p[j][i].y, p[j][i].z);

		}
	}
}

void
drawTorusLine()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPolygonOffset(1.0f, 1.0f);

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);

	torusFunc();

	glEnd();
}

void
drawTorus()
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_QUADS);
	glColor3f(0.3, 0.3, 0.3);

	torusFunc();

	glEnd();
}

void
drawTorusWithLine()
{
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawTorus();

	glPolygonOffset(0.0f, 0.0f);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTorusLine();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //for fill the light
}

//Sphere, cylinder
GLUquadricObj* sphere = NULL;
GLUquadricObj* cylinder = NULL;
GLUquadricObj* cone = NULL;

void
init()
{
	//Animation system
	reinitialize();

	//Prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);

	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricOrientation(cylinder, GLU_OUTSIDE);
	gluQuadricTexture(cylinder, GL_FALSE);

	cone = gluNewQuadric();
	gluQuadricDrawStyle(cone, GLU_FILL);
	gluQuadricNormals(cone, GLU_SMOOTH);
	gluQuadricOrientation(cone, GLU_OUTSIDE);
	gluQuadricTexture(cone, GL_FALSE);

	//Keyboard
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: n for draw vertex normal vector" << endl;
	cout << "Keyboard input: p for toggle Point light" << endl;
	cout << "Keyboard input: d for toggle Directional light" << endl;
	cout << "Keyboard input: s for toggle Spot light" << endl;
	cout << "Keyboard input: t for time dependent shininess coefficient" << endl;
}

void
quit()
{
	//Delete quadric shapes
	gluDeleteQuadric(sphere);
	gluDeleteQuadric(cylinder);
	gluDeleteQuadric(cone);

}

//Material
void
setupColoredMaterial(const vec3& color)
{
	if (!shininess)
	{
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		//Material
		GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
		GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
		GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
		GLfloat mat_shininess = 25;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}

	else if(shininess)
	{
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		//Material
		GLfloat mat_ambient[4] = { 0.1, 0.1, 0.1, 1 };
		GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
		GLfloat mat_specular[4] = { 0.5, 0.5, 0.5, 1 };
		GLfloat mat_shininess = shininessValue;


		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}
	
}

//Draw a sphereusing a GLU quadric
void
drawSphere(float radius, int slices, int stacks)
{
	//Sphere using GLU quadrics
	gluSphere(sphere, radius, slices, stacks);
}

//Draw a cylinder using a GLU quadric
void
drawCylinder(float radius, float height, int slices, int stacks)
{
	gluCylinder(cylinder, radius, radius, height, slices, stacks);
}

//Draw a cone using a GLU quadric
void
drawCone(float radius, float height, int slices, int stacks)
{
	gluCylinder(cone, 0, radius, height, slices, stacks);
}

//Compute the rotation axis and angle from a to b
//Axis is not normalized.
//theta is represented in degrees.

void
computeRotation(const vec3& a, const vec3& b, float& theta, vec3& axis)
{
	axis = cross(a, b);
	float sinTheta = length(axis);
	float cosTheta = dot(a, b);
	theta = atan2(sinTheta, cosTheta) * 180 / M_PI;
}

//Light
void
setupLight(const vec4& p, int i)
{

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 1 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1 };
	GLfloat specular[4] = { 1.0, 1.0, 1.0, 1 };

	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + i, GL_POSITION, value_ptr(p));

	//Attenuation for the point light
	if (i == 0)
	{
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.1);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.05);
	}

	else { //Default value
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.0);
	}

	if (i == 2) //Spot light
	{
		vec3 spotDirection = -vec3(p);
		glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, value_ptr(spotDirection));
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, cutoffValue); //[0,90]
		glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, exponentValue); //0, 128]

	}
	else { //Point and distant light.
		   //180 to turn off cutoff when it was used as a spot light.
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180); //uniform light distribution.
	}
}

void
drawArrow(const vec4& p, bool tailOnly)
{
	//Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//Common material
	GLfloat mat_spcular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 25;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spcular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	//Transformation
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	if (!tailOnly)
	{
		float theta;
		vec3 axis;
		computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
		glRotatef(theta, axis.x, axis.y, axis.z);
	}

	//Tail sphere
	float arrowTailRadius = 0.05;
	glColor3f(1, 0, 0); //ambient and diffuse
	drawSphere(arrowTailRadius, 16, 16);

	if (!tailOnly)
	{
		//Shaft cylinder
		float arrowShaftRadius = 0.02;
		float arrowShaftLength = 0.2;
		glColor3f(0, 1, 0);
		drawCylinder(arrowShaftRadius, arrowShaftLength, 16, 5);

		//Head cone
		float arrowheadHeight = 0.09;
		float arrowheadRadius = 0.06;
		glTranslatef(0, 0, arrowShaftLength + arrowheadHeight);
		glRotatef(180, 1, 0, 0);
		glColor3f(0, 0, 1); //ambient and diffuse
		drawCone(arrowheadRadius, arrowheadHeight, 16, 5);
	}

	glPopMatrix();

	//For convential material setting
	glDisable(GL_COLOR_MATERIAL);
}

void
drawSpotLight(const vec4& p, float cutoff)
{
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	float theta;
	vec3 axis;
	computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
	glRotatef(theta, axis.x, axis.y, axis.z);

	//Color
	setupColoredMaterial(vec3(0, 0, 1));

	//tan(cutoff) = r/h
	float h = 0.15;
	float r = h * tan(radians(cutoff));
	drawCone(r, h, 16, 5);

	//Color
	setupColoredMaterial(vec3(1, 0, 0));

	//Apex
	float apexRadius = 0.06 * (0.5 + exponentValue / 128.0);
	drawSphere(apexRadius, 16, 16);

	glPopMatrix();
}

void
render(GLFWwindow* window)
{
	//Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	setTorusPoints();
	drawTorusWithLine();

	//Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH*dpiScaling);

	//Smooth shading
	glShadeModel(GL_SMOOTH);

	//Rotation of the light or 2x2 models
	vec3 axis(0, 1, 0);

	//Lighting
	glEnable(GL_LIGHTING);

	//Set up the lights
	vec4 lightP[3];
	for (int i = 0; i < 3; i++)
	{
		//Just turn off the i-th light, if not lit
		if (!lightOn[i]) { glDisable(GL_LIGHT0 + i); continue; }

		//Turn on the i-th light
		glEnable(GL_LIGHT0 + i);

		//Dealing with the distant light
		lightP[i] = lightInitialP;
		if (i == 1) lightP[i].w = 0;

		//Lights rotate around the center of the world coordinate system
		mat4 R = rotate(mat4(1.0), radians(thetaLight[i]), axis);
		lightP[i] = R * lightP[i];

		//Set up the i-th lgith
		setupLight(lightP[i], i);
	}

	//Draw the geometries of the lights
	for (int i = 0; i < 3; i++)
	{
		if (!lightOn[i]) continue;


		if (i == 2) drawSpotLight(lightP[i], cutoffValue);
		else drawArrow(lightP[i], i == 0); //Tail only for a point light
	}

	setupColoredMaterial(vec3(1, 0, 0));
}


void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			//Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			//Play on/off
		case GLFW_KEY_SPACE: pause = !pause; break;

			//Turn on/off the point, distant, spot light
		case GLFW_KEY_P: lightOn[0] = !lightOn[0]; break;
		case GLFW_KEY_D: lightOn[1] = !lightOn[1]; break;
		case GLFW_KEY_S: lightOn[2] = !lightOn[2]; break;

			//for shininess
		case GLFW_KEY_T: shininess = !shininess; break;
		}
	}
}

int
main(int argc, char* argv[])
{
	//Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	//Callbacks
	glfwSetKeyCallback(window, keyboard);

	//Depth test
	glEnable(GL_DEPTH_TEST);

	//Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	//Viewport and perspective setting
	reshape(window, windowW, windowH);

	//Initializeation - Main loop - Finalization
	init();

	//Main loop
	float previous = glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); //Events

		//Time apssed during a single loop
		float now = glfwGetTime();
		float delta = now - previous;
		previous = now;

		//Time passed after the previous frame
		elapsed += delta;

		//Deal with the current frame
		if (elapsed > timeStep)
		{
			//Animate 1 frame
			if (!pause) animate();

			elapsed = 0; //Reset the elapsed time
		}

		render(window); //Draw one frame
		glfwSwapBuffers(window); //Swap buffers
	}

	//Finalization
	quit();

	//Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
