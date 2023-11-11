#pragma once
#include "Vector3D.h"

// A class for performing basic operations with homogeneous points in 3D space.
// Feel free to edit/extend!
class Point3D
{
public:
	Point3D(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f) : x(x_), y(y_), z(z_), w(1.0f) {}

	// Components of the point (distance along each axis)
	float x, y, z, w;

	// Returns the point as a vector displacement from the origin
	Vector3D asVector() const
	{
		return Vector3D(x, y, z);
	}

	// Returns the point at the given vector displacement from this point
	Point3D operator+(const Vector3D& vec) const
	{
		return Point3D(x + vec.x, y + vec.y, z + vec.z);
	}

	// Returns the vector difference between two points
	Vector3D operator-(const Point3D& other) const
	{
		return Vector3D(x - other.x, y - other.y, z - other.z);
	}
};