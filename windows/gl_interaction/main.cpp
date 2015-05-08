#include "gl_common.h"
#include "ICPAlgorithm.h"
#include "loadFaceModel.h"
#include "KrigingAlgorithm.h"

using namespace GL;

int main(int argc, char** argv)
{
	std::string Texture = "data\\test.jpg";
	GL::model = glmReadOBJ("data\\trigle.obj"); // load a obj file with the glmReadOBJ()



	//-------------------------------------------Demo 01: basic function-------------------------------------------
	//// basic test keyboard and mouse------------------------------------------------------------------------------
	//GL::show(Texture, display); // using GL::spin() to move is automaticlly



	//--------------------------------Demo 02: Kriging Algorithm and Class Transform test---------------------------
	//// Kriging Algorithm and Class Transform test-----------------------------------------------------------------
	//cov_pts marks[2];
	//Transform trans; // declare the transform instance

	//float centriod[3], trans_vec[3]; // move the object to the center	
	//trans.glm_centroid(model, centriod);
	//trans_vec[0] = -centriod[0]; // subtract
	//trans_vec[1] = -centriod[1];
	//trans_vec[2] = -centriod[2];
	//trans.TranslatePoint(trans_vec);
	//trans.TranformInOnce(model);
	//// ----subsection test the kriging algorithm-------------------------------------------------------------------
	//marks[0] = { 1, 3.7967 - centriod[0]- 9.0, 13.1000-centriod[1], 48.1781-centriod[2] };// (index, new coords(x,y,z))
	//marks[1] = { 162, 12.3829-centriod[0], -9.6992-centriod[1] + 9.0, 50.1306-centriod[2] };

	//model_back = glmCopyOBJ(model);

	//Kriging_Alg.setModel(model);
	//Kriging_Alg.AdjustByKriging(model,marks,2);

	//trans.setIdentity();
	//trans.TranslatePoint(20, 0, 0);
	//trans.TranformInOnce(model);

	//trans.setIdentity();
	//trans.TranslatePoint(-20, 0, 0);
	//trans.TranformInOnce(model_back);
	//GL::show(Texture, display_kriging_demo);
	


	//--------------------------------Demo 03: ICP Algorithm and Class Transform test---------------------------
	// test the ICP (Iterative Closet Point) (OpenCV is required)
	std::string modelz = "data\\model_rand.txt";
	std::string dataz = "data\\data_rand.txt";
	ICP::demo(dataz,modelz);
	


	//--------------------------------Demo 04: render face model---------------------------
	//LOADER faceloader;
	//faceloader.loadSample3DModel("Sample3DModel.txt");
	//GL::model = faceloader.cvtToGLM(25);
	//glmFacetNormals(GL::model);
	//glmVertexNormals(GL::model, 90);
	//GL::show(Texture, display_face);
	return 0;
}