#ifndef LOAD3DMODEL_H
#define LOAD3DMODEL_H
/*@ brief: this class is designed as model loader (from "data/Sample3DModel.txt" 
@ witch contains a mean face shape model a 50 Engine faces, 
@ note: note texture is support now
*/
class LOADER
{
public:
	LOADER();
	~LOADER();
	void loadSample3DModel(const char* modelf);
	void saveSample3DModel(const char* modelf);
	GLMmodel* cvtToGLM(int index);
public:
	// m_avgShape:
	int nVertex;
	float* mShape;

	// Eigen value
	int nEigen;
	float* EigenVals;

	// Eigen faces
	int rows, cols;
	float* EigenFaces;

	int nTrigles;
	int triRows, triCols;
	float* Trigles;

private:
	/*
	@ brief: all the data are save as Matrix with size rows*cols
	*/
	void loadFloatMat(FILE* fp, float*& fmat, int& rows, int& cols);
	/*
	@ brief: though vector can be seem as matrix,we'd like to write another interface for convenient
	*/
	void loadFloatVec(FILE* fp, float*& fvec, int& len);
	/*
	@ brief: save matrix data to file
	*/
	void writeFloatMat(FILE* fp, const float* fmat, int frows, int fcols);
	/*
	@ brief: save vector data to file
	*/
	void writeFloatVec(FILE* fp, const float* fvec, int len);
};

#endif /*LOAD3DMODEL_H*/
