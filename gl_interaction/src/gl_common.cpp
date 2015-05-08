#include "gl_basic.h"
#include "gl_common.h"
#include "glm.h"

#include <stdio.h>
#include <stdlib.h>
#include "genTexture.h"
#include <string>
namespace GL
{
	GLUquadric *quad=NULL;
	GLMmodel* model=NULL;
	GLMmodel* model_back=NULL;
	GLuint m_Texture=0;
}
void GL::spin()
{
	// change some parameter here to realize the animation function
	// for example:
	Trans[0] = (int(Trans[0]) + 1) % 100; // Trans[0] is defined in gl_basic to control the movation
	Trans[1] = (int(Trans[0]) + 1) % 70; // Trans[1] is defined in gl_basic to control the movation
	Trans[2] = (int(Trans[0]) + 1) % 50; // Trans[2] is defined in gl_basic to control the movation

	Rotate[0] = (int(Rotate[0]) + 1) % 30;// Rotate[0] is defiend in gl_basic to control the rotation
	Rotate[1] = (int(Rotate[0]) + 1) % 40;// Rotate[1] is defiend in gl_basic to control the rotation
	Rotate[2] = (int(Rotate[0]) + 1) % 50;// Rotate[2] is defiend in gl_basic to control the rotation

	//Sleep(30); // control the FPS
	for(int k=0;k<50000;k++); // control the FPS

	display();
}

void GL::show(std::string Texture, void(*disp)())
{
	if (!model) return; // check if the model is ready

	// Part 1: init part
	int argc=1;
	char ** argv=NULL;
	glutInit(&argc, argv); //this line connect with the console like the main???
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GL_Show");
	// init opengl drawing 
	init_opengl();
	// init RealSense Camera!
	loadTexture(Texture, m_Texture, 1); //laod face texture 

	// Part 2: interactive part 
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MouseEvent);
	glutKeyboardFunc(KeyFunc);

	//// Part 3: display function:
	//glutDisplayFunc(display);
	//// Part 4: functions for the main loop
	////glutIdleFunc(spin);
	//glutReshapeFunc(reshape);

	// this is also OK-----------------------------------------------
	// Part 3: display function (list):
	glutDisplayFunc(disp);
	// Part 4: functions for the main loop
	//glutIdleFunc(GL::spin);
	glutReshapeFunc(GL::reshape);
	//----------------------------------------------------------------

	glutMainLoop();
}

void GL::display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, (GLsizei)640, (GLsizei)480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	float ratio = std::sin(75*CV_PI / 360) / std::sin(55*CV_PI / 360);
	float focus = 0.1; // do not try to set this to 0
	gluPerspective(55, ratio, focus, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,
		0, 0, 1.0,
		0, 1.0, 0);
	// model Transformation  part:
	//glTranslatef(0, 0, 50);
	glTranslatef(Trans[0], Trans[1], Trans[2]);
	glTranslatef(0, 0, 100);
	glRotatef(Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(Rotate[1], 0.0, 1.0, 0.0);
	glRotatef(Rotate[2], 0.0, 0.0, 1.0);
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glRotatef(90, 1.0, 0.0, 0.0);// for fake eye texture
	gluSphere(quad, 20, 50, 100);
	//glmDraw(model_back, GLM_TEXTURE);
	//glmDraw(model, GLM_TEXTURE);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glutSwapBuffers();
}

void GL::display_face()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, (GLsizei)640, (GLsizei)480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio = std::sin(75 * CV_PI / 360) / std::sin(55 * CV_PI / 360);
	float focus = 0.1; // do not try to set this to 0
	gluPerspective(55, ratio, focus, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,
		0, 0, 1.0,
		0, 1.0, 0);
	// model Transformation  part:
	//glTranslatef(0, 0, 50);
	glTranslatef(Trans[0], Trans[1], Trans[2]);
	glTranslatef(0, 0, 100);
	glRotatef(Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(Rotate[1], 0.0, 1.0, 0.0);
	glRotatef(Rotate[2], 0.0, 0.0, 1.0);

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glRotatef(90, 1.0, 0.0, 0.0);// for fake eye texture
	//gluSphere(quad, 20, 50, 100);
	//glmDraw(model_back, GLM_TEXTURE);
	glmDraw(model, GLM_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glutSwapBuffers();
}

void GL::display_kriging_demo()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, (GLsizei)640, (GLsizei)480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio = std::sin(75 * CV_PI / 360) / std::sin(55 * CV_PI / 360);
	float focus = 0.1; // do not try to set this to 0
	gluPerspective(55, ratio, focus, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,
		0, 0, 1.0,
		0, 1.0, 0);
	// model Transformation  part:
	//glTranslatef(0, 0, 50);
	glTranslatef(Trans[0], Trans[1], Trans[2]);
	glTranslatef(0, 0, 100);
	glRotatef(Rotate[0], 1.0, 0.0, 0.0);
	glRotatef(Rotate[1], 0.0, 1.0, 0.0);
	glRotatef(Rotate[2], 0.0, 0.0, 1.0);

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glRotatef(90, 1.0, 0.0, 0.0);// for fake eye texture
	//gluSphere(quad, 20, 50, 100);
	glmDraw(model_back, GLM_TEXTURE);
	glmDraw(model, GLM_TEXTURE);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glutSwapBuffers();
}
void GL::init_opengl(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0); //black background

	quad = gluNewQuadric();
	gluQuadricNormals(quad, GL_SMOOTH);						// smooth normal
	gluQuadricTexture(quad, GL_TRUE);						// turn on texture
}

void GL::reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.10, 1200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0, 0, 0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
	gluLookAt(0, 0, 0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
}


cv::Mat GL::glgrab(int x, int y, int width, int height, cv::Mat& depth)
{
	if (width <= 0 || height <= 0)
	{
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		x = viewport[0];
		y = viewport[1];
		width = viewport[2];
		height = viewport[3];
	}
	if (x < 0 || y < 0 || width <= 0 || height <= 0)
	{
		printf("This param comnination (x=%d,y=%d,width=%d,height=%d)!\n",
			x, y, width, height);
		return cv::Mat();
	}
	// calculate the length of the data
	width -= width % 4; //caution!!!
	// assign the memory
	uchar* pPixel = (uchar*)malloc(3 * height*width*sizeof(uchar));
	float* pDepth = (float*)malloc(height*width*sizeof(float));

	if (!pPixel)//||!pDepth)
	{
		printf("not Enough memeory avilable!(in gl_getImage())!\n");
		return cv::Mat();
	}
	// read the pixels
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(x, y, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixel);
	glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, pDepth);

	cv::Mat image(height, width, CV_8UC3, pPixel);
	cv::Mat dep(height, width, CV_32FC1, pDepth);

	cv::Mat out;
	cv::flip(dep,depth,0);
	cv::flip(image, out, 0);

	if (pDepth)delete[] pDepth;
	if (pPixel)delete[] pPixel;

	return out;
}
