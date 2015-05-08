﻿/*=================================================================
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

#include <math.h>
#include <stdio.h>

#include "gl_basic.h"
#include "ICPAlgorithm.h"
#include "TransformPoints.h"

namespace ICP
{
	PointCloud modelz;
	PointCloud dataz;
	cv::flann::Index flann_index;
}

double ICP::pwr2(double a){
	return a*a;
}
void ICP::tranposMat(float R_mat[9])
{
	for (int i = 0; i < 3; i++)// transponse
	{
		for (int j = i + 1; j < 3; j++)
		{
			double temp = R_mat[i * 3 + j];
			R_mat[i * 3 + j] = R_mat[j * 3 + i];
			R_mat[j * 3 + i] = temp;
		}
	}
}
void ICP::ICP_Alg(float trpr[9], float ttpr[3], const PointCloud& modelz, const PointCloud& dataz,
	unsigned int iimax)
{
	int nrandz = min(modelz.size, dataz.size);

	unsigned int i, itemp, j, k, ii, cou = 0;
	double R[9], T[3], distcc;
	double datam[3], modelm[3], MM[3], Spx[9], quasum;
	unsigned short int bol = 1;
	double SIGMA[3];
	double SpxTSpx[6];
	double A, B, C;
	double sqrtA23B;
	double x0, f0;
	double SIp, difl;
	double invmat[6];
	double V[9];
	double U[9];

	memset(trpr, 0, sizeof(float)* 9);
	memset(ttpr, 0, sizeof(float)* 3);

	trpr[0] = 1.0;
	trpr[4] = 1.0;
	trpr[8] = 1.0;

	for (ii = 0; ii<iimax; ii++){
		for (i = 0; i<9; i++){
			Spx[i] = 0.0;
		}
		for (i = 0; i<3; i++){
			modelm[i] = 0.0;
			datam[i] = 0.0;
		}

		quasum = 0.0;
		for (itemp = 0; itemp<dataz.size; itemp++)
		{
			i = itemp;

			if (i>nrandz)break;

			T[0] = trpr[0] * dataz.pc[i].x + trpr[3] * dataz.pc[i].y + trpr[6] * dataz.pc[i].z + ttpr[0];
			T[1] = trpr[1] * dataz.pc[i].x + trpr[4] * dataz.pc[i].y + trpr[7] * dataz.pc[i].z + ttpr[1];
			T[2] = trpr[2] * dataz.pc[i].x + trpr[5] * dataz.pc[i].y + trpr[8] * dataz.pc[i].z + ttpr[2];

			distcc = 0.0;
			A = 0.0;

			k = i;// indices.at<int>(0, 0);

			MM[0] = modelz.pc[k].x;
			MM[1] = modelz.pc[k].y;
			MM[2] = modelz.pc[k].z;

			datam[0] += T[0];
			datam[1] += T[1];
			datam[2] += T[2];

			modelm[0] += MM[0];
			modelm[1] += MM[1];
			modelm[2] += MM[2];

			for (j = 0; j<3; j++){
				Spx[j] += MM[j] * T[0];
				Spx[j + 3] += MM[j] * T[1];
				Spx[j + 6] += MM[j] * T[2];
			}
			quasum += 1.0;
		}
		if (quasum<1e-12){
			break;
		}

		for (j = 0; j<3; j++){
			modelm[j] = modelm[j] / quasum;
			datam[j] = datam[j] / quasum;
		}

		for (j = 0; j<3; j++){
			Spx[j] -= quasum*(modelm[j] * datam[0]);
			Spx[j + 3] -= quasum*(modelm[j] * datam[1]);
			Spx[j + 6] -= quasum*(modelm[j] * datam[2]);
		}

		k = 1;
		if (ii<1){

			distcc = 0.0;
			for (j = 0; j<9; j++){
				distcc += Spx[j] * Spx[j];
			}

			distcc = distcc / pwr2(quasum);

			if (distcc<1e-3){
				k = 0;
				T[0] = modelm[0] - datam[0];
				T[1] = modelm[1] - datam[1];
				T[2] = modelm[2] - datam[2];
				R[0] = 1.0; R[3] = 0.0; R[6] = 0.0;
				R[1] = 0.0; R[4] = 1.0; R[7] = 0.0;
				R[2] = 0.0; R[5] = 0.0; R[8] = 1.0;
			}

		}

		if (k) {

			/* get R*/

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[1] * Spx[1] + Spx[2] * Spx[2];
			SpxTSpx[1] = Spx[3] * Spx[3] + Spx[4] * Spx[4] + Spx[5] * Spx[5];
			SpxTSpx[2] = Spx[6] * Spx[6] + Spx[7] * Spx[7] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[3] + Spx[1] * Spx[4] + Spx[5] * Spx[2];
			SpxTSpx[4] = Spx[3] * Spx[6] + Spx[4] * Spx[7] + Spx[5] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[6] + Spx[1] * Spx[7] + Spx[2] * Spx[8];


			/*   CharacteristicPolynomial  sigma^3-A*sigma^2-B*sigma+C   */

			A = SpxTSpx[2] + SpxTSpx[1] + SpxTSpx[0];
			B = SpxTSpx[5] * SpxTSpx[5] + SpxTSpx[4] * SpxTSpx[4] - SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[2] * SpxTSpx[0] + SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[1] * SpxTSpx[0];
			C = -2 * SpxTSpx[5] * SpxTSpx[3] * SpxTSpx[4] + SpxTSpx[5] * SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[4] * SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[2] * SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[2] * SpxTSpx[1] * SpxTSpx[0];

			sqrtA23B = sqrt(A*A + 3 * B);

			x0 = (A - sqrtA23B) / 3.0;
			f0 = (x0*x0 - A*x0 - B)*x0;

			SIGMA[2] = MIN(MAX((x0*(C + 2 * f0 - 2 * sqrt(f0*(f0 + C))) / f0), 0), 0.5*x0);

			x0 = (A + sqrtA23B) / 3.0;
			f0 = x0*x0*x0 - A*x0*x0 - B*x0 + C;

			SIGMA[0] = MAX(MIN((x0*(B*A - C) + 2 * x0*f0 + 2 * (x0 - A)*sqrt(f0*(f0 + B*A - C)) - A*f0) / (f0 + B*A - C), A), 0.5*(A + x0));

			for (k = 0; k<3; k++){

				if (k == 0){
					j = 0;
				}
				else if (k == 1){
					j = 2;
				}
				else if (k == 2){
					j = 1;
					SIGMA[1] = A - SIGMA[0] - SIGMA[2];
				}

				/* Newton-Raphson */

				for (i = 0; i<50; i++){
					SIp = SIGMA[j];
					difl = (-3 * SIGMA[j] + 2 * A)*SIGMA[j] + B;
					if (fabs(difl)>1e-15){
						SIGMA[j] = ((-2 * SIGMA[j] + A)*SIGMA[j] * SIGMA[j] + C) / difl;
						if (fabs(SIGMA[j] - SIp)<1e-25){
							break;
						}
					}
					else {
						break;
					}
				}
			}

			k = 0;
			if (fabs(SIGMA[1] - SIGMA[0])<1e-12){
				k = 1;
			}

			/* eigenvalues found, corresponding eigenvectors V[i] ... */

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					V[3 * i] = invmat[0];
					V[3 * i + 1] = invmat[1];
					V[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							V[3] = invmat[1];
							V[4] = invmat[3];
							V[5] = invmat[4];

							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

				}
				else {

					/* Eigen vectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					V[6] = V[1] * V[5] - V[2] * V[4];
					V[7] = V[2] * V[3] - V[0] * V[5];
					V[8] = V[0] * V[4] - V[1] * V[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = V[3 * i];
					MM[1] = V[3 * i + 1];
					MM[2] = V[3 * i + 2];

					V[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					V[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					V[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

					distcc = sqrt(pwr2(V[3 * i]) + pwr2(V[3 * i + 1]) + pwr2(V[3 * i + 2]));

					V[3 * i] = V[3 * i] / distcc;
					V[3 * i + 1] = V[3 * i + 1] / distcc;
					V[3 * i + 2] = V[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(V[3 * i] - MM[0]) + pwr2(V[3 * i + 1] - MM[1]) + pwr2(V[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}
			}

			/* singular values & V[i] of Spx found, U[i] ... */

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[3] * Spx[3] + Spx[6] * Spx[6];
			SpxTSpx[1] = Spx[1] * Spx[1] + Spx[4] * Spx[4] + Spx[7] * Spx[7];
			SpxTSpx[2] = Spx[2] * Spx[2] + Spx[5] * Spx[5] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[1] + Spx[3] * Spx[4] + Spx[6] * Spx[7];
			SpxTSpx[4] = Spx[1] * Spx[2] + Spx[4] * Spx[5] + Spx[7] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[2] + Spx[3] * Spx[5] + Spx[6] * Spx[8];

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					U[3 * i] = invmat[0];
					U[3 * i + 1] = invmat[1];
					U[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							U[3] = invmat[1];
							U[4] = invmat[3];
							U[5] = invmat[4];

							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

				}
				else {

					/* Eigenvectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					U[6] = U[1] * U[5] - U[2] * U[4];
					U[7] = U[2] * U[3] - U[0] * U[5];
					U[8] = U[0] * U[4] - U[1] * U[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = U[3 * i];
					MM[1] = U[3 * i + 1];
					MM[2] = U[3 * i + 2];

					U[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					U[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					U[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

					distcc = sqrt(pwr2(U[3 * i]) + pwr2(U[3 * i + 1]) + pwr2(U[3 * i + 2]));

					U[3 * i] = U[3 * i] / distcc;
					U[3 * i + 1] = U[3 * i + 1] / distcc;
					U[3 * i + 2] = U[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(U[3 * i] - MM[0]) + pwr2(U[3 * i + 1] - MM[1]) + pwr2(U[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}

			}

			k = 0;
			for (i = 0; i<3; i++){
				A = (Spx[0] * V[3 * i] + Spx[3] * V[3 * i + 1] + Spx[6] * V[3 * i + 2])*U[3 * i] + (Spx[1] * V[3 * i] + Spx[4] * V[3 * i + 1] + Spx[7] * V[3 * i + 2])*U[3 * i + 1] + (Spx[2] * V[3 * i] + Spx[5] * V[3 * i + 1] + Spx[8] * V[3 * i + 2])*U[3 * i + 2];
				if (A<0){
					k++;
					U[3 * i] = -U[3 * i];
					U[3 * i + 1] = -U[3 * i + 1];
					U[3 * i + 2] = -U[3 * i + 2];
				}
			}

			/* Get R=U*diag([1,1,det(U*V')])*V' */

			if (k == 0 || k == 2){           /* det(U*V')=+1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] + U[i + 6] * V[j + 6];
					}
				}
			}
			else{                        /* det(U*V')=-1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] - U[i + 6] * V[j + 6];
					}
				}
			}

			/* Get T=modelm-R*datam */

			T[0] = modelm[0] - R[0] * datam[0] - R[3] * datam[1] - R[6] * datam[2];
			T[1] = modelm[1] - R[1] * datam[0] - R[4] * datam[1] - R[7] * datam[2];
			T[2] = modelm[2] - R[2] * datam[0] - R[5] * datam[1] - R[8] * datam[2];

		}

		/* Get TR */

		for (j = 0; j<3; j++){
			MM[0] = trpr[3 * j];
			MM[1] = trpr[3 * j + 1];
			MM[2] = trpr[3 * j + 2];

			trpr[3 * j] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2];
			trpr[1 + 3 * j] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2];
			trpr[2 + 3 * j] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2];
		}

		/* Get TT */

		MM[0] = ttpr[0];
		MM[1] = ttpr[1];
		MM[2] = ttpr[2];

		ttpr[0] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2] + T[0];
		ttpr[1] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2] + T[1];
		ttpr[2] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2] + T[2];

	}

}
void ICP::ICP_Alg(float trpr[9], float ttpr[3], const PointCloud& modelz, const PointCloud& dataz,
	unsigned int nrandz, unsigned int iimax, cv::flann::Index& flann_index)
{

	unsigned int i, itemp, j, k, ii, cou = 0;
	double R[9], T[3], distcc;
	double datam[3], modelm[3], MM[3], Spx[9], quasum;
	unsigned short int bol = 1;
	double SIGMA[3];
	double SpxTSpx[6];
	double A, B, C;
	double sqrtA23B;
	double x0, f0;
	double SIp, difl;
	double invmat[6];
	double V[9];
	double U[9];

	memset(trpr, 0, sizeof(float)* 9);
	memset(ttpr, 0, sizeof(float)* 3);

	trpr[0] = 1.0;
	trpr[4] = 1.0;
	trpr[8] = 1.0;

	for (ii = 0; ii<iimax; ii++){
		for (i = 0; i<9; i++){
			Spx[i] = 0.0;
		}
		for (i = 0; i<3; i++){
			modelm[i] = 0.0;
			datam[i] = 0.0;
		}

		quasum = 0.0;
		for (itemp = 0; itemp<dataz.size; itemp++)
		{
			i = itemp;

			if (i>nrandz)break;

			T[0] = trpr[0] * dataz.pc[i].x + trpr[3] * dataz.pc[i].y + trpr[6] * dataz.pc[i].z + ttpr[0];
			T[1] = trpr[1] * dataz.pc[i].x + trpr[4] * dataz.pc[i].y + trpr[7] * dataz.pc[i].z + ttpr[1];
			T[2] = trpr[2] * dataz.pc[i].x + trpr[5] * dataz.pc[i].y + trpr[8] * dataz.pc[i].z + ttpr[2];

			distcc = 0.0;
			A = 0.0;

			float temp[3] = { T[0], T[1], T[2] };
			cv::Mat query(1, 3, CV_32FC1, temp);

			cv::Mat indices, dist;
			flann_index.knnSearch(query, indices, dist, 1);
			k = indices.at<int>(0, 0);

			MM[0] = modelz.pc[k].x;
			MM[1] = modelz.pc[k].y;
			MM[2] = modelz.pc[k].z;

			datam[0] += T[0];
			datam[1] += T[1];
			datam[2] += T[2];

			modelm[0] += MM[0];
			modelm[1] += MM[1];
			modelm[2] += MM[2];

			for (j = 0; j<3; j++){
				Spx[j] += MM[j] * T[0];
				Spx[j + 3] += MM[j] * T[1];
				Spx[j + 6] += MM[j] * T[2];
			}
			quasum += 1.0;
		}
		if (quasum<1e-12){
			break;
		}

		for (j = 0; j<3; j++){
			modelm[j] = modelm[j] / quasum;
			datam[j] = datam[j] / quasum;
		}

		for (j = 0; j<3; j++){
			Spx[j] -= quasum*(modelm[j] * datam[0]);
			Spx[j + 3] -= quasum*(modelm[j] * datam[1]);
			Spx[j + 6] -= quasum*(modelm[j] * datam[2]);
		}

		k = 1;
		if (ii<1){

			distcc = 0.0;
			for (j = 0; j<9; j++){
				distcc += Spx[j] * Spx[j];
			}

			distcc = distcc / pwr2(quasum);

			if (distcc<1e-3){
				k = 0;
				T[0] = modelm[0] - datam[0];
				T[1] = modelm[1] - datam[1];
				T[2] = modelm[2] - datam[2];
				R[0] = 1.0; R[3] = 0.0; R[6] = 0.0;
				R[1] = 0.0; R[4] = 1.0; R[7] = 0.0;
				R[2] = 0.0; R[5] = 0.0; R[8] = 1.0;
			}

		}

		if (k) {

			/* get R*/

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[1] * Spx[1] + Spx[2] * Spx[2];
			SpxTSpx[1] = Spx[3] * Spx[3] + Spx[4] * Spx[4] + Spx[5] * Spx[5];
			SpxTSpx[2] = Spx[6] * Spx[6] + Spx[7] * Spx[7] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[3] + Spx[1] * Spx[4] + Spx[5] * Spx[2];
			SpxTSpx[4] = Spx[3] * Spx[6] + Spx[4] * Spx[7] + Spx[5] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[6] + Spx[1] * Spx[7] + Spx[2] * Spx[8];


			/*   CharacteristicPolynomial  sigma^3-A*sigma^2-B*sigma+C   */

			A = SpxTSpx[2] + SpxTSpx[1] + SpxTSpx[0];
			B = SpxTSpx[5] * SpxTSpx[5] + SpxTSpx[4] * SpxTSpx[4] - SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[2] * SpxTSpx[0] + SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[1] * SpxTSpx[0];
			C = -2 * SpxTSpx[5] * SpxTSpx[3] * SpxTSpx[4] + SpxTSpx[5] * SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[4] * SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[2] * SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[2] * SpxTSpx[1] * SpxTSpx[0];

			sqrtA23B = sqrt(A*A + 3 * B);

			x0 = (A - sqrtA23B) / 3.0;
			f0 = (x0*x0 - A*x0 - B)*x0;

			SIGMA[2] = MIN(MAX((x0*(C + 2 * f0 - 2 * sqrt(f0*(f0 + C))) / f0), 0), 0.5*x0);

			x0 = (A + sqrtA23B) / 3.0;
			f0 = x0*x0*x0 - A*x0*x0 - B*x0 + C;

			SIGMA[0] = MAX(MIN((x0*(B*A - C) + 2 * x0*f0 + 2 * (x0 - A)*sqrt(f0*(f0 + B*A - C)) - A*f0) / (f0 + B*A - C), A), 0.5*(A + x0));

			for (k = 0; k<3; k++){

				if (k == 0){
					j = 0;
				}
				else if (k == 1){
					j = 2;
				}
				else if (k == 2){
					j = 1;
					SIGMA[1] = A - SIGMA[0] - SIGMA[2];
				}

				/* Newton-Raphson */

				for (i = 0; i<50; i++){
					SIp = SIGMA[j];
					difl = (-3 * SIGMA[j] + 2 * A)*SIGMA[j] + B;
					if (fabs(difl)>1e-15){
						SIGMA[j] = ((-2 * SIGMA[j] + A)*SIGMA[j] * SIGMA[j] + C) / difl;
						if (fabs(SIGMA[j] - SIp)<1e-25){
							break;
						}
					}
					else {
						break;
					}
				}
			}

			k = 0;
			if (fabs(SIGMA[1] - SIGMA[0])<1e-12){
				k = 1;
			}

			/* eigenvalues found, corresponding eigenvectors V[i] ... */

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					V[3 * i] = invmat[0];
					V[3 * i + 1] = invmat[1];
					V[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							V[3] = invmat[1];
							V[4] = invmat[3];
							V[5] = invmat[4];

							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

				}
				else {

					/* Eigen vectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					V[6] = V[1] * V[5] - V[2] * V[4];
					V[7] = V[2] * V[3] - V[0] * V[5];
					V[8] = V[0] * V[4] - V[1] * V[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = V[3 * i];
					MM[1] = V[3 * i + 1];
					MM[2] = V[3 * i + 2];

					V[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					V[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					V[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

					distcc = sqrt(pwr2(V[3 * i]) + pwr2(V[3 * i + 1]) + pwr2(V[3 * i + 2]));

					V[3 * i] = V[3 * i] / distcc;
					V[3 * i + 1] = V[3 * i + 1] / distcc;
					V[3 * i + 2] = V[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(V[3 * i] - MM[0]) + pwr2(V[3 * i + 1] - MM[1]) + pwr2(V[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}
			}

			/* singular values & V[i] of Spx found, U[i] ... */

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[3] * Spx[3] + Spx[6] * Spx[6];
			SpxTSpx[1] = Spx[1] * Spx[1] + Spx[4] * Spx[4] + Spx[7] * Spx[7];
			SpxTSpx[2] = Spx[2] * Spx[2] + Spx[5] * Spx[5] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[1] + Spx[3] * Spx[4] + Spx[6] * Spx[7];
			SpxTSpx[4] = Spx[1] * Spx[2] + Spx[4] * Spx[5] + Spx[7] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[2] + Spx[3] * Spx[5] + Spx[6] * Spx[8];

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					U[3 * i] = invmat[0];
					U[3 * i + 1] = invmat[1];
					U[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							U[3] = invmat[1];
							U[4] = invmat[3];
							U[5] = invmat[4];

							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

				}
				else {

					/* Eigenvectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					U[6] = U[1] * U[5] - U[2] * U[4];
					U[7] = U[2] * U[3] - U[0] * U[5];
					U[8] = U[0] * U[4] - U[1] * U[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = U[3 * i];
					MM[1] = U[3 * i + 1];
					MM[2] = U[3 * i + 2];

					U[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					U[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					U[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

					distcc = sqrt(pwr2(U[3 * i]) + pwr2(U[3 * i + 1]) + pwr2(U[3 * i + 2]));

					U[3 * i] = U[3 * i] / distcc;
					U[3 * i + 1] = U[3 * i + 1] / distcc;
					U[3 * i + 2] = U[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(U[3 * i] - MM[0]) + pwr2(U[3 * i + 1] - MM[1]) + pwr2(U[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}

			}

			k = 0;
			for (i = 0; i<3; i++){
				A = (Spx[0] * V[3 * i] + Spx[3] * V[3 * i + 1] + Spx[6] * V[3 * i + 2])*U[3 * i] + (Spx[1] * V[3 * i] + Spx[4] * V[3 * i + 1] + Spx[7] * V[3 * i + 2])*U[3 * i + 1] + (Spx[2] * V[3 * i] + Spx[5] * V[3 * i + 1] + Spx[8] * V[3 * i + 2])*U[3 * i + 2];
				if (A<0){
					k++;
					U[3 * i] = -U[3 * i];
					U[3 * i + 1] = -U[3 * i + 1];
					U[3 * i + 2] = -U[3 * i + 2];
				}
			}

			/* Get R=U*diag([1,1,det(U*V')])*V' */

			if (k == 0 || k == 2){           /* det(U*V')=+1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] + U[i + 6] * V[j + 6];
					}
				}
			}
			else{                        /* det(U*V')=-1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] - U[i + 6] * V[j + 6];
					}
				}
			}

			/* Get T=modelm-R*datam */

			T[0] = modelm[0] - R[0] * datam[0] - R[3] * datam[1] - R[6] * datam[2];
			T[1] = modelm[1] - R[1] * datam[0] - R[4] * datam[1] - R[7] * datam[2];
			T[2] = modelm[2] - R[2] * datam[0] - R[5] * datam[1] - R[8] * datam[2];

		}

		/* Get TR */

		for (j = 0; j<3; j++){
			MM[0] = trpr[3 * j];
			MM[1] = trpr[3 * j + 1];
			MM[2] = trpr[3 * j + 2];

			trpr[3 * j] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2];
			trpr[1 + 3 * j] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2];
			trpr[2 + 3 * j] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2];
		}

		/* Get TT */

		MM[0] = ttpr[0];
		MM[1] = ttpr[1];
		MM[2] = ttpr[2];

		ttpr[0] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2] + T[0];
		ttpr[1] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2] + T[1];
		ttpr[2] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2] + T[2];

	}

}
void ICP::ICP_Alg(
	double *trpr,
	double *ttpr,
	double *modelz,
	unsigned int nmodelz,
	double *dataz,
	double *qltyz,
	unsigned int ndataz,
	unsigned int *randvecz,
	unsigned int nrandvecz,
	unsigned int nrandz,
	unsigned int iimax,
	cv::flann::Index& flann_index
	)
{

	unsigned int i, itemp, j, k, ii, cou = 0;
	double R[9], T[3], distcc;
	double datam[3], modelm[3], MM[3], Spx[9], quasum;
	unsigned short int bol = 1;
	double SIGMA[3];
	double SpxTSpx[6];
	double A, B, C;
	double sqrtA23B;
	double x0, f0;
	double SIp, difl;
	double invmat[6];
	double V[9];
	double U[9];

	memset(trpr, 0, sizeof(double)* 9);
	memset(ttpr, 0, sizeof(double)* 3);

	trpr[0] = 1.0;
	trpr[4] = 1.0;
	trpr[8] = 1.0;

	if ((nrandz + 1)>ndataz){
		bol = 0;
	}

	for (ii = 0; ii<iimax; ii++){

		for (i = 0; i<9; i++){
			Spx[i] = 0.0;
		}
		for (i = 0; i<3; i++){
			modelm[i] = 0.0;
			datam[i] = 0.0;
		}

		quasum = 0.0;
		for (itemp = 0; itemp<ndataz; itemp++){

			if (bol){
				if (itemp<nrandz){
					i = randvecz[cou];

					if (cou<(nrandvecz - 1)){
						cou++;
					}
					else{
						cou = 0;
					}
				}
				else{
					break;
				}
			}
			else{
				i = itemp;
			}

			if (qltyz[i]>0.0){

				T[0] = trpr[0] * dataz[3 * i] + trpr[3] * dataz[3 * i + 1] + trpr[6] * dataz[3 * i + 2] + ttpr[0];
				T[1] = trpr[1] * dataz[3 * i] + trpr[4] * dataz[3 * i + 1] + trpr[7] * dataz[3 * i + 2] + ttpr[1];
				T[2] = trpr[2] * dataz[3 * i] + trpr[5] * dataz[3 * i + 1] + trpr[8] * dataz[3 * i + 2] + ttpr[2];

				distcc = 0.0;
				A = 0.0;

				float temp[3] = { T[0], T[1], T[2] };
				cv::Mat query(1, 3, CV_32FC1, temp);

				cv::Mat indices, dist;
				flann_index.knnSearch(query, indices, dist, 1);
				k = indices.at<int>(0, 0);

				//run_queries(tree->rootptr, T, 1, 3, &A, &distcc, RETURN_INDEX);
				//k = (unsigned int)A;
				//mexPrintf("index=%d\n",k);

				MM[0] = qltyz[i] * modelz[3 * k];
				MM[1] = qltyz[i] * modelz[3 * k + 1];
				MM[2] = qltyz[i] * modelz[3 * k + 2];

				datam[0] += qltyz[i] * T[0];
				datam[1] += qltyz[i] * T[1];
				datam[2] += qltyz[i] * T[2];

				modelm[0] += MM[0];
				modelm[1] += MM[1];
				modelm[2] += MM[2];

				for (j = 0; j<3; j++){
					Spx[j] += MM[j] * T[0];
					Spx[j + 3] += MM[j] * T[1];
					Spx[j + 6] += MM[j] * T[2];
				}
				quasum += qltyz[i];
			}

		}

		if (quasum<1e-12){
			break;
		}

		for (j = 0; j<3; j++){
			modelm[j] = modelm[j] / quasum;
			datam[j] = datam[j] / quasum;
		}

		for (j = 0; j<3; j++){
			Spx[j] -= quasum*(modelm[j] * datam[0]);
			Spx[j + 3] -= quasum*(modelm[j] * datam[1]);
			Spx[j + 6] -= quasum*(modelm[j] * datam[2]);
		}

		k = 1;
		if (ii<1){

			distcc = 0.0;
			for (j = 0; j<9; j++){
				distcc += Spx[j] * Spx[j];
			}

			distcc = distcc / pwr2(quasum);

			if (distcc<1e-3){
				k = 0;
				T[0] = modelm[0] - datam[0];
				T[1] = modelm[1] - datam[1];
				T[2] = modelm[2] - datam[2];
				R[0] = 1.0; R[3] = 0.0; R[6] = 0.0;
				R[1] = 0.0; R[4] = 1.0; R[7] = 0.0;
				R[2] = 0.0; R[5] = 0.0; R[8] = 1.0;
			}

		}

		if (k) {

			/* get R*/

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[1] * Spx[1] + Spx[2] * Spx[2];
			SpxTSpx[1] = Spx[3] * Spx[3] + Spx[4] * Spx[4] + Spx[5] * Spx[5];
			SpxTSpx[2] = Spx[6] * Spx[6] + Spx[7] * Spx[7] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[3] + Spx[1] * Spx[4] + Spx[5] * Spx[2];
			SpxTSpx[4] = Spx[3] * Spx[6] + Spx[4] * Spx[7] + Spx[5] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[6] + Spx[1] * Spx[7] + Spx[2] * Spx[8];


			/*   CharacteristicPolynomial  sigma^3-A*sigma^2-B*sigma+C   */

			A = SpxTSpx[2] + SpxTSpx[1] + SpxTSpx[0];
			B = SpxTSpx[5] * SpxTSpx[5] + SpxTSpx[4] * SpxTSpx[4] - SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[2] * SpxTSpx[0] + SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[1] * SpxTSpx[0];
			C = -2 * SpxTSpx[5] * SpxTSpx[3] * SpxTSpx[4] + SpxTSpx[5] * SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[4] * SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[2] * SpxTSpx[3] * SpxTSpx[3] - SpxTSpx[2] * SpxTSpx[1] * SpxTSpx[0];

			sqrtA23B = sqrt(A*A + 3 * B);

			x0 = (A - sqrtA23B) / 3.0;
			f0 = (x0*x0 - A*x0 - B)*x0;

			SIGMA[2] = MIN(MAX((x0*(C + 2 * f0 - 2 * sqrt(f0*(f0 + C))) / f0), 0), 0.5*x0);

			x0 = (A + sqrtA23B) / 3.0;
			f0 = x0*x0*x0 - A*x0*x0 - B*x0 + C;

			SIGMA[0] = MAX(MIN((x0*(B*A - C) + 2 * x0*f0 + 2 * (x0 - A)*sqrt(f0*(f0 + B*A - C)) - A*f0) / (f0 + B*A - C), A), 0.5*(A + x0));

			for (k = 0; k<3; k++){

				if (k == 0){
					j = 0;
				}
				else if (k == 1){
					j = 2;
				}
				else if (k == 2){
					j = 1;
					SIGMA[1] = A - SIGMA[0] - SIGMA[2];
				}

				/* Newton-Raphson */

				for (i = 0; i<50; i++){
					SIp = SIGMA[j];
					difl = (-3 * SIGMA[j] + 2 * A)*SIGMA[j] + B;
					if (fabs(difl)>1e-15){
						SIGMA[j] = ((-2 * SIGMA[j] + A)*SIGMA[j] * SIGMA[j] + C) / difl;
						if (fabs(SIGMA[j] - SIp)<1e-25){
							break;
						}
					}
					else {
						break;
					}
				}
			}

			k = 0;
			if (fabs(SIGMA[1] - SIGMA[0])<1e-12){
				k = 1;
			}

			/* eigenvalues found, corresponding eigenvectors V[i] ... */

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					V[3 * i] = invmat[0];
					V[3 * i + 1] = invmat[1];
					V[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							V[3] = invmat[1];
							V[4] = invmat[3];
							V[5] = invmat[4];

							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

				}
				else {

					/* Eigen vectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					V[6] = V[1] * V[5] - V[2] * V[4];
					V[7] = V[2] * V[3] - V[0] * V[5];
					V[8] = V[0] * V[4] - V[1] * V[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = V[3 * i];
					MM[1] = V[3 * i + 1];
					MM[2] = V[3 * i + 2];

					V[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					V[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					V[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = V[3] * V[0] + V[4] * V[1] + V[5] * V[2];
							V[3] = V[3] - distcc*V[0];
							V[4] = V[4] - distcc*V[1];
							V[5] = V[5] - distcc*V[2];
						}
					}

					distcc = sqrt(pwr2(V[3 * i]) + pwr2(V[3 * i + 1]) + pwr2(V[3 * i + 2]));

					V[3 * i] = V[3 * i] / distcc;
					V[3 * i + 1] = V[3 * i + 1] / distcc;
					V[3 * i + 2] = V[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(V[3 * i] - MM[0]) + pwr2(V[3 * i + 1] - MM[1]) + pwr2(V[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}
			}

			/* singular values & V[i] of Spx found, U[i] ... */

			SpxTSpx[0] = Spx[0] * Spx[0] + Spx[3] * Spx[3] + Spx[6] * Spx[6];
			SpxTSpx[1] = Spx[1] * Spx[1] + Spx[4] * Spx[4] + Spx[7] * Spx[7];
			SpxTSpx[2] = Spx[2] * Spx[2] + Spx[5] * Spx[5] + Spx[8] * Spx[8];
			SpxTSpx[3] = Spx[0] * Spx[1] + Spx[3] * Spx[4] + Spx[6] * Spx[7];
			SpxTSpx[4] = Spx[1] * Spx[2] + Spx[4] * Spx[5] + Spx[7] * Spx[8];
			SpxTSpx[5] = Spx[0] * Spx[2] + Spx[3] * Spx[5] + Spx[6] * Spx[8];

			for (i = 0; i<3; i++){

				invmat[0] = SpxTSpx[2] * SpxTSpx[1] - SpxTSpx[1] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[4] * SpxTSpx[4];
				invmat[1] = SpxTSpx[4] * SpxTSpx[5] - SpxTSpx[3] * SpxTSpx[2] + SpxTSpx[3] * SIGMA[i];
				invmat[2] = SpxTSpx[3] * SpxTSpx[4] - SpxTSpx[5] * SpxTSpx[1] + SpxTSpx[5] * SIGMA[i];
				invmat[3] = SpxTSpx[2] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SIGMA[i] * SpxTSpx[2] + SIGMA[i] * SIGMA[i] - SpxTSpx[5] * SpxTSpx[5];
				invmat[4] = -SpxTSpx[4] * SpxTSpx[0] + SpxTSpx[4] * SIGMA[i] + SpxTSpx[3] * SpxTSpx[5];
				invmat[5] = SpxTSpx[1] * SpxTSpx[0] - SpxTSpx[0] * SIGMA[i] - SpxTSpx[1] * SIGMA[i] + SIGMA[i] * SIGMA[i] - SpxTSpx[3] * SpxTSpx[3];

				if (i<2){
					U[3 * i] = invmat[0];
					U[3 * i + 1] = invmat[1];
					U[3 * i + 2] = invmat[2];

					if (k){
						if (i == 1){
							U[3] = invmat[1];
							U[4] = invmat[3];
							U[5] = invmat[4];

							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

				}
				else {

					/* Eigenvectors corresponding to symmetric positiv definite matrices
					are orthogonal. */

					U[6] = U[1] * U[5] - U[2] * U[4];
					U[7] = U[2] * U[3] - U[0] * U[5];
					U[8] = U[0] * U[4] - U[1] * U[3];
				}

				for (j = 0; j<10; j++){

					MM[0] = U[3 * i];
					MM[1] = U[3 * i + 1];
					MM[2] = U[3 * i + 2];

					U[3 * i] = invmat[0] * MM[0] + invmat[1] * MM[1] + invmat[2] * MM[2];
					U[3 * i + 1] = invmat[1] * MM[0] + invmat[3] * MM[1] + invmat[4] * MM[2];
					U[3 * i + 2] = invmat[2] * MM[0] + invmat[4] * MM[1] + invmat[5] * MM[2];

					if (k){
						if (i == 1){
							distcc = U[3] * U[0] + U[4] * U[1] + U[5] * U[2];
							U[3] = U[3] - distcc*U[0];
							U[4] = U[4] - distcc*U[1];
							U[5] = U[5] - distcc*U[2];
						}
					}

					distcc = sqrt(pwr2(U[3 * i]) + pwr2(U[3 * i + 1]) + pwr2(U[3 * i + 2]));

					U[3 * i] = U[3 * i] / distcc;
					U[3 * i + 1] = U[3 * i + 1] / distcc;
					U[3 * i + 2] = U[3 * i + 2] / distcc;

					if (j>2){
						if ((pwr2(U[3 * i] - MM[0]) + pwr2(U[3 * i + 1] - MM[1]) + pwr2(U[3 * i + 2] - MM[2]))<1e-29){
							break;
						}
					}

				}

			}

			k = 0;
			for (i = 0; i<3; i++){
				A = (Spx[0] * V[3 * i] + Spx[3] * V[3 * i + 1] + Spx[6] * V[3 * i + 2])*U[3 * i] + (Spx[1] * V[3 * i] + Spx[4] * V[3 * i + 1] + Spx[7] * V[3 * i + 2])*U[3 * i + 1] + (Spx[2] * V[3 * i] + Spx[5] * V[3 * i + 1] + Spx[8] * V[3 * i + 2])*U[3 * i + 2];
				if (A<0){
					k++;
					U[3 * i] = -U[3 * i];
					U[3 * i + 1] = -U[3 * i + 1];
					U[3 * i + 2] = -U[3 * i + 2];
				}
			}

			/* Get R=U*diag([1,1,det(U*V')])*V' */

			if (k == 0 || k == 2){           /* det(U*V')=+1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] + U[i + 6] * V[j + 6];
					}
				}
			}
			else{                        /* det(U*V')=-1 */
				for (i = 0; i<3; i++){
					for (j = 0; j<3; j++){
						R[i + 3 * j] = U[i] * V[j] + U[i + 3] * V[j + 3] - U[i + 6] * V[j + 6];
					}
				}
			}

			/* Get T=modelm-R*datam */

			T[0] = modelm[0] - R[0] * datam[0] - R[3] * datam[1] - R[6] * datam[2];
			T[1] = modelm[1] - R[1] * datam[0] - R[4] * datam[1] - R[7] * datam[2];
			T[2] = modelm[2] - R[2] * datam[0] - R[5] * datam[1] - R[8] * datam[2];

		}

		/* Get TR */

		for (j = 0; j<3; j++){
			MM[0] = trpr[3 * j];
			MM[1] = trpr[3 * j + 1];
			MM[2] = trpr[3 * j + 2];

			trpr[3 * j] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2];
			trpr[1 + 3 * j] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2];
			trpr[2 + 3 * j] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2];
		}

		/* Get TT */

		MM[0] = ttpr[0];
		MM[1] = ttpr[1];
		MM[2] = ttpr[2];

		ttpr[0] = R[0] * MM[0] + R[3] * MM[1] + R[6] * MM[2] + T[0];
		ttpr[1] = R[1] * MM[0] + R[4] * MM[1] + R[7] * MM[2] + T[1];
		ttpr[2] = R[2] * MM[0] + R[5] * MM[1] + R[8] * MM[2] + T[2];

	}
}
cv::Mat ICP::PointCloud2Mat(PointCloud& pts)
{
	cv::Mat re;
	float xyz[3];
	for (int k = 0; k < pts.size; k++)
	{
		xyz[0] = pts.pc[k].x;
		xyz[1] = pts.pc[k].y;
		xyz[2] = pts.pc[k].z;
		re.push_back(cv::Mat(1, 3, CV_32FC1, xyz));
	}
	return re;
}

