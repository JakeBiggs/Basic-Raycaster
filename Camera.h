#pragma once
#include "Matrix3D.h"
#include "PixelBuffer.h"
#include "Object.h"

class Camera
{
public:
	void init(const Point3D& pos);
	bool updatePixelBuffer(const std::vector<Object*>& objects);

	unsigned	getViewPlaneResolutionX() const { return m_viewPlane.resolutionX; }
	unsigned	getViewPlaneResolutionY() const { return m_viewPlane.resolutionY; }

	// Change the camera's world space position
	void	translateX(float x) { m_position.x += x; m_worldTransformChanged = true; }
	void	translateY(float y) { m_position.y += y; m_worldTransformChanged = true; }
	void	translateZ(float z) { m_position.z += z; m_worldTransformChanged = true; }

	// Change the camera's world space rotation
	void	rotateX(float x) { m_rotation.x += x; m_worldTransformChanged = true; }
	void	rotateY(float y) { m_rotation.y += y; m_worldTransformChanged = true; }
	void	rotateZ(float z) { m_rotation.z += z; m_worldTransformChanged = true; }

	// Change the distance from the camera to the view plane
	void	zoom(float d) { m_viewPlane.distance += d; m_viewPlane.distance = max(1.0f, m_viewPlane.distance); }

	//Gets Colour at current pixel
	Colour	getColourAtPixel(unsigned i, unsigned j, std::vector<Object*> m_objects);

	Vector3D getReflectionVector(Vector3D& U, Vector3D& N);

	//Handles Diffuse, Specular and Ambient Light calculations
	Colour Phong(const Object *object, Colour colour, Point3D raySrc, Vector3D rayDir, std::vector<Light*> lights);

	//Colour  calculateDiffuseColour(const Point3D& intersectionPoint, const Vector3D& surfaceNormal, const Light& light) const;
private:
	Vector3D	getRayDirectionThroughPixel(int i, int j);
	void		updateWorldTransform();

	Point3D worldToCameraSpace(Point3D p);
	
	Point3D		m_position = Point3D();			// The position (translation) of the camera in world space
	Vector3D	m_rotation = Vector3D();		// The Euler rotation of the camera in world space
	Matrix3D	m_cameraToWorldTransform;		// The matrix representing the camera transfrom in world space
	bool		m_worldTransformChanged = true;	// Flag indicating whether the camera's world transform has been updated
	
	// Properties describing the view plane (framing of the picture)
	struct
	{
		float distance = 2.5f;		// Distance of the view plane from the camera (along the normal/z-axis)
		float halfWidth = 3.0f;		// Half extent of the view plane along the x-axis
		float halfHeight = 3.0f;	// Half extent of the view plane along the y-axis
		unsigned resolutionX = 250, resolutionY = 250;	// The number of pixels in the x and y directions
	}	m_viewPlane;

	// Cached info for generating the image
	PixelBuffer m_pixelBuf;								// Stores information about the closest object to each pixel
	float m_pixelWidth = -1.0f, m_pixelHeight = -1.0f;	// Stores the dimensions of each pixel in camera space units
};