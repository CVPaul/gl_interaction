// define a class to deal with the common operation for mouse
#ifndef GL_BASIC_H
#define GL_BASIC_H
#ifdef __unix__
#include <GL/glut.h>
#elif defined(_WIN32)||defined(WIN32)
#include <GL\glut.h>
#endif
extern GLfloat PI;
extern float Trans[3];// = { 0, 0, 0 };
extern float Rotate[3];

//public:
extern int mousex;
extern int mousey;
extern GLboolean mouseldow;
extern GLboolean mouserdown;
extern GLboolean mousemdown;

extern GLfloat center[3];
extern GLfloat eye[3];

extern GLfloat xrotate;
extern GLfloat yrotate;
extern GLfloat celength;

extern GLfloat mSpeed;
extern GLfloat rSpeed;
//public:

void initMouseEvent();

void CalEyePostion();

void MouseMotion(int x, int y);
void KeyFunc(unsigned char key, int x, int y);
void MouseFunc(int button, int state, int x, int y);

void MoveBackward();
void MoveForward();

void RotateLeft();
void RotateRight();
void RotateUp();
void RotateDown();

void tLookAt();

void OnLButton(int x, int y);
void OnRButton(int x, int y);
void MouseEvent(int x, int y);

#endif/*gl_interaction.h*/
