#include "stdafx.h"
#include "Object.h"

// Plane constructor. Params are:
//	centrePoint		The point on the plane from which the width and height limits are measured
//	n				The unit vector that is normal to the plane (in world space)
//	up				The unit vector along which the plane height is measured (in world space; should be orthogonal to the normal)
//	w, h			The width and height of the plane (zero/negative for an infinite plane)
Plane::Plane(Point3D centrePoint, Vector3D n, Vector3D up, float w, float h) :
	Object(centrePoint),
	m_heightDirection(up),
	m_normal(n),
	m_halfWidth(w / 2.0f),
	m_halfHeight(h / 2.0f)
{
	m_normal.normalise();
	m_isBounded = m_halfWidth > 0.0f && m_halfHeight > 0.0f;
	if (m_isBounded)
	{
		m_widthDirection = m_heightDirection.cross(m_normal);
		m_widthDirection.normalise();
		m_heightDirection.normalise();
		m_halfDiagonal = sqrt(m_halfWidth * m_halfWidth + m_halfHeight * m_halfHeight);
	}
}



//--------------------------------------------------------------------------------------------------------------------//

float Plane::getDistToIntersection(const Point3D& raySrc, const Vector3D& rayDir) const
{
	return ((m_centre - raySrc).dot(m_normal)) / rayDir.dot(m_normal);
}

// Returns true if the ray intersects with this plane.
// Params:
//	raySrc					source/starting point of the ray (input)
//	rayDir					direction of the ray (input)
//	distToFirstIntersection	distance along the ray from the starting point of the first intersection with the plane (output)
bool Plane::getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const
{
	/////
	// uses the equation: t = (n * (p - p2))/(n * v) to find the distance from the source to the point of intersection on the plane
	// t = distance to the intersection
	// p = point on the plane (m_centre)
	// p2 = source of the ray point (raySrc)
	// n = normal vector of the plane (.dot(m_normal))
	// v = ray direction vector (rayDir)
	distToFirstIntersection = ((m_centre - raySrc).dot(m_normal) / (rayDir.dot(m_normal)));
	
	// parametric equation of a line is p = p1 + tv
	// p: intersection point
	// p1: ray source (raySrc)
	// t: distance to intersection
	// v: ray direction vector (rayDir)
	Point3D intersectionPoint = raySrc + (distToFirstIntersection*rayDir);


	//This finds the vector from the centre of the plane to the intersection point
	//Allows us to check if the intersection point is within the bounds of the plane
	Vector3D centreToIntersection = intersectionPoint - m_centre;

	//Checks if dot product of centre to intersection vector and respective axis vector are within half of the bounds of the plane (i.e. within the plane as from centre)
	if (abs(centreToIntersection.dot(m_widthDirection)) < m_halfWidth && 
		abs(centreToIntersection.dot(m_heightDirection)) < m_halfHeight &&
		distToFirstIntersection > 0) { //Checks that plane is in front of camera not behind
		//hitNormal = Vector3D(m_normal);

		

		return true;
	}
	else {
		return false;
	}

}

//--------------------------------------------------------------------------------------------------------------------//

// Transforms the object using the given matrix.
void Plane::applyTransformation(const Matrix3D & matrix)
{
	m_centre = matrix * m_centre;
	m_heightDirection = matrix * m_heightDirection;
	m_widthDirection = matrix * m_widthDirection;
	m_normal = matrix * m_normal;
}

float Sphere::getDistToIntersection(const Point3D& raySrc, const Vector3D& rayDir) const 
{
	 
	Vector3D raySrcToCentre = m_centre - raySrc;
	float distToCentre = raySrcToCentre.dot(rayDir);
	float dist2 = raySrcToCentre.dot(raySrcToCentre) - (distToCentre * distToCentre);
	return distToCentre - sqrt(abs(m_radius2 - dist2));
}

// Returns true if the ray intersects with this sphere.
// Params:
//	raySrc					starting point of the ray (input)
//	rayDir					direction of the ray (input)
//	distToFirstIntersection	distance along the ray from the starting point of the first intersection with the sphere (output)
bool Sphere::getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const
{
	// Find the point on the ray closest to the sphere's centre
	Vector3D srcToCentre = m_centre - raySrc;
	float tc = srcToCentre.dot(rayDir);
	
	// Check whether the closest point is inside the sphere
	if (tc > 0.0f)
	{
		float distSq = srcToCentre.dot(srcToCentre) - tc * tc;
		if (distSq < m_radius2)
		{
			distToFirstIntersection = tc - sqrt(m_radius2 - distSq);
			return true;
		}
	}

	return false;
}

Vector3D Sphere::calculateNormal(Point3D& pointOnSurface) const {
	//The normal of a sphere is always the vector from the centre to the point on the surface of the sphere
	Vector3D normal = pointOnSurface - m_centre; 
	return normal;
}

// Transforms the object using the given matrix.
void Sphere::applyTransformation(const Matrix3D & matrix)
{
	m_centre = matrix * m_centre;
}

bool Light::getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const
{
	return false;
}

void Light::applyTransformation(const Matrix3D& matrix)
{
	m_centre = matrix * m_centre;

}

