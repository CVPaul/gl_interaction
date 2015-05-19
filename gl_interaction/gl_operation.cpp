#include "gl_operation.h"
#include "TranformPoints.h"
#include "genTexture.h"
#include "gl_mouse.h"

float leye_mv_vec[3] = { -42.339, 34.157, 513.486};
float reye_mv_vec[3] = { 19.631, 34.186, 511.093};
float face_mv_vec[3] = { -8.547, 4.882, 548.478 };

float get_n(const PXCPoint3DF32 p1, const PXCPoint3DF32 p2, const PXCPoint3DF32 p3, PXCPoint3DF32& n)
{
	PXCPoint3DF32 a, b;
	a.x = p2.x - p1.x; a.y = p2.y - p1.y; a.z = p2.z - p1.z;
	b.x = p3.x - p1.x; b.y = p3.y - p1.y; b.z = p3.z - p1.z;

	double ra = a.x*a.x + a.y*a.y + a.z*a.z;
	double rb = b.x*b.x + b.y*b.y + b.z*b.z;
	ra = sqrt(ra);
	rb = sqrt(rb);
	a.x /= ra; a.y /= ra; a.z /= ra;
	b.x /= rb; b.y /= rb; b.z /= rb;

	n.x = a.y*b.z - a.z*b.y;
	n.y = a.z*b.x - a.x*b.z;
	n.z = a.x*b.y - a.y*b.x;

	float angle = a.x*b.x+a.y*b.y+a.z*b.z;
	return angle = acos(angle)*180/CV_PI;
}
void gl_getTransdPt(PXCPoint3DF32 new_pt[2]) // for display_merge()
{
	new_pt[0].x = (point_ref[76].world.x*MM_PER_M + CameraVec[0]);
	new_pt[0].y = (point_ref[76].world.y*MM_PER_M - CameraVec[1]);
	new_pt[0].z = (point_ref[76].world.z*MM_PER_M - CameraVec[2]);

	new_pt[1].x = (point_ref[77].world.x*MM_PER_M + CameraVec[0]);
	new_pt[1].y = (point_ref[77].world.y*MM_PER_M - CameraVec[1]);
	new_pt[1].z = (point_ref[77].world.z*MM_PER_M - CameraVec[2]);
}
void gl_getTransdPt(PXCPoint3DF32* pt, PXCPoint3DF32* new_pt,int size) // for display_CGimg
{
	Transform Translater;
	// processing........
	if (size != 2)
	{
		printf("Error! this is only for two point of the eye centers!\n");
		return;
	}
	// move to head coords
	pt[0].x = pt[0].x + leye_mv_vec[0] - face_mv_vec[0]; // left_eye
	pt[0].y = pt[0].y + leye_mv_vec[1] - face_mv_vec[1];
	pt[0].z = pt[0].z + leye_mv_vec[2] - face_mv_vec[2];

	pt[1].x = pt[1].x + reye_mv_vec[0] - face_mv_vec[0]; // right_eye
	pt[1].y = pt[1].y + reye_mv_vec[1] - face_mv_vec[1];
	pt[1].z = pt[1].z + reye_mv_vec[2] - face_mv_vec[2];
	
	float nTrans_vec[3] = { 
		Trans[0] + face_mv_vec[0],
		Trans[1] + face_mv_vec[1],
		Trans[2] + face_mv_vec[2] 
	};
	float Move_vec[3] = { 0, 0, 0 };

	Translater.TranformInOnce(size, nTrans_vec, Rotate, Move_vec, pt, new_pt);
}
void gl_cal_RAngles(PXCPoint3DF32* orig_direct, PXCPoint3DF32* now_direct,
	PXCPoint3DF32* n_direct, float* angles, int size)
{
	PXCPoint3DF32 Original = { 0, 0, 0 };
	for (int k = 0; k < size; k++)
	{
		angles[k]=get_n(Original, orig_direct[k], now_direct[k], n_direct[k]);
	}
}
void world2head(int size,PXCPoint3DF32*world, PXCPoint3DF32* head)
{
	float TransV[3] = { 0, 0, 0 };
	float RotateV[3] = {
		-outAngle.pitch,
		outAngle.yaw,
		outAngle.roll
	};
	float MoveV[3] = {
		-hpose.headCenter.x,
		-hpose.headCenter.y,
		-hpose.headCenter.z,
	};
	convertCoords(TransV, RotateV, MoveV, size, world, head);
}
void head2world(int size, PXCPoint3DF32* head, PXCPoint3DF32* world)
{
	float TransV[3] = {
		hpose.headCenter.x,
		hpose.headCenter.y,
		hpose.headCenter.z
	};
	float RotateV[3] = {
		outAngle.pitch,
		-outAngle.yaw,
		-outAngle.roll
	};
	float MoveV[3] = { 0, 0, 0 };
	convertCoords(TransV, RotateV, MoveV, size, world, head);
}
void convertCoords(float TransV[3], float RotateV[3], float MoveV[3],
	int size, PXCPoint3DF32* worlds, PXCPoint3DF32* heads)
{
	Transform trans;
	trans.TranformInOnce(size, TransV, RotateV, MoveV, worlds, heads);
}
void convertLansmarks(PXCPoint3DF32 world_pt[LANDMARKS], PXCPointF32 image_pt[LANDMARKS])
{
	for (int k = 0; k < LANDMARKS; k++)
	{
		if (point_ref[k].confidenceImage)
		{
			image_pt[k] = { point_ref[k].image.x, point_ref[k].image.y };
		}
		else
		{
			image_pt[k] = {0,0};
		}
		if (point_ref[k].confidenceWorld)
		{
			world_pt[k] = { point_ref[k].world.x*MM_PER_M, point_ref[k].world.y*MM_PER_M, point_ref[k].world.z*MM_PER_M };
		}
		else
		{
			world_pt[k] = { 0, 0, 0 };
		}
	}
}
void draw2image(int size, PXCPointF32* pts, cv::Mat& canvas,cv::Scalar color)
{
	for (int k = 0; k < size; k++)
	{
		cv::circle(canvas, cv::Point2f(pts[k].x, pts[k].y), 2, color, 1);
	}
}
void cropModel(PXCPoint3DF32 ledge[EYE_MARKS_COUNT],PXCPoint3DF32 redge[EYE_MARKS_COUNT], float scale)
{
	PXCPoint3DF32 center;
	// left eye region:
	center = { 0, 0, 0 };
	for (int k = 0; k < EYE_MARKS_COUNT; k++)
	{
		center.x += eye_mks.left[k].x;
		center.y += eye_mks.left[k].y;
		center.z += eye_mks.left[k].z;
	}
	center.x /= EYE_MARKS_COUNT;
	center.y /= EYE_MARKS_COUNT;
	center.z /= EYE_MARKS_COUNT;

	//PXCPoint3DF32 direct = { 0, 0, 0 };
	for (int k = 0; k < EYE_MARKS_COUNT; k++)
	{
		ledge[k].x = center.x + (eye_mks.left[k].x - center.x)*scale;
		ledge[k].y = center.y + (eye_mks.left[k].y - center.y)*scale;
		ledge[k].z = center.z + (eye_mks.left[k].z - center.z)*scale;
	}
	// right eye region:
	center = { 0, 0, 0 };
	for (int k = 0; k < EYE_MARKS_COUNT; k++)
	{
		center.x += eye_mks.right[k].x;
		center.y += eye_mks.right[k].y;
		center.z += eye_mks.right[k].z;
	}
	center.x /= EYE_MARKS_COUNT;
	center.y /= EYE_MARKS_COUNT;
	center.z /= EYE_MARKS_COUNT;

	//direct = { 0, 0, 0 };
	for (int k = 0; k < EYE_MARKS_COUNT; k++)
	{
		redge[k].x = center.x + (eye_mks.right[k].x - center.x)*scale;
		redge[k].y = center.y + (eye_mks.right[k].y - center.y)*scale;
		redge[k].z = center.z + (eye_mks.right[k].z - center.z)*scale;
	}
}
void runTimeTexture(GLMmodel*& model, float align_trans[3], float align_angle,PXCPoint3DF32 rotAxis,
	float align_move[3], cv::Mat texture, GLuint& TID)
{
	//Part 01: replace the texture
	if (TID)
	{
		glDeleteTextures(1, &TID);
		TID = 0;
	}
	bool suc = loadTexture(texture, TID);
	if (!TID)
	{
		printf("Error!generate texture failed(in runTimeTexture())!\n");
		return;
	}
	//Part 02: compute the texcoords
	PXCPoint3DF32 model_pts;
	PXCPointF32 pts_coords;
	int pos = 0, cpos;

	Transform transf;
	sim_gl_transform(transf, align_trans, align_angle, rotAxis, align_move);


	for (int i = 0; i < model->numvertices; i++)
	{
		pos = 3 * (i + 1); // caution:index of the model start from 1
		cpos = 2 * (i + 1);// color position
		model_pts.x = model->vertices[pos];
		model_pts.y = model->vertices[pos + 1];
		model_pts.z = model->vertices[pos + 2];
		transf.TranformInOnce(1, &model_pts, &model_pts);
		projection->ProjectCameraToColor(1, &model_pts, &pts_coords);
		if (pts_coords.x < 0)pts_coords.x = 0;
		if (pts_coords.x >= C_COLS) pts_coords.x = C_COLS - 1;

		if (pts_coords.y < 0)pts_coords.x = 0;
		if (pts_coords.y >= C_ROWS) pts_coords.x = C_ROWS - 1;

		model->texcoords[cpos] = pts_coords.x/C_COLS;
		model->texcoords[cpos+1] = (C_ROWS-pts_coords.y) / C_ROWS;
	}
}
void runTimeTexture(GLMmodel*& model, int viewport[4], double model_mat[16], double proj_mat[16], cv::Mat texture, GLuint& TID)
{
	//Part 01: replace the texture
	if (TID)
	{
		glDeleteTextures(1, &TID);
		TID = 0;
	}
	bool suc = loadTexture(texture, TID);
	if (!TID)
	{
		printf("Error!generate texture failed(in runTimeTexture())!\n");
		return;
	}
	//Part 02: compute the texcoords
	PXCPoint3DF32 model_pts;
	PXCPointF32 pts_coords;
	int pos = 0, cpos;

	double winx, winy, winz;
	for (int i = 0; i < model->numvertices; i++)
	{
		pos = 3 * (i + 1); // caution:index of the model start from 1
		cpos = 2 * (i + 1);// color position
		model_pts.x = model->vertices[pos];
		model_pts.y = model->vertices[pos + 1];
		model_pts.z = model->vertices[pos + 2];
		
		gluProject(model_pts.x, model_pts.y, model_pts.z, model_mat, proj_mat, viewport, &winx, &winy, &winz);
		pts_coords.x = winx;
		pts_coords.y = winy;
		if (pts_coords.x < 0)pts_coords.x = 0;
		if (pts_coords.x >= C_COLS) pts_coords.x = C_COLS - 1;

		if (pts_coords.y < 0)pts_coords.x = 0;
		if (pts_coords.y >= C_ROWS) pts_coords.x = C_ROWS - 1;

		model->texcoords[cpos] = pts_coords.x / C_COLS;
		model->texcoords[cpos + 1] = (C_ROWS - pts_coords.y) / C_ROWS;
	}
}
void invModel(GLMmodel*& model)
{
	if (!model) return;
	int pos = 0;
	for (int k = 1; k <= model->numvertices; k++)
	{
		pos = 3 * k;
		model->vertices[pos + 2]=-model->vertices[pos+2]; // inverse the z-axis
	}
}
void sim_gl_transform(Transform& Transf,float align_Trans[3],float align_angle,PXCPoint3DF32 rotAxis, float align_move[3])
{
	Transf.setIdentity();// init the rote matrix;
	float Move2Orig[3];
	Move2Orig[0] = -face_mv_vec[0];
	Move2Orig[1] = -face_mv_vec[1];
	Move2Orig[2] = -face_mv_vec[2];
	// set the transform matrix
	Transf.TranslatePoint(align_Trans);
	Transf.roteByAxis(align_angle, rotAxis.x, rotAxis.y, rotAxis.z);
	Transf.TranslatePoint(align_move);

	Transf.roteByAxis(Rotate[0], 1.0, 0.0, 0.0);
	Transf.roteByAxis(Rotate[1], 0.0, 1.0, 0.0);
	Transf.roteByAxis(Rotate[2], 0.0, 0.0, 1.0);
	Transf.TranslatePoint(Move2Orig);
}
void mergeImage(cv::Mat& sence, cv::Mat back, cv::Mat mask)
{
	if (sence.size() != back.size() || sence.size() != mask.size())
	{
		printf("Error in mergeImage(),size of the the three mat should be the same!\n");
		return;
	}
	uchar *pSen, *pbk, *pMsk;
	int pos = 0;
	for (int i = 0; i < sence.rows; i++)
	{
		pSen = sence.ptr<uchar>(i);
		pbk = back.ptr<uchar>(i);
		pMsk = mask.ptr<uchar>(i);

		for (int j = 0; j < sence.cols; j++)
		{
			if (pMsk[j] < 100)
				continue;
			pos = 3 * j;
			pSen[pos] = pbk[pos];
			pSen[pos + 1] = pbk[pos + 1];
			pSen[pos + 2] = pbk[pos + 2];
		}
	}
}
void landmark2PointCloud(int size, PXCFaceData::LandmarkPoint *landmarks, PointCloud& npc)
{
	npc.create(size);
	for (int k = 0; k < size; k++)
	{
		if (landmarks[k].world.z*MM_PER_M < 10)
		{
			npc.pc[k].x = 0;
			npc.pc[k].y = 0;
			npc.pc[k].z = 0;
		}
		else
		{
			npc.pc[k].x = landmarks[k].world.x*MM_PER_M;
			npc.pc[k].y = landmarks[k].world.y*MM_PER_M;
			npc.pc[k].z = landmarks[k].world.z*MM_PER_M;
		}
	}
}