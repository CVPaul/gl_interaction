#ifndef GL_OPERATION_H
#define GL_OPERATION_H

#include "gl_animation.h"
#include "TranformPoints.h"
#include "global_values.h"
#include "PointCloud.h"
#include "glm.h"

extern float leye_mv_vec[3];
extern float reye_mv_vec[3];
extern float face_mv_vec[3];

/*this is a specify function which only used for this model, the frame work is general!*/
/*
@ brief: get the angle and normal betwwen two vectors(form (p2-p1) to (p3-p1)) 
*/
float get_n(const PXCPoint3DF32 p1, const PXCPoint3DF32 p2, const PXCPoint3DF32 p3, PXCPoint3DF32& n);

void gl_getTransdPt(PXCPoint3DF32* pt, PXCPoint3DF32* new_pt,int size=2); // for display_CGimg()
void gl_getTransdPt(PXCPoint3DF32 new_pt[2]); // for display_merge()

void gl_cal_RAngles(PXCPoint3DF32* orig_direct, PXCPoint3DF32* now_direct, 
	PXCPoint3DF32* n_direct, float* angles, int size = 2);
/*
@ brief: convert wolrd to appointed coords which paralle with head coords(for example coords at eye)
@ input coords of the point is denate by coords in camera coords( world coords)
*/
void convertCoords(float TransV[3], float RotateV[3], float MoveV[3],
	int size, PXCPoint3DF32* worlds, PXCPoint3DF32* heads);
/*
@ brief: convert camera coordds to head coords
*/
void world2head(int size, PXCPoint3DF32*world, PXCPoint3DF32* head);
/*
@ brief: the invert process of world2head
*/
void head2world(int size, PXCPoint3DF32* head, PXCPoint3DF32* world);
/*
@ brief: convert landmarks on face to the preporty format  for test
*/
void convertLansmarks(PXCPoint3DF32 world_pt[LANDMARKS], PXCPointF32 image_pt[LANDMARKS]);
/*
@ brief: draw to Image to show
*/
void draw2image(int size, PXCPointF32* pts, cv::Mat& canvas,cv::Scalar color);
/*
@ brief: crop of the model image based on the model eye_maks
*/
void cropModel(PXCPoint3DF32 ledge[EYE_MARKS_COUNT], PXCPoint3DF32 redge[EYE_MARKS_COUNT], float scale);
/*
@ brief: merge to Image
*/
void mergeImage(cv::Mat& sence, cv::Mat back, cv::Mat mask);
/*
@brief: inverse the model by z-axis
*/
void invModel(GLMmodel*& model);
/*
@ brief: update the texture at run time
*/
void runTimeTexture(GLMmodel*& model, float align_trans[3], float align_angle, PXCPoint3DF32 rotAxis,
	float align_move[3], cv::Mat texture, GLuint& TID);
/*
@ brief: update the texture ar run time
*/
void runTimeTexture(GLMmodel*& model, int viewport[4], double model_mat[16],double proj_mat[16], cv::Mat texture, GLuint& TID);
/*
@ brief: this is a specify function for runtimeTexture,specially for simulate model transformation in opengl
*/
void sim_gl_transform(Transform& Transf, float align_Trans[3], float align_angle, PXCPoint3DF32 rotAxis, float align_move[3]);
/*
@ brief: transform ladmarks point to PointCloud
*/
void landmark2PointCloud(int size, PXCFaceData::LandmarkPoint *landmarks, PointCloud& npc);
#endif/*gl_operation.h*/