#include <stdio.h>
#include "PointCloud.h"
#include "loadFaceModel.h"
#include "TransformPoints.h"
//---------------------------------------------------------------------
LOADER::LOADER()
{
	//init at here
	// face Eigen-PCA model
	nVertex = 0;
	mShape = NULL;

	nEigen = 0;
	EigenVals = NULL;

	rows = 0;
	cols = 0;
	EigenFaces = NULL;

	//Triangle map
	nTrigles = 0;
	triRows = 0;
	triCols = 0;
	Trigles = NULL;
}
LOADER::~LOADER()
{
	// destroy at here
	nVertex = 0;
	if (mShape != NULL)
	{
		delete[] mShape;
		mShape = NULL;
	}

	nEigen = 0;
	if (EigenVals != NULL)
	{
		delete[] EigenVals;
		EigenVals = NULL;
	}

	rows = 0;
	cols = 0;
	if (EigenFaces != NULL)
	{
		delete[] EigenFaces;
		EigenFaces = NULL;
	}

	//if (ToTexture != NULL)
	//{
	//	delete[] ToTexture;
	//}

	////Texture Map model
	//nTxtPoints = 0;
	//if (TxtMapx != NULL)
	//{
	//	delete[] TxtMapx;
	//	TxtMapx = NULL;
	//}
	//if (TxtMapy != NULL)
	//{
	//	delete[] TxtMapy;
	//	TxtMapy = NULL;
	//}
	//Triangle map
	nTrigles = 0;
	triRows = 0;
	triCols = 0;
	if (Trigles != NULL)
	{
		delete[] Trigles;
		Trigles = NULL;
	}
}
void LOADER::loadFloatMat(FILE* fp, float*& fmat, int& rows, int& cols)
{
	fscanf(fp, "%d %d", &rows, &cols);
	if (fmat != NULL)
	{
		delete[]fmat;
		fmat = NULL;
	}
	fmat = new float[rows*cols];
	if (fmat == NULL)
	{
		printf("Error in LOADER::loadFlotMat(),memory allocation failed!\n");
		return;
	}
	fread(fmat, rows*cols, sizeof(float), fp);
}
void LOADER::loadFloatVec(FILE* fp, float*& fvec, int& len)
{
	fscanf(fp, "%d", &len);
	if (fvec != NULL)
	{
		delete[]fvec;
		fvec = NULL;
	}
	fvec = new float[len];
	if (fvec == NULL)
	{
		printf("Error in LOADER::loadFlotVec(),memory allocation failed!\n");
		return;
	}
	fread(fvec, len, sizeof(float), fp);
}
/*
@ conver the face model to GLMmodel format for convinient show
*/
GLMmodel* LOADER::cvtToGLM(int index)
{
	if (index < 0)
	{
		printf("Error! index should be smaller than %d,and no less than 0,please check it!", nEigen);
		return NULL;
	}
	GLMmodel* model;
	model = (GLMmodel*)malloc(sizeof(GLMmodel));
	model->pathname = _strdup("");
	model->mtllibname = NULL;
	model->numvertices = nVertex;
	model->vertices = NULL;
	model->numnormals = 0;
	model->normals = NULL;
	model->numtexcoords = 0;
	model->texcoords = NULL;
	model->numfacetnorms = 0;
	model->facetnorms = NULL;
	model->numtriangles = nTrigles;
	model->triangles = NULL;
	model->nummaterials = 0;
	model->materials = NULL;
	model->numgroups = 1;
	model->groups = NULL;
	model->position[0] = 0.0;
	model->position[1] = 0.0;
	model->position[2] = 0.0;

	/* allocate memory */
	model->vertices = (GLfloat*)malloc(sizeof(GLfloat)*
		3 * (model->numvertices + 1));
	model->triangles = (GLMtriangle*)malloc(sizeof(GLMtriangle)*
		model->numtriangles);
	if (model->numnormals) {
		model->normals = (GLfloat*)malloc(sizeof(GLfloat)*
			3 * (model->numnormals + 1));
	}
	if (model->numtexcoords) {
		model->texcoords = (GLfloat*)malloc(sizeof(GLfloat)*
			2 * (model->numtexcoords + 1));
	}
	int pos = 0;
	for (int k = 1; k <= model->numvertices; k++)
	{
		pos = 3 * k;
		model->vertices[pos] = mShape[pos - 3];
		model->vertices[pos + 1] = mShape[pos - 2];
		model->vertices[pos + 2] = mShape[pos - 1];
	}
	for (int i = 0; i <model->numtriangles; i++)
	{
		pos = i * 3;
		model->triangles[i].vindices[0] = Trigles[pos]+1;
		model->triangles[i].vindices[1] = Trigles[pos+1]+1;
		model->triangles[i].vindices[2] = Trigles[pos+2]+1;
	}
	model->groups = (GLMgroup*) malloc(sizeof(GLMgroup)); model->groups->next = NULL;// only one group
	model->numgroups = 1;
	model->groups->name = new char[10];// length("default")=7
	memcpy(model->groups->name, "default", 7*sizeof(char));
	model->groups->numtriangles = model->numtriangles;
	model->groups->triangles = (GLuint*)malloc(sizeof(GLuint)*(model->groups->numtriangles));
	for (int k = 0; k < model->numtriangles; k++)
		model->groups->triangles[k] = k;
	if (index>0)
	{
		int base = (index - 1)*this->cols;
		for (int k = 1; k <= model->numvertices; k++)
		{
			pos = 3 * k;
			model->vertices[pos] += EigenFaces[base + pos - 3];
			model->vertices[pos + 1] += EigenFaces[base + pos - 2];
			model->vertices[pos + 2] += EigenFaces[base + pos - 1];
		}
	}
	return model;
}
void LOADER::writeFloatMat(FILE* fp, const float* fmat, int frows, int fcols)
{
	fprintf(fp, "%d %d", frows, fcols);
	if (fmat == NULL || frows == 0 || fcols == 0)
	{
		printf("Error in LOADER::writeFlotMat()\n");
		return;
	}
	fwrite(fmat, frows*fcols, sizeof(float), fp);
}
void LOADER::writeFloatVec(FILE* fp, const float* fvec, int len)
{
	fprintf(fp, "%d", len);
	if (fvec == NULL || len == 0)
	{
		printf("Error in LOADER::writeFlotVec()\n");
		return;
	}
	fwrite(fvec, len, sizeof(float), fp);
}
void LOADER::loadSample3DModel(const char* modelf)
{
	FILE *fp;
	fp = fopen(modelf, "rb");
	//----------------------------Face Shape Model part (start)-----------------
	// avrg face
	fscanf(fp, "%d ", &nVertex);
	int mRows, mCols;
	loadFloatMat(fp, mShape, mRows, mCols);

	// Eigen values
	//Read the number of the eigen values retained
	fscanf(fp, "%d ", &nEigen);
	int Elen;
	loadFloatVec(fp, EigenVals, Elen);

	//Eigen faces
	loadFloatMat(fp, EigenFaces, rows, cols);
	//----------------------------Face Shape Model part (end)--------------------
	/// Triangle model
	fscanf(fp, "%d", &nTrigles);
	loadFloatMat(fp, Trigles, triRows, triCols);
	//---------------------------------The End------------------------------------
	fclose(fp);
}
void LOADER::saveSample3DModel(const char* modelf)
{
	FILE *fp;
	fp = fopen(modelf, "wb");
	//----------------------------Face Shape Model part (start)-----------------
	// avrg face
	fprintf(fp, "%d ", nVertex);
	writeFloatMat(fp, mShape, nVertex, 3);

	// Eigen values
	//Read the number of the eigen values retained
	fprintf(fp, "%d ", nEigen);
	writeFloatVec(fp, EigenVals, nEigen);

	//Eigen faces
	writeFloatMat(fp, EigenFaces, rows, cols);
	//----------------------------Face Shape Model part (end)--------------------
	/// Triangle model
	fprintf(fp, "%d ", nTrigles);
	writeFloatMat(fp, Trigles, triRows, triCols);
	//---------------------------------The End------------------------------------
	fclose(fp);
}