void ICP::build_flann_index(cv::Mat points, cv::flann::Index& findex)
{
	findex.build(points, cv::flann::KDTreeIndexParams());
}
void ICP::demo(string src,string target)
{
	// Part 1: init part

	dataz.load(src);
	modelz.load(target);
	cv::Mat points = PointCloud2Mat(modelz);
	build_flann_index(points, flann_index);

	//glutInit(&argc, argv); this line connect with the console like the main???

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GL_Show");
	GL::init_opengl();
	// Part 2: interactive part 
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MouseEvent);
	glutKeyboardFunc(KeyFunc);

	glutDisplayFunc(display_icp);
	glutIdleFunc(spin);
	glutReshapeFunc(GL::reshape);
	glutMainLoop();
}
void ICP::spin()
{
	float R_mat[9], T_vec[3];
	ICP_Alg(R_mat, T_vec, modelz, dataz, 100, 1, ICP::flann_index);

	Transform trans;
	tranposMat(R_mat); // transpose the mat
	trans.TranslatePoint(T_vec[0], T_vec[1], T_vec[2]);

	trans.roteByMatrix(R_mat);
	trans.TranformInOnce(dataz.size, dataz.pc, dataz.pc);
	//trans.printMat();
	display_icp();
}
void ICP::display_icp()
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
		0, 3, 3,
		0, 0, 0.0,
		0, 1.0, 0);
	glTranslatef(Trans[0],Trans[1]-2,Trans[2]);
	glRotatef(Rotate[0]+200, 1.0, 0.0, 0.0);
	glRotatef(Rotate[1], 0.0, 1.0, 0.0);
	glRotatef(Rotate[2], 0.0, 0.0, 2.0);

	modelz.draw(); // white
	dataz.draw(1.0, 0.0, 0.0);// red
	glutSwapBuffers();
}