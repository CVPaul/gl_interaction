#ifndef TRANSFORMPOINTS_H
#define TRANSFORMPOINTS_H

/**===============================================================================================
 * class Transform package the common operation for 3d points, this class is desgined to deal
 * with the opengl's can not conviniently get the rendered points's axis at the beginning, but
 * now more function are added to the class

 * note: like pipeline in opengl all the operation will operate on the data only when function  
										TranformInOnce()( recommand )
										rotatePoint()
 * is called!!!, other function will change the trans_mat only 
*==================================================================================================
*/
#include "glm.h"
struct Point3D
{
	float x, y, z;
};

class Transform
{
#define TRANS_MATRIX_DIM 4
#define TRANS_MATRIX_SIZE 16
private:
	float trans_mat[TRANS_MATRIX_SIZE];
public:
	Transform();
	Transform(float mat[TRANS_MATRIX_SIZE]);
	~Transform();
	// the followin part is for 3D point transfomation:

	/** @brief: extract the rotate mat to the _rotate_mat[] and the rotate the point only (no movement))
	* param[in] size: the point to transform
	* param[in] orig_points: original point that expected to transform
	* param[out] new_points: points after tranformed (enough space is needed)
	* return[void]
	*/
	void rotatePoint(int size, Point3D* orig_points, Point3D* new_points);

	/**@ mulit the trans_mat with a roatetion matrix,which is defined by the params:angle and the axis(x,y,z)
	* param[in] angle: angle to rotate(in degree)
	* param[in] (axis_x,axis_y,axis_z): the axis to rotate by
	* return[void] and no output but this will change the trans_mat
	*/
	void Transform::roteByAxis(float angle, float axis_x, float axis_y, float axis_z);


	/**@ mulit the trans_mat with a rotation matrix,which is defined by the 3*3 rot_mat
	* param[in] rot_mat: any rotation matrix
	* return[void] and no output but this will change the trans_mat
	*/
	void Transform::roteByMatrix(float rot_mat[9]);

	/**@ an simple matrix multy funtion
	* param[in&out] mat1: this first parameter of mat1*mat2 whose size is 4*4
	* param[in] mat2: the second parameter of mat1*mat2
	* return[void]
	*/
	void Transform::multi(float mat1[TRANS_MATRIX_SIZE], float mat2[TRANS_MATRIX_SIZE]);

	/**@ mulit this->rotate_mat with a Translate Matrix
	* param[in] Trans_vec: the parameter of the Translate_Matrix
	* return[void] and no output but this will change the trans_mat
	*/
	void Transform::TranslatePoint(float Trans_vec[3]);

	/**@ mulit this->rotate_mat with a Translate Matrix(2)
	* param[in] (Tx,Ty,Tz): define the transform vector
	* return[void] and no output but this will change the trans_mat
	*/
	void Transform::TranslatePoint(float Tx, float Ty, float Tz);


	/**@ Transform all the point in once, this is the real funtion change the points
	* param[in] size: the point to transform
	* param[in] Trans_vec: the parameter of the Translate
	* param[in] Rotate_vec: the parameter of the Rotation
	* param[in] Move_vec: the parameter of the Movation(the defference between movation 
	  and Translate is before or after rotation, the movation is before rotation while 
	  Translate is after rotation
	* param[in] orig_points: original point that expected to transform
	* param[out] new_points: points after tranformed (enough space is needed)
	* return[void]
	*/
	void TranformInOnce(int size, float Trans_vec[3], float Rotate_vec[3], float Move_vec[3],
		Point3D* orig_points, Point3D* new_points);

	/**@ Transform all the point in once(2)!!
	* param[in] size: the point to transform
	* param[in] Trans_vec: the parameter of the Translate
	* param[in] RAngle: angle to rotate(in degree)
	* param[in] rotate_axis: the axis to rotate by
	* param[in] orig_points: original point that expected to transform
	* param[out] new_points: points after tranformed (enough space is needed)
	* return[void]
	*/
	void TranformInOnce(int size, float Trans_vec[3], float RAngle, Point3D rotate_axis,
		Point3D* orig_points, Point3D* new_points);

	/**@ Transform all the point in once(3),no Transform param needed,use the already set params!!
	* param[in] size: the point to transform
	* param[in] orig_points: original point that expected to transform
	* param[out] new_points: points after tranformed (enough space is needed)
	* return[void]
	*/
	void TranformInOnce(int size, Point3D* orig_points, Point3D* new_points);


	/*
	*@ set the trans_mat as Identity matrix.
	*/
	void setIdentity();
	/*
	*@ set the trans_mat as an appointed mat
	*/
	void setRotationMat(float rt_mat[TRANS_MATRIX_SIZE]);

	/**
	@ in case some pramas are double ,we can convert it
	*/
	void double2float(int size, double* db_a, float* ft_a);

	/*
	@ brief: get the angle and normal between two vectors(form (p2-p1) to (p3-p1))
	*/
	float get_n(const Point3D p1, const Point3D p2, const Point3D p3, Point3D& n);

	/**
	*@ print the trans_mat
	*/
	void printMat();

	// the following line begin a part for GLMmodel:
	/*
	@ brief: get the centroid of the model;
	@ param[in&out]:model- model data;
	@ param[in&out]: centroid of the model
	*/
	void glm_centroid(GLMmodel* model, float centroid[3]);
	/*
	@ brief: move the model by mv_vec[3]
	@ param[in&out]: model
	@ param[in]: mv_vec- transform vector
	@ param[in]: decide the the direction of the move direction (+1.0 , -1.0 or any scale)
	*/
	void glm_move(GLMmodel* &model, float mv_vec[3], float symbol);
	/*
	@ brief: Transfrom in Once the function is similar as before but the object change to GLMmodel
	* param[in&out] model: model need to tranform
	* param[in] Trans_vec: the parameter of the Translate
	* param[in] Rotate_vec: the parameter of the Rotation
	* param[in] Move_vec: the parameter of the Movation(the defference between movation
	  and Translate is before or after rotation, the movation is before rotation while
	  Translate is after rotation
	* return[void]
	*/
	void TranformInOnce(GLMmodel* &model, float Trans_vec[3], float Rotate_vec[3], float Move_vec[3]);

	/*
	@ brief: Transfrom in Once the function is similar as before but the object change to GLMmodel(2)
	* param[in&out] model: model need to tranform
	* param[in] Trans_vec: the parameter of the Translate
	* param[in] Move_vec: the parameter of the Movation(the defference between movation
	  and Translate is before or after rotation, the movation is before rotation while
	  Translate is after rotation
	* param[in] RAngle: angle to rotate(in degree)
	* param[in] rotate_axis: the axis to rotate by
	* return[void]
	*/
	void TranformInOnce(GLMmodel* &model, float Trans_vec[3], float Move_vec[3], float RAngle, Point3D rotate_axis);

	/*
	@ Transform all the point in once(3),no Transform param needed,use the already set params!!
	* param[in&out] model: model need to tranform
	* return[void]
	*/
	void TranformInOnce(GLMmodel*& model);

	/*
	@ Transform all the point in once(4),no Transform param needed,use the already set params and generate a new instance
	* param[in] model: model need to tranform
	* param[out] new_model: model that transformed
	*/
	void Transform::TranformInOnce(const GLMmodel* org_model, GLMmodel*& new_model);
};
#endif /*TransformPoint.h*/