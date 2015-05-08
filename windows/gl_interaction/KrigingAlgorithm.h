#ifndef KRIGINGALGORITHM_H
#define KRIGINGALGORITHM_H

#include <GL\glut.h>

#include "glm.h"
#include "TransformPoints.h"
#include "PointCloud.h"

struct cov_pts
{
	int index;
	float x, y, z;
};
class Kriging
{
public:
	Kriging();
	virtual ~Kriging();
private:
	float krg_a; // kriging Algorithm param 'a'
	float krg_c; // krigind Algorithm param C1=krg_c - krgnuggest
	float krg_nuggest;// kriging algorithm param
	int n_valid_points;
	cov_pts* valid_points; // coords of the valid points
	Point3D* vpoints_displacement; // save the diplayment of the valid points
	GLMmodel* KrigingModel;
	int mat_dim;
	float* MatrixV; // matrix of the kriging algorithm on the left
	float* VectorD; // vector of the kriging algorithm on the right
public: // operations
	/**@ brief: adjust the model by kriging Algorithm
	* param[in&out] model: the model that want to adjust
	* param[in] marks: a tuple(index, new coordinate(x,y,z)) array, record the mark points of the model that want to adjust
	* param[in] size: size of the mark points
	* return[bool] adjust succeed or failed
	*/
	bool Kriging::AdjustByKriging(GLMmodel*& model, cov_pts* marks, int size);

	/**@ call by AdjustByKriging(), init the n_valid_points with npts( this means remove the invalid points in the input array of marks
	* param[in] size: size of the input array
	* param[in] points: input array
	* return[int]: valid points that copy to n_valid
	*/
	int Kriging::getValidPoints(int size, cov_pts* points);

	/**@ compute the covaraince of the displacement;
	* param[in] displacement: the dispalcement between the new point and the original point
	* return[float] the covaraince of the displacement array
	*/
	float computeCov(Point3D* displacement);// number of the array is n_valid_points;

	/**
	* @ brief: init the MatrixV
	*/
	void initMatrixV(); 

	/**
	* @ compute the invert matrix of Matrix V,return true if it is invertable else false
	*/
	bool invMatrixV();

	/**
	* @ initailize the vector D by point index
	* param[in] nPos: point index
	*/
	void initVecD(int nPos);

	/**
	* @ adjust the model by Kriging Algorithm, with point index
	*/
	Point3D KrigingIntepolate(int nPos);

	/** 
	* @ compute the weight vector W
	*/
	void CalculateMatrixW(float* W);

	/**
	* @ set the model (copy=true means generate a new model)
	*/
	void setModel(GLMmodel* model,bool copy=false);

	/**
	* @ return the model ()
	*/
	GLMmodel* getModel();
};
extern Kriging Kriging_Alg;
#endif