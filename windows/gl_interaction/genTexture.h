#ifndef GENTEXTURE_H
#define GENTEXTURE_H
#include <string>
#include <opencv2\core\core.hpp>
#include <GL\glut.h>
/**@ brief: load image and convert to continual memory Byte m_pBGRTexture
* param[in] filename: path of the texture
* param[out] m_pBGRTexture: continual memory Byte (never forget to release it)
* param[out] rows: height of the texture
* param[out] cols: width of the texture
* return[bool] success or failed
*/
bool loadImage(std::string filename, uchar*& m_pBGRTexture, int& rows, int& cols);

/**@ brief: call loadImage and generate the Texture(return the assignment ID:m_Texture)
* param[in] Textpath: path of the texture
* param[out] m_pBGRTexture: continual memory Byte (never forget to release it)
* param[out] m_Texture: the assignment ID for opengl render
* return[bool] success or failed
*/
bool genOneTexture(std::string TextPath, uchar*& m_pBGRTexture, GLuint& m_Texture);

/**@brief: convert the cv::Mat to opengl texture and return texture ID in m_Texture
* param[in] Orig: the original opencv Mat data
* param[out] m_Texture: the assignment ID for opengl render
* return[bool] success or failed
*/
bool loadTexture(const cv::Mat& Orig, GLuint& m_Texture);

/**@ brief: call loadTexture_const_cv::Mat_GLuint to get the texture from file
* param[in] tex_path: path of the texture
* param[out] m_Texture: the assignment ID for opengl render
* param[in] flip: flip the Mat or not (since the axis of opencv and opengl are different, defalut is false)
* return[bool] success or failed
*/
bool loadTexture(std::string tex_path, GLuint& m_Texture, bool flip = false);
#endif/*genTexture.h*/