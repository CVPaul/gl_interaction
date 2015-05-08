#include "PointCloud.h"
#ifdef __unix__
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#elif defined(_WIN32)||defined(WIN32)
#include <GL\glut.h>
#include <stdio.h>
#include <stdlib.h>
#endif
PointCloud::~PointCloud()
{
	destroy();
}
void PointCloud::destroy()
{
	if (pc) delete[] pc;
	size = 0;
}
void PointCloud::load(std::string file)
{
	FILE* fp;
	fp = fopen(file.c_str(), "r");
	if (!fp)
	{
		printf("Error! in loadPointCloud():file open failed!\n");
		return;
	}
	int nsz;
	fscanf(fp, "%d\n", &nsz);
	if (nsz != size)
	{
		delete[] pc;
		size = nsz;
		pc = new Point3D[size];
	}
	if (!pc)
	{
		printf("Error! in load(): out if memory!\n");
		return;
	}
	int count = 0, valid_sz = 0;
	center.x = center.y = center.z = 0.0;
	while (fscanf(fp, "%f %f %f\n", &pc[count].x,
		&pc[count].y, &pc[count].z) != EOF)
	{
		if (pc[count].z > 0 /*100 && pc[count].z < 2000*/)
		{
			valid_sz++;
			center.x += pc[count].x;
			center.y += pc[count].y;
			center.z += pc[count].z;
		}
		count++;
	}
	if (valid_sz>0)
	{
		center.x /= valid_sz;
		center.y /= valid_sz;
		center.z /= valid_sz;
	}
	if (size != count)
	{
		printf("Error! in loadPointCloud():sizes are not equal:size=%d,count=%d!\n", size, count);
		delete[] pc; pc = NULL;
		size = 0; count = 0;
		fclose(fp);
		return;
	}
	fclose(fp);
}
void PointCloud::save(std::string file)
{
	FILE *fp;
	fp = fopen(file.c_str(), "w");
	if (!fp)
	{
		printf("Error! in savePointCloud():file open failed!\n");
		return;
	}
	fprintf(fp, "%d\n", size);
	for (int k = 0; k < size; k++)
	{
		fprintf(fp, "%f %f %f\n", pc[k].x, pc[k].y, pc[k].z);
	}
	fclose(fp);
}
void PointCloud::create(int sz)
{
	if (size != sz)
	{
		delete[] pc;
		size = sz;
		pc = new Point3D[size];
	}
	if (!pc)
	{
		size = 0;
		pc = NULL;
	}
}
void PointCloud::getFromVec(std::vector<Point3D> vec)
{
	if (size> 0)
	{
		delete[] pc;
		size = 0;
	}
	create(vec.size());
	for (int k = 0; k < size; k++)
	{
		pc[k] = vec[k];
	}
}
void PointCloud::move(Point3D trans)
{
	for (int k = 0; k < size; k++)
	{
		pc[k].x -= trans.x;
		pc[k].y -= trans.y;
		pc[k].z -= trans.z;
	}
}
void PointCloud::draw(float r, float g, float b)
{
	GLdouble obj[3];
	glBegin(GL_POINTS);
	for (int pos = 0; pos < size; pos++)
	{
		obj[0] = pc[pos].x;// -CameraVec[0];
		obj[1] = pc[pos].y;// -CameraVec[1];
		obj[2] = pc[pos].z;// -CameraVec[2];

		if (obj[2]>0)
		{
			glColor3f(r, g, b); glVertex3f(-obj[0], obj[1], obj[2]);
		}
	}
	glEnd();
}
void PointCloud::copyTo(PointCloud& npc)
{
	npc.create(this->size);
	for (int k = 0; k < npc.size; k++)
	{
		npc.pc[k] = this->pc[k];
	}
	npc.center = this->center;
}
void PointCloud::resize(int sz)
{
	if (sz <= 0)
	{
		printf("in PointCloud::resize(),size must >0 ,nothing done and return!\n");
		return;
	}
	if (sz <= size) // no operation for the reduant elements
	{
		size = sz;
		return;
	}
	PointCloud temp;
	this->copyTo(temp);
	this->create(sz);
	for (int k = 0; k < temp.size; k++)// copy the original data to it
	{
		this->pc[k] = temp.pc[k];
	}
}
