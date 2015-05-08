#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <string>
#include <vector>

#include "TransformPoints.h"

struct PointCloud // using struct define a 'public class' for convenience
{
public:// operation
	PointCloud() 
	{
		size = 0;
		pc = NULL;
		center.x = 0.0;
		center.y = 0.0;
		center.z = 0.0;
	}
	~PointCloud();
	/**
	* @ load pointclouds from the file
	*/
	void load(std::string file);

	/**
	* @ save pointclouds to the file
	*/
	void save(std::string file);

	/**
	* @ create PointCloud container with size=sz 
	*/
	void create(int sz);

	/**
	* @ resize the PointCloud container with size=sz(with data remain)
	*/
	void resize(int sz);

	/**
	* @ trans form the data in a vector to a PointCloud conatainer
	*/
	void getFromVec(std::vector<Point3D> vec);

	/**
	* @ since move is a simple but mostly using operation so here implement it again,
	* for more trasformation just use the Transform class
	*/
	void move(Point3D trans);

	/**
	* @ draw the point cloud with opengl with color(r,g,b)
	*/
	void draw(float r = 1.0, float g = 1.0, float b = 1.0);

	/** 
	* @copy the PointCloud to a new instance
	*/
	void copyTo(PointCloud& npc);

	/**
	* @destroy the container
	*/
	void destroy();

public: // attributes
	Point3D* pc;
	Point3D center;
	int size;
};

#endif /*PointCloud.h*/
