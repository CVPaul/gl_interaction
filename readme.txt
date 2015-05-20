This can be seem as a small tool kit of 3D model rendering which has been used in my Gaze Correction Project,in this repository following thigs are include:
1. Class_<Transform>:
==================================================================================================
 * class_<Transform> packagae the common operation for 3d points, at the beginning this class is desgined to deal
 * with the problem that getting the tranfomed points's coordinates in opengl
 * now more function are added to the class(details can be find in the Transform.h)
 * note: like pipeline in opengl all the operation change the data only when function  
                             TranformInOnce()( recommand )
                             or   rotatePoint()
 * is called!!!, other functions will change the trans_mat only 
===================================================================================================
2. file_<genTexture>
==================================================================================================
*functions in this file designed to load more than .bmp(for example: jpg,png and so on) format texture file in memory by opencv and transform it into opengl teture format for texture mapping(redenring)
==================================================================================================
3. file_<gl_basic and gl_commom>
==================================================================================================
*these files implement the basic function for user interaction ,for example:
	a) rotate a model
	b) move a modle
	c) grab pixels from the rendered window and save in OpenCV Mat format
	d) this also include some Demo display function call by main.cpp
4.Class_<PointCloud>
==================================================================================================
*a simple pointcloud load,save and draw class, it is easy to use
==================================================================================================
5.Class_<ICPAlgorithm>
=================================================================================================
*the famous ICP(Iterative Closest Point) Algorithm and a demo is include. More details is record in  the .h file of it
=================================================================================================
6.Class_<KrigingAlgorithm>
=================================================================================================
Kriging interpolate Algorithm and also a demo is include
=================================================================================================
7. loadFaceMode(Class_<LOADER>)
=================================================================================================
/*@ brief: this class is designed as model loader (from "data/Sample3DModel.txt" 
@ witch contains a mean face shape model a 50 Engine faces, 
@ note: note texture is support now
*/
=================================================================================================
8. file_<gl_operation>
=================================================================================================
not called in any demo but include some useful functions, for example, get_n() will compute the 
cross product of two 3d vectors.
=================================================================================================
Requirement
OPenCV and OPenGL are reqiured
Compilation：
in linux(ubuntu) ./makeit
There is some way to use CMake on Windows but I am not familiar with it if not work, a VS2013 project is put in the windows direcory
Demo:
uncomment the coorespoing lines in the main.cpp to have a look at the demo
