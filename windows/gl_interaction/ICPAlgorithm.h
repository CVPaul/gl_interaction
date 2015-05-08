/*=================================================================
*
* ICP algorithm implemented in c++
*
* written by:  Per Bergström 2007-10-09
* email: per.bergstrom@ltu.se
* Uses kd-tree written by Guy Shechter
* http://www.mathworks.com/matlabcentral/fileexchange/loadFile.do?objectId=4586&objectType=file
* 
* something of this Algorithm are changed:
* repalce the kd-tree wirte by by Guy Shechter with flann library's kd-tree
* Added a Transform instance to deal with the transform mation
* using the PointCloud Container to as input and out put for conveniet reason
*
* note: original interface are mex interface (conveninet for matlab calling), so if you need a matalb callable interface
        please refer to the original author:http://www.mathworks.com/matlabcentral/fileexchange/16766-iterative-closest-point-method--c++
  note: the data in the result is save by column so use the tranposMat() to transpose the Rotation mat;
* 
*=================================================================*/
#ifndef ICP_ALGORITHM_H
#define ICP_ALGORITHM_H

#include <math.h>
#include <stdio.h>

#include <opencv2\core\core.hpp>
#include <opencv2\flann\flann.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "gl_common.h"
#include "genTexture.h"
#include "PointCloud.h"
#include "TransformPoints.h"

using namespace std;

#if !defined(MAX)
#define	MAX(A, B)	((A) > (B) ? (A) : (B))
#endif

#if !defined(MIN)
#define	MIN(A, B)	((A) < (B) ? (A) : (B))
#endif
namespace ICP
{
	/**
	* this namespace construct the kd-tree with flann library 
	*/
	extern cv::flann::Index flann_index;
	// some global value for demo
	extern PointCloud modelz;
	extern PointCloud dataz;

	/*
	@ show a simple demo of the ICP Algorithm
	* param[in] src: is the source data's path
	* param[in] target: is the target data's path
	* return void
	*/
	void demo(string src, string target);

	/*
	@ spin function for the demo
	*/
	void spin();

	/*
	@ the dispaly functio for the demo
	*/
	void display_icp();

	//---------------------------------------------------------------------------------------------------------
	/**
	* @ return a*a;
	*/
	double pwr2(double a);

	/**@ the classical ICP Algorithm, closet pairs are match by kd-tree
	* param[out] trpr[9]: the rotation matrix save by column
	* param[out] ttpr[3]: the translate vector
	* param[in]: modelz: the refrence PointCloud
	* param[in]: dataz: the source data that want to align to modelz
	* param[in]: n2use: how many point in dataz to use
	* param[in]: iimax: set the max iteration
	* param[in]: flann_index: the kNN  search structure of the build by flann library(kd-tree is used)
	* return[void]
	*/
	void ICP_Alg(float trpr[9], float ttpr[3], const PointCloud& modelz, const PointCloud& dataz,
		unsigned int n2use, unsigned int iimax, cv::flann::Index& flann_index);

	/**@ special design for landmark PointClouds,pair is aligend by index, n2use: is set to min(modelz.size dataz.size)
	* param[out] trpr[9]: the rotation matrix save by column
	* param[out] ttpr[3]: the translate vector
	* param[in]: modelz: the refrence PointCloud
	* param[in]: dataz: the source data that want to align to modelz
	* param[in]: iimax: set the max iteration
	* return[void]
	*/
	void ICP_Alg(float trpr[9], float ttpr[3], const PointCloud& modelz, const PointCloud& dataz,
		unsigned int iimax);// special design for landmark PointClouds,pair is aligend by index, num-of-pair = min(modelz.size dataz.size)

	/**@ the original version witten by  Per Bergström(kd-tree is replaced by flann_index)
	* param[out] trpr[9]: the rotation matrix save by column
	* param[out] ttpr[3]: the translate vector
	* param[in]: modelz: the refrence PointCloud
	* param[in]: nmodelz: the number of points in modelz
	* param[in]: dataz: the source data that want to align to modelz
	* param[in]: qltyz: weight of the each point
	* param[in]: ndataz: the number of points in dataz
	* param[in]: randvecz: a rand permute of the point index convenient for check
	* param[in]: nrandvecz: number of the randvec
	* param[in]: nrandz: how many point in dataz to use,point with index[1:nrandz] in ranvecz are used
	* param[in]: iimax: set the max iteration
	* param[in]: flann_index: the kNN  search structure of the build by flann library(kd-tree is used) 
	* return[void]
	*/
	void ICP_Alg(double *trpr, double *ttpr, double *modelz, unsigned int nmodelz, double *dataz,
		double *qltyz, unsigned int ndataz, unsigned int *randvecz, unsigned int nrandvecz,
		unsigned int nrandz, unsigned int iimax, cv::flann::Index& flann_index);

	/**@ the data in the result is save by column so tranposMat() is used to transpose the Rotation mat;
	* param[in&out] the rotation mat that want to transpose
	* return[void]
	*/
	void tranposMat(float R_mat[9]);

	/**@ convert the PointCloud to cv::Mat
	* param[in] pts: input pointclouds
	* return[cv::Mat] output the pts.size*3 Mat
	*/

	cv::Mat PointCloud2Mat(PointCloud& pts);

	/**@ build the kd-treed with pts, this will transfrom pts into cv::Mat
	* param[in] points: PointCloud transformed by PointCloud2Mat or by other way
	* param[out] findex: flann index result(kd-tree is used at here)
	* return[void]:
	*/
	void build_flann_index(cv::Mat points, cv::flann::Index& findex);
};

#endif /*icp_algorithm.h*/