#include "genTexture.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
#include <opencv2/highgui/highgui.hpp>
#include <GL/glut.h>
#elif defined(_WIN32)||defined(WIN32)
#include <opencv2\highgui\highgui.hpp>
#include <GL\glut.h>
#endif

typedef unsigned int UINT;

bool loadImage(std::string filename, uchar*& m_pBGRTexture, int& rows, int& cols)
{
	std::string cf_name = filename;

	cv::Mat m_Image = cv::imread(cf_name);

	if (m_Image.empty() || (m_Image.channels() != 3))
	{
		printf("cannot load the Image or the channels of it is not 3");
		return false;
	}
	rows = m_Image.rows;
	cols = m_Image.cols;
	// set texture size,for OpenGL 2.0 or later 
	// earlier than opengl 2.0 the texture size should be power of 2
	m_pBGRTexture =
		new uchar[rows*cols * 3];// caution it is 3!
	if (!m_pBGRTexture)
		return 0;

	memset(m_pBGRTexture, 0xFF, rows*cols * 3 * sizeof(uchar));

	uchar* p = NULL;
	uchar* obj = m_pBGRTexture;
	// since in opencv the data may not store continuously so there need to m_pBGRTexture;
	for (size_t row = 0; row<m_Image.rows; row++)
	{
		p = m_Image.ptr<uchar>(row);
		for (size_t col = 0; col<m_Image.cols; col++)
		{
			*(obj++) = *(p++);
			*(obj++) = *(p++);
			*(obj++) = *(p++);
			//obj++; // be careful !!!
		}
	}
	return true;
}
bool genOneTexture(std::string TextPath, uchar*& m_pBGRTexture, UINT& m_Texture)
{
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_Texture);

	//uchar* m_pBGRTexture;
	int rows, cols;
	if (loadImage(TextPath, m_pBGRTexture, rows, cols))
	{
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB8,
			rows,
			cols,
			0,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			m_pBGRTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//mapping
	}
	else
	{
		return 0;
	}
	return 1;
}
bool loadTexture(const cv::Mat& Orig, GLuint& m_Texture)
{
	glEnable(GL_TEXTURE_2D);
	//glDeleteTextures(1,&m_Texture);

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB8,
		Orig.cols,
		Orig.rows,
		0,
		GL_BGR_EXT,
		GL_UNSIGNED_BYTE,
		Orig.data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//mapping
	return 1;
}
bool loadTexture(std::string tex_path, GLuint& m_Texture, bool flip)
{
	cv::Mat Tex = cv::imread(tex_path);
	if (flip)cv::flip(Tex, Tex, 0);

	if (Tex.empty())
	{
		printf("Error! can not load the Image(in loadTexture())!\n");
		system("PAUSE");
		exit(-1);
	}
	loadTexture(Tex, m_Texture);
}
