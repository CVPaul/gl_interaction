#ifdef __unix__
#include <opencv2/core/core.hpp>
#elif defined(_WIN32)||defined(WIN32)
#include <opencv2\core\core.hpp>
#endif
#include "TransformPoints.h"
#include "PointCloud.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>

Transform::Transform()
{
	memset(trans_mat, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	trans_mat[0] = trans_mat[5] = trans_mat[10] = trans_mat[15] = 1.0;
}
Transform::Transform(float mat[TRANS_MATRIX_SIZE])
{
	for (int i = 0; i < TRANS_MATRIX_SIZE; i++)
		trans_mat[i] = mat[i];
}
Transform::~Transform(){};

//extract the rotate mat to the _trans_mat[] and the rotate the point only (no movement))
void Transform::rotatePoint(int size, Point3D* orig_points, Point3D* new_points)
{
	//extract the rotate mat to the _trans_mat[] and the rotate the point only (no movement))
	float _trans_mat[9];
	_trans_mat[0] = trans_mat[0]; _trans_mat[1] = trans_mat[1]; _trans_mat[2] = trans_mat[2];
	_trans_mat[3] = trans_mat[4]; _trans_mat[4] = trans_mat[5]; _trans_mat[5] = trans_mat[6];
	_trans_mat[6] = trans_mat[8]; _trans_mat[7] = trans_mat[9]; _trans_mat[8] = trans_mat[10];
	float tx, ty, tz;
	for (int k = 0; k < size; k++)
	{
		tx = _trans_mat[0] * orig_points[k].x +
			_trans_mat[1] * orig_points[k].y +
			_trans_mat[2] * orig_points[k].z;

		ty = _trans_mat[3] * orig_points[k].x +
			_trans_mat[4] * orig_points[k].y +
			_trans_mat[5] * orig_points[k].z;

		tz = _trans_mat[6] * orig_points[k].x +
			_trans_mat[7] * orig_points[k].y +
			_trans_mat[8] * orig_points[k].z;
		new_points[k].x = tx;
		new_points[k].y = ty;
		new_points[k].z = tz;
	}
}
// mulit the trans_mat with a roatetion matrix,which is defined by the params:angle and the axis(x,y,z)
void Transform::roteByAxis(float angle, float axis_x, float axis_y, float axis_z)
{
	float len = sqrt(axis_x*axis_x + axis_y*axis_y + axis_z*axis_z);
	if (0 == len)return;

	axis_x = axis_x / len;
	axis_y = axis_y / len;
	axis_z = axis_z / len;

	angle = angle / 180.0 * CV_PI; // translate from degree to rad

	float ctheta = cos(angle);
	float stheta = sin(angle);
	float atheta = 1 - ctheta;

	float R_Matrix[TRANS_MATRIX_SIZE];

	R_Matrix[0] = ctheta + atheta*axis_x*axis_x;
	R_Matrix[1] = atheta*axis_x*axis_y - stheta*axis_z;
	R_Matrix[2] = atheta*axis_x*axis_z + stheta*axis_y;
	R_Matrix[3] = 0.0;

	R_Matrix[4] = atheta*axis_y*axis_x + stheta*axis_z;
	R_Matrix[5] = ctheta + atheta*axis_y*axis_y;
	R_Matrix[6] = atheta*axis_y*axis_z - stheta*axis_x;
	R_Matrix[7] = 0.0;

	R_Matrix[8] = atheta*axis_z*axis_x - stheta*axis_y;
	R_Matrix[9] = atheta*axis_z*axis_y + stheta*axis_x;
	R_Matrix[10] = ctheta + atheta*axis_z*axis_z;
	R_Matrix[11] = 0.0;

	R_Matrix[12] = R_Matrix[13] = R_Matrix[14] = 0.0;
	R_Matrix[15] = 1.0;

	multi(this->trans_mat, R_Matrix);
}
// an sample matrix multy funtion, return the result in the mat1;
void Transform::multi(float mat1[TRANS_MATRIX_SIZE], float mat2[TRANS_MATRIX_SIZE])
{
	float temp[TRANS_MATRIX_SIZE];
	memset(temp, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	for (int i = 0; i < TRANS_MATRIX_DIM; i++)
	{
		for (int j = 0; j < TRANS_MATRIX_DIM; j++)
		{
			for (int k = 0; k < TRANS_MATRIX_DIM; k++)
			{
				temp[i * TRANS_MATRIX_DIM + j] += mat1[i * TRANS_MATRIX_DIM + k] * mat2[k * TRANS_MATRIX_DIM + j];
			}
		}
	}
	for (int i = 0; i < TRANS_MATRIX_DIM; i++)
	{
		for (int j = 0; j < TRANS_MATRIX_DIM; j++)
		{
			mat1[i * TRANS_MATRIX_DIM + j] = temp[i * TRANS_MATRIX_DIM + j];
		}
	}
}
// print the matrix
void Transform::printMat()
{
	int pos = 0;
	for (int i = 0; i < TRANS_MATRIX_DIM; i++)
	{
		pos = i*TRANS_MATRIX_DIM;
		printf("%f,%f,%f,%f\n", trans_mat[pos], trans_mat[pos + 1], trans_mat[pos + 2], trans_mat[pos + 3]);
	}
}
// mulit the trans_mat with a roatetion matrix,which is defined by the 3*3 rotation matrix
void Transform::roteByMatrix(float rot_mat[9])
{
	float R_matrix[TRANS_MATRIX_SIZE];
	memset(R_matrix, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	R_matrix[15] = 1.0;

	R_matrix[0] = rot_mat[0]; R_matrix[1] = rot_mat[1]; R_matrix[2] = rot_mat[2];
	R_matrix[4] = rot_mat[3]; R_matrix[5] = rot_mat[4]; R_matrix[6] = rot_mat[5];
	R_matrix[8] = rot_mat[6]; R_matrix[9] = rot_mat[7]; R_matrix[10] = rot_mat[8];

	multi(this->trans_mat, R_matrix);
}
// in case some pramas are double ,we can convert it
void Transform::double2float(int size, double* db_a, float* ft_a)
{
	for (int k = 0; k < size; k++)
		ft_a[k] = db_a[k];
}
// move original point by trans_vec and then put in the new_points,orignal_p and new_p can be the same
void Transform::TranslatePoint(float Trans_vec[3])
{
	float T_matrix[TRANS_MATRIX_SIZE];
	memset(T_matrix, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	T_matrix[0] = T_matrix[5] = T_matrix[10] = T_matrix[15] = 1.0;
	T_matrix[3] = Trans_vec[0];
	T_matrix[7] = Trans_vec[1];
	T_matrix[11] = Trans_vec[2];

	multi(this->trans_mat, T_matrix);
}
// mulit this->trans_mat with a Translate Matrix(2)
void Transform::TranslatePoint(float Tx, float Ty, float Tz)
{
	float T_matrix[TRANS_MATRIX_SIZE];
	memset(T_matrix, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	T_matrix[0] = T_matrix[5] = T_matrix[10] = T_matrix[15] = 1.0;
	T_matrix[3] = Tx;
	T_matrix[7] = Ty;
	T_matrix[11] = Tz;

	multi(this->trans_mat, T_matrix);
}
// Transform all the point in once!!
void Transform::TranformInOnce(int size, float Trans_vec[3], float Rotate_vec[3], float Move_vec[3],
	Point3D* orig_points, Point3D* new_points)
{
	if (size <= 0) return;
	float tx, ty, tz, ts;
	// set transform matrix
	TranslatePoint(Trans_vec);
	roteByAxis(Rotate_vec[0], 1, 0, 0);
	roteByAxis(Rotate_vec[1], 0, 1, 0);
	roteByAxis(Rotate_vec[2], 0, 0, 1);
	TranslatePoint(Move_vec);

	for (int k = 0; k < size; k++)
	{
		tx = trans_mat[0] * (orig_points[k].x) +
			trans_mat[1] * (orig_points[k].y) +
			trans_mat[2] * (orig_points[k].z) +
			trans_mat[3];

		ty = trans_mat[4] * (orig_points[k].x) +
			trans_mat[5] * (orig_points[k].y) +
			trans_mat[6] * (orig_points[k].z) +
			trans_mat[7];


		tz = trans_mat[8] * (orig_points[k].x) +
			trans_mat[9] * (orig_points[k].y) +
			trans_mat[10] * (orig_points[k].z) +
			trans_mat[11];

		ts = trans_mat[12] * (orig_points[k].x) +
			trans_mat[13] * (orig_points[k].y) +
			trans_mat[14] * (orig_points[k].z) +
			trans_mat[15];

		if (abs(ts - 1.0)>1e-6)
		{
			new_points[k].x = tx / ts;
			new_points[k].y = ty / ts;
			new_points[k].z = tz / ts;
		}
		else
		{
			new_points[k].x = tx;
			new_points[k].y = ty;
			new_points[k].z = tz;
		}
	}
}
void Transform::TranformInOnce(int size, float Trans_vec[3], float RAngle, Point3D rotate_axis,
	Point3D* orig_points, Point3D* new_points)
{
	if (size <= 0) return;
	float tx, ty, tz, ts;
	// set transform matrix
	TranslatePoint(Trans_vec);
	roteByAxis(RAngle, rotate_axis.x, rotate_axis.y, rotate_axis.z);

	for (int k = 0; k < size; k++)
	{
		tx = trans_mat[0] * (orig_points[k].x) +
			trans_mat[1] * (orig_points[k].y) +
			trans_mat[2] * (orig_points[k].z) +
			trans_mat[3];

		ty = trans_mat[4] * (orig_points[k].x) +
			trans_mat[5] * (orig_points[k].y) +
			trans_mat[6] * (orig_points[k].z) +
			trans_mat[7];


		tz = trans_mat[8] * (orig_points[k].x) +
			trans_mat[9] * (orig_points[k].y) +
			trans_mat[10] * (orig_points[k].z) +
			trans_mat[11];

		ts = trans_mat[12] * (orig_points[k].x) +
			trans_mat[13] * (orig_points[k].y) +
			trans_mat[14] * (orig_points[k].z) +
			trans_mat[15];

		if (abs(ts - 1.0)>1e-6)
		{
			new_points[k].x = tx / ts;
			new_points[k].y = ty / ts;
			new_points[k].z = tz / ts;
		}
		else
		{
			new_points[k].x = tx;
			new_points[k].y = ty;
			new_points[k].z = tz;
		}
	}
}
// Transform all the point in once(3),no Transform param needed,use the already set params!!
void Transform::TranformInOnce(int size, Point3D* orig_points, Point3D* new_points)
{
	if (size <= 0) return;
	float tx, ty, tz, ts;

	for (int k = 0; k < size; k++)
	{
		tx = trans_mat[0] * (orig_points[k].x) +
			trans_mat[1] * (orig_points[k].y) +
			trans_mat[2] * (orig_points[k].z) +
			trans_mat[3];

		ty = trans_mat[4] * (orig_points[k].x) +
			trans_mat[5] * (orig_points[k].y) +
			trans_mat[6] * (orig_points[k].z) +
			trans_mat[7];


		tz = trans_mat[8] * (orig_points[k].x) +
			trans_mat[9] * (orig_points[k].y) +
			trans_mat[10] * (orig_points[k].z) +
			trans_mat[11];

		ts = trans_mat[12] * (orig_points[k].x) +
			trans_mat[13] * (orig_points[k].y) +
			trans_mat[14] * (orig_points[k].z) +
			trans_mat[15];

		if (abs(ts - 1.0)>1e-6)
		{
			new_points[k].x = tx / ts;
			new_points[k].y = ty / ts;
			new_points[k].z = tz / ts;
		}
		else
		{
			new_points[k].x = tx;
			new_points[k].y = ty;
			new_points[k].z = tz;
		}
	}
}
// set the trans_mat as Identity matrix.
void Transform::setIdentity()
{
	memset(trans_mat, 0, sizeof(float)* TRANS_MATRIX_SIZE);
	trans_mat[0] = trans_mat[5] = trans_mat[10] = trans_mat[15] = 1.0;
}
// set the rotation mat as an appointed mat
void Transform::setRotationMat(float rt_mat[TRANS_MATRIX_SIZE])
{
	for (int i = 0; i < TRANS_MATRIX_SIZE; i++)
		trans_mat[i] = rt_mat[i];
}
void Transform::glm_centroid(GLMmodel* model, float centroid[3])
{
	int pos = 0;
	centroid[0] = centroid[1] = centroid[2] = 0.0;
	for (int i = 1; i <= model->numvertices; i++)
	{
		pos = 3 * i;
		centroid[0] += model->vertices[pos + 0];
		centroid[1] += model->vertices[pos + 1];
		centroid[2] += model->vertices[pos + 2];
	}
	centroid[0] /= model->numvertices;
	centroid[1] /= model->numvertices;
	centroid[2] /= model->numvertices;
}
void Transform::glm_move(GLMmodel* &model, float mv_vec[3], float symbol)
{
	int pos = 0;
	float temp[3] = { mv_vec[0] * symbol, mv_vec[1] * symbol, mv_vec[2] * symbol };

	for (int i = 1; i <= model->numvertices; i++)
	{
		pos = 3 * i;
		model->vertices[pos] += temp[0];
		model->vertices[pos + 1] += temp[1];
		model->vertices[pos + 2] += temp[2];
	}
}
/*
@ brief: Transfrom in Once the function is similar as before but the objection change to GLMmodel
*/
void Transform::TranformInOnce(GLMmodel* &model, float Trans_vec[3], float Rotate_vec[3], float Move_vec[3])
{
	int size = model->numvertices;
	if (size <= 0) return;
	float tx, ty, tz, ts;

	TranslatePoint(Trans_vec);
	roteByAxis(Rotate_vec[0], 1, 0, 0);
	roteByAxis(Rotate_vec[1], 0, 1, 0);
	roteByAxis(Rotate_vec[2], 0, 0, 1);
	TranslatePoint(Move_vec);

	int pos = 0;
	for (int k = 0; k < size; k++)
	{
		pos = 3 * (k + 1);
		tx = trans_mat[0] * (model->vertices[pos]) +
			trans_mat[1] * (model->vertices[pos + 1]) +
			trans_mat[2] * (model->vertices[pos + 2]) +
			trans_mat[3];

		ty = trans_mat[4] * (model->vertices[pos]) +
			trans_mat[5] * (model->vertices[pos + 1]) +
			trans_mat[6] * (model->vertices[pos + 2]) +
			trans_mat[7];


		tz = trans_mat[8] * (model->vertices[pos]) +
			trans_mat[9] * (model->vertices[pos + 1]) +
			trans_mat[10] * (model->vertices[pos + 2]) +
			trans_mat[11];

		ts = trans_mat[12] * (model->vertices[pos]) +
			trans_mat[13] * (model->vertices[pos + 1]) +
			trans_mat[14] * (model->vertices[pos + 2]) +
			trans_mat[15];

		if (abs(ts - 1.0) > 1e-6)
		{
			model->vertices[pos] = tx / ts;
			model->vertices[pos + 1] = ty / ts;
			model->vertices[pos + 2] = tz / ts;
		}
		else
		{
			model->vertices[pos] = tx;
			model->vertices[pos + 1] = ty;
			model->vertices[pos + 2] = tz;
		}
	}
}
/*
@ brief: Transfrom in Once the function is similar as before but the objection change to GLMmodel(2)
*/
void Transform::TranformInOnce(GLMmodel* &model, float Trans_vec[3], float Move_vec[3], float RAngle, Point3D rotate_axis)
{
	int size = model->numvertices;
	if (size <= 0) return;
	float tx, ty, tz, ts;

	// set transform matrix
	TranslatePoint(Trans_vec);
	roteByAxis(RAngle, rotate_axis.x, rotate_axis.y, rotate_axis.z);
	TranslatePoint(Move_vec);

	int pos = 0;
	for (int k = 0; k < size; k++)
	{
		pos = 3 * (k + 1);
		tx = trans_mat[0] * (model->vertices[pos]) +
			trans_mat[1] * (model->vertices[pos + 1]) +
			trans_mat[2] * (model->vertices[pos + 2]) +
			trans_mat[3];

		ty = trans_mat[4] * (model->vertices[pos]) +
			trans_mat[5] * (model->vertices[pos + 1]) +
			trans_mat[6] * (model->vertices[pos + 2]) +
			trans_mat[7];


		tz = trans_mat[8] * (model->vertices[pos]) +
			trans_mat[9] * (model->vertices[pos + 1]) +
			trans_mat[10] * (model->vertices[pos + 2]) +
			trans_mat[11];

		ts = trans_mat[12] * (model->vertices[pos]) +
			trans_mat[13] * (model->vertices[pos + 1]) +
			trans_mat[14] * (model->vertices[pos + 2]) +
			trans_mat[15];

		if (abs(ts - 1.0) > 1e-6)
		{
			model->vertices[pos] = tx / ts;
			model->vertices[pos + 1] = ty / ts;
			model->vertices[pos + 2] = tz / ts;
		}
		else
		{
			model->vertices[pos] = tx;
			model->vertices[pos + 1] = ty;
			model->vertices[pos + 2] = tz;
		}
	}
}
/*
@ Transform all the point in once(3),no Transform param needed,use the already set params!!
*/
void Transform::TranformInOnce(GLMmodel*& model)
{
	int size = model->numvertices;
	if (size <= 0) return;
	float tx, ty, tz, ts;

	int pos = 0;
	for (int k = 0; k < size; k++)
	{
		pos = 3 * (k + 1);
		tx = trans_mat[0] * (model->vertices[pos]) +
			trans_mat[1] * (model->vertices[pos + 1]) +
			trans_mat[2] * (model->vertices[pos + 2]) +
			trans_mat[3];

		ty = trans_mat[4] * (model->vertices[pos]) +
			trans_mat[5] * (model->vertices[pos + 1]) +
			trans_mat[6] * (model->vertices[pos + 2]) +
			trans_mat[7];


		tz = trans_mat[8] * (model->vertices[pos]) +
			trans_mat[9] * (model->vertices[pos + 1]) +
			trans_mat[10] * (model->vertices[pos + 2]) +
			trans_mat[11];

		ts = trans_mat[12] * (model->vertices[pos]) +
			trans_mat[13] * (model->vertices[pos + 1]) +
			trans_mat[14] * (model->vertices[pos + 2]) +
			trans_mat[15];

		if (abs(ts - 1.0) > 1e-6)
		{
			model->vertices[pos] = tx / ts;
			model->vertices[pos + 1] = ty / ts;
			model->vertices[pos + 2] = tz / ts;
		}
		else
		{
			model->vertices[pos] = tx;
			model->vertices[pos + 1] = ty;
			model->vertices[pos + 2] = tz;
		}
	}
}
/*
@ Transform all the point in once(4),no Transform param needed,use the already set params!!
*/
void Transform::TranformInOnce(const GLMmodel* model, GLMmodel*& new_model)
{
	int size = model->numvertices;
	if (size <= 0) return;
	if (model->numvertices != new_model->numvertices)
	{
		printf("vertices size of the two model should be the same!(in TransformInOnce(4)) \n ");
		return;
	}
	float tx, ty, tz, ts;

	int pos = 0;
	for (int k = 0; k < size; k++)
	{
		pos = 3 * (k + 1);
		tx = trans_mat[0] * (model->vertices[pos]) +
			trans_mat[1] * (model->vertices[pos + 1]) +
			trans_mat[2] * (model->vertices[pos + 2]) +
			trans_mat[3];

		ty = trans_mat[4] * (model->vertices[pos]) +
			trans_mat[5] * (model->vertices[pos + 1]) +
			trans_mat[6] * (model->vertices[pos + 2]) +
			trans_mat[7];


		tz = trans_mat[8] * (model->vertices[pos]) +
			trans_mat[9] * (model->vertices[pos + 1]) +
			trans_mat[10] * (model->vertices[pos + 2]) +
			trans_mat[11];

		ts = trans_mat[12] * (model->vertices[pos]) +
			trans_mat[13] * (model->vertices[pos + 1]) +
			trans_mat[14] * (model->vertices[pos + 2]) +
			trans_mat[15];

		if (abs(ts - 1.0) > 1e-6)
		{
			new_model->vertices[pos] = tx / ts;
			new_model->vertices[pos + 1] = ty / ts;
			new_model->vertices[pos + 2] = tz / ts;
		}
		else
		{
			new_model->vertices[pos] = tx;
			new_model->vertices[pos + 1] = ty;
			new_model->vertices[pos + 2] = tz;
		}
	}
}
float Transform::get_n(const Point3D p1, const Point3D p2, const Point3D p3, Point3D& n)
{
	Point3D a, b;
	a.x = p2.x - p1.x; a.y = p2.y - p1.y; a.z = p2.z - p1.z;
	b.x = p3.x - p1.x; b.y = p3.y - p1.y; b.z = p3.z - p1.z;

	double ra = a.x*a.x + a.y*a.y + a.z*a.z;
	double rb = b.x*b.x + b.y*b.y + b.z*b.z;
	ra = sqrt(ra);
	rb = sqrt(rb);
	a.x /= ra; a.y /= ra; a.z /= ra;
	b.x /= rb; b.y /= rb; b.z /= rb;

	n.x = a.y*b.z - a.z*b.y;
	n.y = a.z*b.x - a.x*b.z;
	n.z = a.x*b.y - a.y*b.x;

	float angle = a.x*b.x + a.y*b.y + a.z*b.z;
	return angle = acos(angle) * 180 / CV_PI;
}
