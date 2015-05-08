#include "KrigingAlgorithm.h"
#include <stdio.h>
#include <cmath>

#define FALSE 0
#define TRUE 1

Kriging Kriging_Alg;

Kriging::Kriging()
{

}
Kriging::~Kriging()
{
	if (valid_points) delete[] valid_points; // coords of the valid points
	if (vpoints_displacement) delete[] vpoints_displacement; // save the diplayment of the valid points
	if (MatrixV) delete[] MatrixV; // matrix of the kriging algorithm on the left
	if (VectorD) delete[] VectorD; // vector of the kriging algorithm on the right
}
void Kriging::setModel(GLMmodel* model,bool copy)
{
	if (copy)this->KrigingModel=glmCopyOBJ(model);
	else this->KrigingModel = model;
}
GLMmodel* Kriging::getModel()
{
	return KrigingModel;
}
void Kriging::initMatrixV()
{
	int i, j;
	float fTemp;
	float C0, C1;

	int posi = 0, posj = 0;
	for (i = 0; i<mat_dim - 1; i++)
	{
		posi = valid_points[i].index * 3;
		for (j = i; j<mat_dim - 1; j++)
		{
			posj = valid_points[j].index * 3;
			fTemp = (KrigingModel->vertices[posi] - KrigingModel->vertices[posj])*(KrigingModel->vertices[posi] - KrigingModel->vertices[posj]) +
				(KrigingModel->vertices[posi + 1] - KrigingModel->vertices[posj + 1])*(KrigingModel->vertices[posi + 1] - KrigingModel->vertices[posj + 1]) +
				(KrigingModel->vertices[posi + 2] - KrigingModel->vertices[posj + 2])*(KrigingModel->vertices[posi + 2] - KrigingModel->vertices[posj + 2]);
			MatrixV[i*mat_dim + j] = (float)sqrt(fTemp);
		}
	}

	C0 = 0.0;
	C1 = krg_c - krg_nuggest;
	for (i = 0; i<mat_dim - 1; i++)
	{
		for (j = i; j<mat_dim - 1; j++)
		{
			*(MatrixV + i*mat_dim + j) = C0 + C1 * (1 - (float)exp(-3.0f**(MatrixV + i*mat_dim + j) / krg_a));
			*(MatrixV + j*mat_dim + i) = *(MatrixV + i*mat_dim + j);
		}
		*(MatrixV + i*mat_dim + mat_dim - 1) = 1.0f;
		*(MatrixV + (mat_dim - 1)*mat_dim + i) = 1.0f;
	}
	*(MatrixV + (mat_dim - 1)*mat_dim + mat_dim - 1) = 0.0f;
}
bool Kriging::AdjustByKriging(GLMmodel*& model, cov_pts* marks, int size) // adjust the model by kriging Algorithm
{
	int i;
	n_valid_points = getValidPoints(size, marks);
	if (n_valid_points < 1)
	{
		printf("no valid points geted! return with nothing to do(in AdjustByKriging())\n");
		return false;
	}
	mat_dim = n_valid_points + 1;

	// allocate the memeory
	vpoints_displacement = (Point3D*)malloc(n_valid_points*sizeof(Point3D));
	MatrixV = (float*)malloc(mat_dim*mat_dim*sizeof(float));
	VectorD = (float*)malloc(mat_dim*sizeof(float));

	// caiculate the displacement
	int dpos = 0;
	for (i = 0; i<n_valid_points; i++)
	{
		dpos = marks[i].index * 3;
		vpoints_displacement[i].x = marks[i].x - KrigingModel->vertices[dpos];
		vpoints_displacement[i].y = marks[i].y - KrigingModel->vertices[dpos + 1];
		vpoints_displacement[i].z = marks[i].z - KrigingModel->vertices[dpos + 2];
	}

	//kriging parameters
	krg_c = computeCov(vpoints_displacement) / 400;
	krg_a = (float)(sqrt(8.0*8.0 + 8.0*8.0 + 9.0*9.0)*10.0);
	krg_nuggest = 0.0f;

	//init MatrixV
	initMatrixV();
	//inverse the MatrixV
	if (!invMatrixV())
	{
		printf("MatrixV is not invertablea!\n");
		return false;
	}
	// get the displacement of each point;
	Point3D displacement;
	dpos = 0;
	for (i = 1; i <= KrigingModel->numvertices; i++)
	{
		dpos = i * 3;
		displacement = KrigingIntepolate(i);
		model->vertices[dpos] = KrigingModel->vertices[dpos] + displacement.x;
		model->vertices[dpos + 1] = KrigingModel->vertices[dpos + 1] + displacement.y;
		model->vertices[dpos + 2] = KrigingModel->vertices[dpos + 2] + displacement.z;
	}
	return true;
}
int Kriging::getValidPoints(int size, cov_pts* points) // init the n_valid_points with npts;
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		//if (points[i].z>100 && points[i].z < 2000) // valid points
		count++;
	}
	this->valid_points = new cov_pts[count];
	count = 0;
	for (int i = 0; i < size; i++)
	{
		//if (points[i].z>100 && points[i].z < 2000) // valid points
		{
			valid_points[count] = points[i];
			count++;
		}
	}
	return count;
}
float Kriging::computeCov(Point3D* displacement)// number of the array is n_valid_points;
{
	int i;
	float average_x, average_y, average_z, s_x, s_y, s_z, s;

	average_x = average_y = average_z = 0.0f;
	for (i = 0; i< n_valid_points; i++)
	{
		average_x += displacement[i].x;
		average_y += displacement[i].y;
		average_z += displacement[i].z;
	}

	average_x = average_x / (float)n_valid_points;
	average_y = average_y / (float)n_valid_points;
	average_z = average_z / (float)n_valid_points;

	s_x = s_y = s_z = 0.0f;
	for (i = 0; i<n_valid_points; i++)
	{
		s_x += (displacement[i].x - average_x)*(displacement[i].x - average_x);//s_x+
		s_y += (displacement[i].y - average_y)*(displacement[i].y - average_y);//s_x+
		s_z += (displacement[i].z - average_z)*(displacement[i].z - average_z);
	}
	s = (s_x + s_y + s_z) / (float)(3 * (mat_dim - 1));

	return s;
}
bool Kriging::invMatrixV() // compute the invert matrix of Matrix V,return true if it is invertable else false
{
	int *is, *js, i, j, k, l, u, v;
	float d, p;
	int n = mat_dim;
	float* a = MatrixV;
	is = (int *)malloc(n*sizeof(int));
	js = (int *)malloc(n*sizeof(int));
	for (k = 0; k <= n - 1; k++){
		d = 0.0f;
		for (i = k; i <= n - 1; i++)
		for (j = k; j <= n - 1; j++){
			l = i*n + j; p = (float)fabs(a[l]);
			if (p>d){
				d = p; is[k] = i; js[k] = j;
			}
		}
		if (d + 1.0 == 1.0){
			free(is); free(js); //printf("err**not inv\n");
			return(FALSE);
		}
		if (is[k] != k)
		for (j = 0; j <= n - 1; j++){
			u = k*n + j; v = is[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		if (js[k] != k)
		for (i = 0; i <= n - 1; i++){
			u = i*n + k; v = i*n + js[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		l = k*n + k;
		a[l] = 1.0f / a[l];

		for (j = 0; j <= n - 1; j++)
		if (j != k){
			u = k*n + j; a[u] = a[u] * a[l];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k)
		for (j = 0; j <= n - 1; j++)
		if (j != k){
			u = i*n + j;
			a[u] = a[u] - a[i*n + k] * a[k*n + j];
		}
		for (i = 0; i <= n - 1; i++)
		if (i != k){
			u = i*n + k; a[u] = -a[u] * a[l];
		}
	}
	for (k = n - 1; k >= 0; k--){
		if (js[k] != k)
		for (j = 0; j <= n - 1; j++){
			u = k*n + j; v = js[k] * n + j;
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
		if (is[k] != k)
		for (i = 0; i <= n - 1; i++){
			u = i*n + k; v = i*n + is[k];
			p = a[u]; a[u] = a[v]; a[v] = p;
		}
	}
	free(is); free(js);
	return(TRUE);
}
void Kriging::initVecD(int nPos)// initailize the vector D by point index
{
	float C0, C1;
	int i;
	float h;

	C0 = 0.0f;
	C1 = krg_c - krg_nuggest;

	int pos = 3 * nPos;
	int valid_index = 0;
	for (i = 0; i<mat_dim - 1; i++)
	{
		valid_index = valid_points[i].index * 3;
		h = (float)sqrt(
			(KrigingModel->vertices[pos] - KrigingModel->vertices[valid_index])*(KrigingModel->vertices[pos] - KrigingModel->vertices[valid_index]) +
			(KrigingModel->vertices[pos + 1] - KrigingModel->vertices[valid_index + 1])*(KrigingModel->vertices[pos + 1] - KrigingModel->vertices[valid_index + 1]) +
			(KrigingModel->vertices[pos + 2] - KrigingModel->vertices[valid_index + 2])*(KrigingModel->vertices[pos + 2] - KrigingModel->vertices[valid_index + 2]));
		VectorD[i] = C0 + C1 * (1 - (float)exp(-3.0*h / krg_a));
	}
	VectorD[mat_dim - 1] = 1.0f;
}
Point3D Kriging::KrigingIntepolate(int nPos)// morph the model by Kriging Algorithm;
{
	float* fMatrixW;
	Point3D displacement;
	int i;

	fMatrixW = (float*)malloc(mat_dim*sizeof(float));

	initVecD(nPos);
	CalculateMatrixW(fMatrixW);
	displacement.x = 0;
	displacement.y = 0;
	displacement.z = 0;
	for (i = 0; i<n_valid_points; i++)
	{
		displacement.x += fMatrixW[i] * vpoints_displacement[i].x;
		displacement.y += fMatrixW[i] * vpoints_displacement[i].y;
		displacement.z += fMatrixW[i] * vpoints_displacement[i].z;
	}
	delete[]fMatrixW;
	return displacement;
}
void Kriging::CalculateMatrixW(float* W)
{
	int i, j;
	for (i = 0; i<mat_dim; i++)
	{
		W[i] = 0.0f;
		for (j = 0; j < mat_dim; j++)
		{
			W[i] += MatrixV[i*mat_dim + j] * VectorD[j];
		}
	}
}
