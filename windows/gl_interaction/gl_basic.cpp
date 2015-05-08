#include "gl_basic.h"
#include <cmath>

/*
this file define the usually used mouse callback function and the key board callback function
main framework get from http://blog.sina.com.cn/s/blog_78c952670100q3p0.html
rewrite on 2015-04-14
*/

/// record the state of mouse
GLfloat PI = 3.1415926f;
float Trans[3] = { 0, 0, 0 };
float Rotate[3] = { 0, 0, 0 };
//public:
int mousex;
int mousey;
GLboolean mouseldown = GL_FALSE;
GLboolean mouserdown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;

GLfloat center[3] = { 0.0, 0.0, 0.0 };
GLfloat eye[3] = { 0.0, 0.0, -1.0 };

GLfloat xrotate = PI / 4;
GLfloat yrotate = PI / 4;
GLfloat celength = PI / 4;

GLfloat mSpeed = 10.0f;
GLfloat rSpeed = 0.02f;
/// calculate the eye position according to center position and angle,length
void CalEyePostion()
{
	/*if (yrotate > PI / 2.2) yrotate = PI / 2.2;   /// limit the direction to see
	if (yrotate < 0.01)  yrotate = 0.01;
	if (xrotate > 2 * PI)   xrotate = 0.01;
	if (xrotate < 0)   xrotate = 2 * PI;*/
	//if (celength > 50)  celength = 50;     ///  limit the zoom distance
	//if (celength < 5)   celength = 5;

	/// computer the eye's position by sphere axis relation ship
	eye[0] = center[0] + celength * sin(yrotate) * cos(xrotate);
	eye[2] = center[2] + celength * sin(yrotate) * sin(xrotate);
	eye[1] = center[1] + celength * cos(yrotate);
}

/// center moves
void MoveBackward()
{
	center[0] += mSpeed * cos(xrotate);
	center[2] += mSpeed * sin(xrotate);
	CalEyePostion();
}

void MoveForward()
{
	center[0] -= mSpeed * cos(xrotate);
	center[2] -= mSpeed * sin(xrotate);
	CalEyePostion();
}

/// visual angle rotates
void RotateLeft()
{
	xrotate -= rSpeed;
	CalEyePostion();
}

void RotateRight()
{
	xrotate += rSpeed;
	CalEyePostion();
}

void RotateUp()
{
	yrotate += rSpeed;
	CalEyePostion();
}

void RotateDown()
{
	yrotate -= rSpeed;
	CalEyePostion();
}

/// CALLBACK func for keyboard presses
void KeyFunc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a': Trans[0] += mSpeed; break;
	case 'd': Trans[0] -= mSpeed; break;
	case 'w': Trans[1] += mSpeed; break;
	case 's': Trans[1] -= mSpeed; break;
	case 'q': Trans[2] += mSpeed; break;
	case 'e': Trans[1] -= mSpeed; break;
	}
	glutPostRedisplay();
}

/// CALLBACK func for mouse kicks
void MouseFunc(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_RIGHT_BUTTON) mouserdown = GL_TRUE;
		if (button == GLUT_LEFT_BUTTON) mouseldown = GL_TRUE;
		if (button == GLUT_MIDDLE_BUTTON)mousemdown = GL_TRUE;
	}
	else
	{
		if (button == GLUT_RIGHT_BUTTON) mouserdown = GL_FALSE;
		if (button == GLUT_LEFT_BUTTON) mouseldown = GL_FALSE;
		if (button == GLUT_MIDDLE_BUTTON)mousemdown = GL_FALSE;
	}
	mousex = x, mousey = y;
}

/// CALLBACK func for mouse motions
void MouseMotion(int x, int y)
{
	if (mouserdown == GL_TRUE)
	{       /// 80.0f or 120.0 control the speed of the Motion, tune it as you like
		xrotate += (x - mousex) / 80.0f;
		yrotate -= (y - mousey) / 120.0f;
	}

	if (mouseldown == GL_TRUE)
	{
		celength += (y - mousey) / 25.0f;
	}
	mousex = x, mousey = y;
	CalEyePostion();
	glutPostRedisplay();
}

void tLookAt()            /// package gluLookAt() for convieint use
{
	CalEyePostion();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);
}
// add ------------------------------------->>>>>>>>
void OnLButton(int x, int y)
{
	Rotate[0] += (y - mousey);
	Rotate[1] += (x - mousex);
}
void OnRButton(int x, int y)
{
	float r = sqrt((x - mousex)*(x - mousex) + (y - mousey)*(y - mousey));
	float direct = y - mousey;
	
	if (direct > 0)Trans[2] += r;
	else Trans[2] -= r;
}
void MouseEvent(int x, int y)
{

	if (mouseldown == GL_TRUE) OnLButton(x, y);

	if (mouserdown == GL_TRUE) OnRButton(x, y);
	
	mousex = x, mousey = y;
	glutPostRedisplay();
}