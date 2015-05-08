#ifndef GL_COMMON_H
#define GL_COMMON_H


#include "gl_basic.h"
#include "glm.h"

#include <string>
#ifdef __unix__
#include <opencv2/core/core.hpp>
#elif defined(_WIN32)||(WIN32)
#include <opencv2\core\core.hpp>
#endif

namespace GL
{
	extern GLUquadric *quad;
	extern GLMmodel* model;
	extern GLMmodel* model_back;
	extern GLuint m_Texture;
	

	/**@ brief put this function in glutIdleFunc() for dynamic implementaion
	* return[void]
	*/
	void spin();

	/**@ breif this function packaged the mian frame of opengl,call it in the main function
	* param[in] Texture: the Texture save path(just use as an example)
	* param[in] disp: the display function
	* return[void]
	*/
	void show(std::string Texture, void (*disp)());

	/**@ this function call by glutDisplayFunc() to draw on buffer and swap the buffers
	* chage the drawing code as you like
	* return[void]
	*/
	void display();
	
	/**@ this function call by glutDisplayFunc() to draw face model on the buffer and swap buffers
	* chage the drawing code as you like
	* return[void]
	*/
	void display_face();

	/**@ this function call by glutDisplayFunc() to show the Kriging algorithm
	* chage the drawing code as you like
	* return[void]
	*/
	void display_kriging_demo();

	/**@ init the opengl in this function
	* return[void]
	*/
	void init_opengl(void);
	/**@ when the windows reshaped this functon will be called
	* return[void]
	*/
	void reshape(int w, int h);

	/**@ this funtion grab the gl render sense on the screen an tranform it into opencv Mat
	* param[in] (x,y): the start point to grab on the screen, usually it is set to (0,0)
	* param[in] (width,height): the size of the grab region
	* param[out] depth: depth of the corresponding point gl formt (-1.0,1.0)
	* return[cv::Mat] the grab result in BGR format
	*/
	cv::Mat glgrab(int x, int y, int width, int height, cv::Mat& depth);
}

#endif /*gl_common.h*/
