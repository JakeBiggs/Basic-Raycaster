#pragma once
#include "Matrix3D.h"
#include "PixelBuffer.h"
// Structure holding RGBA colour components
struct Colour
{
	unsigned char r, g, b, a;
	Colour(unsigned char r_ = 0, unsigned char g_ = 0, unsigned char b_ = 0) : r(r_), g(g_), b(b_), a(255) {}
};

// Base class for all objects in the scene.
class Object
{
public:
	Object(Point3D centrePoint = Point3D()) : m_centre(centrePoint){}
	virtual ~Object() {}

	// Returns true if the ray intersects with this object.
	// Params:
	//	raySrc					starting point of the ray (input)
	//	rayDir					direction of the ray (input)
	//	distToFirstIntersection	distance along the ray from the starting point of the first intersection with the object (output)
	virtual bool getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const = 0;
	Point3D intersectionPoint;

	// Transforms the object using the given matrix.
	virtual void applyTransformation(const Matrix3D& matrix) = 0;

	// Access the object's position
	const Point3D& position() const { return m_centre; }

	// Get the maximum distance of any of the object's vertices from its centre
	virtual float getMaxRadius() const = 0;
	


	// The object's RGBA colour
	Colour	m_colour = Colour(126, 126, 126);

	// The objects Albedo value (how much light is reflected/absorbed)
	Vector3D albedo;
	
	Vector3D hitNormal;

	// Flag indicating whether the object can move
	bool	m_isDynamic = false;

protected:

	Point3D m_centre;	// The coordinates of the object's centre in world space.
};

// A plane is a 2D surface defined by its normal and 'centre' point,
// plus (optionally) height and width limits (to form a rectangle).
class Plane : public Object
{
public:
	Plane(Point3D centrePoint = Point3D(),
		Vector3D n = Vector3D(0.0f, 1.0f, 0.0f),
		Vector3D up = Vector3D(0.0f, 0.0f, 1.0f),
		float w = 0.0f, float h = 0.0f);
	virtual ~Plane() {}

	virtual Vector3D calculateNormal() const { return m_normal; } //This is because the normal for a plane is given as just N.
	virtual float getDistToIntersection(const Point3D& raySrc, const Vector3D& rayDir) const;
	virtual bool getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const;
	virtual void applyTransformation(const Matrix3D& matrix);
	virtual float getMaxRadius() const { return m_halfDiagonal; }

private:
	// The plane's orientation is defined by its normal and the directions of its width and height in world space.
	Vector3D	m_normal = Vector3D(0.0f, 1.0f, 0.0f),
				m_widthDirection = Vector3D(1.0f, 0.0f, 0.0f),
				m_heightDirection = Vector3D(0.0f, 0.0f, 1.0f);

	// The plane's size is given by its dimensions along the width and height axes (if these are zero or less, the plane is infinite).
	float		m_halfWidth, m_halfHeight, m_halfDiagonal = 0.0f;
	bool		m_isBounded = false;	// True if the plane's width and height are not zero
};

// A sphere is defined by its centre and radius
class Sphere : public Object
{
public:
	Sphere(Point3D centrePoint = Point3D(), float r = 1.0f) : Object(centrePoint), m_radius(r), m_radius2(r * r) {}
	virtual ~Sphere() {}

	//Get the normal of a given object
	virtual Vector3D calculateNormal(Point3D& pointOnSurface) const;
	virtual float getDistToIntersection(const Point3D& raySrc, const Vector3D& rayDir) const;
	virtual bool getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const;
	virtual void applyTransformation(const Matrix3D& matrix);
	virtual float getMaxRadius() const { return m_radius; }


private:
	float	m_radius;	// The radius of the sphere
	float	m_radius2;	// The squared radius of the sphere
};


class Light : public Object
{
public:
	Light(Point3D centrePoint = Point3D(), float ambientIntensity=0.18f) : Object(centrePoint), ambientIntensity(ambientIntensity) {}
	virtual ~Light() {}
	float ambientIntensity;

	virtual bool getIntersection(const Point3D& raySrc, const Vector3D& rayDir, float& distToFirstIntersection) const;
	virtual void applyTransformation(const Matrix3D& matrix);
	virtual float getMaxRadius() const { return 0; }

};
