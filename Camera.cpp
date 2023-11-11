#include "stdafx.h"
#include "Camera.h"
#include "Object.h"

// Initialises the camera at the given position
void Camera::init(const Point3D& pos)
{
	m_position = pos;
	m_pixelBuf.init(m_viewPlane.resolutionX, m_viewPlane.resolutionY);

//--------------------------------------------------------------------------------------------------------------------//

	//Calculates and stores the size of each pixel (in screen units)
	m_pixelWidth = (m_viewPlane.halfWidth*2) / m_viewPlane.resolutionX; //This is the total viewplane width divided by total pixels in X axis
	m_pixelHeight = (m_viewPlane.halfHeight*2)/m_viewPlane.resolutionY; //This is the total viewplane height divided by total pixels in Y axis
//--------------------------------------------------------------------------------------------------------------------//   
}

// Cast rays through the view plane and set colours based on what they intersect with
bool Camera::updatePixelBuffer(const std::vector<Object*>& objects)
{
	if (m_pixelBuf.isInitialised())
	{
		m_pixelBuf.clear();

		// Make sure our cached values are up to date
		if (m_worldTransformChanged)
		{
			updateWorldTransform();
			m_worldTransformChanged = false;
		}

		// Transform the objects to the camera's coordinate system
		const Matrix3D worldToCameraTransform = m_cameraToWorldTransform.inverseTransform();
		for (auto obj : objects)
			obj->applyTransformation(worldToCameraTransform);

		// Fill the pixel buffer with pointers to the closest object for each pixel
		Point3D origin;
		Vector3D rayDir;
		float distToIntersection;
		for (const auto obj : objects)
		{
			// Find the pixel that's intersected by the line from the
			// camera to the object's centre
			Vector3D toCentre = obj->position().asVector();
			toCentre.normalise();
				
			// Centre line intersects the view plane when the z value
			// is the distance to the view plane
			const float t = m_viewPlane.distance / toCentre.z;
			const float viewPlaneX = toCentre.x * t + m_viewPlane.halfWidth,
					viewPlaneY = toCentre.y * t + m_viewPlane.halfHeight;
				
			// Find the pixel indices of the centre line intersection point
			const int pixelX = static_cast<int>(viewPlaneX / m_pixelWidth),
					pixelY = static_cast<int>(viewPlaneY / m_pixelHeight);

			// Find the largest range of pixels that the object might cover,
			// based on its maximum 'radius'.
			const float objectRad = fabsf(obj->getMaxRadius());
			const int pixelRadiusX = static_cast<int>(objectRad / m_pixelWidth) + 1,
					pixelRadiusY = static_cast<int>(objectRad / m_pixelHeight) + 1;
				
			// Find the range of pixels that might be covered by the object,
			// clamped to the view plane resolution
			const unsigned startX = max(pixelX - pixelRadiusX, 0), endX = min(pixelX + pixelRadiusX, m_viewPlane.resolutionX),
					startY = max(pixelY - pixelRadiusY, 0), endY = min(pixelY + pixelRadiusY, m_viewPlane.resolutionY);
			
			// For each of the pixels that might be covered by the object, find the direction
			// of the ray passing through it and test whether it intersects with the object
			for (unsigned i = startX; i < endX; ++i)
			{
				for (unsigned j = startY; j < endY; ++j)
				{
//--------------------------------------------------------------------------------------------------------------------//
					// TODO: if you want to pass through any extra information from the intersection test
					// for Task 4, this is the place to do so. 
					rayDir = getRayDirectionThroughPixel(i, j);

					// Perform the intersection test between the ray through this pixel and the object,
					// and check whether the intersection point is closer than that of previously tested objects
					if (obj->getIntersection(origin, rayDir, distToIntersection)	
						&& distToIntersection < m_pixelBuf.getObjectInfoForPixel(i, j).distanceToIntersection)
					{
						m_pixelBuf.setObjectInfoForPixel(i, j, ObjectInfo(obj, distToIntersection));
					}
//--------------------------------------------------------------------------------------------------------------------//
				}
			}
		}

		// Now put the objects back!
		for (auto obj : objects)
			obj->applyTransformation(m_cameraToWorldTransform);

		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------------------//

// Calculates the normalised direction in camera space of a ray from
// the camera through the view-plane pixel at index (i, j),
// where 0 <= i < m_viewPlane.resolutionX and 0 <= j < m_viewPlane.resolutionY.
Vector3D Camera::getRayDirectionThroughPixel(int i, int j)
{
	Vector3D rayDir;
	Vector3D worldPos(i, j, m_viewPlane.distance); //Vector3D Representing the point in space the ray is colliding with.
	

	rayDir = Vector3D(
		worldPos.x * m_pixelWidth - m_viewPlane.halfWidth, 
		worldPos.y * m_pixelHeight - m_viewPlane.halfHeight, 
		worldPos.z);
	rayDir.normalise();
		
	return rayDir;
}

// Computes the transformation of the camera in world space, which is also the
// transform that will take objects from camera to world coordinates
// and stores it in m_cameraToWorldTransform
void Camera::updateWorldTransform()
{
	// TODO: the following code creates a transform for a camera with translation only
	// plus a flip on the z-axis to change from right- to left-handed coordinates;
	// update it to handle a generic transformation including rotations.
	// The Euler angle values for rotations about each axis are stored in m_rotation and should be
	// included in the Matrix3D m_cameraToWorldTransform; you can access an element of the matrix
	// at row i and column j using the () operator for both get and set operations, e.g.
	// matrix(i, j) = value or value = matrix(i, j) will both work.

	/*m_cameraToWorldTransform(0, 3) = m_position.x;
	m_cameraToWorldTransform(1, 3) = m_position.y;
	m_cameraToWorldTransform(2, 3) = m_position.z;
	m_cameraToWorldTransform(2, 2) = -1.0f;	// scale of -1 on the z-axis*/

	//Initialising Matrices for X, Y and Z rotations
	Matrix3D xRotation = Matrix3D();
	Matrix3D yRotation = Matrix3D();
	Matrix3D zRotation = Matrix3D();

	//X Rotation Matrix
	/*
			[1 ,  0 ,  0  ]
	 Rx(0)= [0 ,cos0, sin0]
			[0 ,-sin0,cos0]
	*/
	//Setting X values in xRotation matrix to those in Rx 
	xRotation(0, 0) = 1;
	xRotation(1, 1) = cosf(m_rotation.x);
	xRotation(1, 2) = -sinf(m_rotation.x);
	xRotation(2, 1) = sinf(m_rotation.x);
	xRotation(2, 2) = cosf(m_rotation.x);


	//Y Rotation Matrix
	/*
			[cos0 ,0,-sin0]
	 Ry(0)= [0 ,   1,  0  ]
			[sin0, 0 ,cos0]
	*/
	yRotation(0, 0) = cosf(m_rotation.y);
	yRotation(0, 2) = -sinf(m_rotation.y);
	yRotation(1, 1) = 1;
	yRotation(0, 2) = sinf(m_rotation.y);
	yRotation(2, 2) = cosf(m_rotation.y);


	//Z Rotation Matrix
		/*
				[cos0,sin0, 0 ]
		 Rz(0)= [-sin,cos0, 0 ]
				[0 ,   0  , 1 ]
		*/
	zRotation(0, 0) = cosf(m_rotation.z);
	zRotation(0, 1) = sinf(m_rotation.z);
	zRotation(1, 0) = -sinf(m_rotation.z);
	zRotation(1, 1) = cosf(m_rotation.z);
	zRotation(2, 2) = 1;


	//Combines all rotations for a final rotation matrix for all axes
	Matrix3D xyzRotation = xRotation * yRotation * zRotation;

	//Translation matrix
	Matrix3D xyzTranslation = Matrix3D(); //Initialises translation matrix and fills with identity matrix (1 in diagonals top left to bottom right)

	//In 4x4 Matrices, right most columns store	translations along the X,Y and Z axes respectively:
	xyzTranslation(0, 3) = m_position.x; 
	xyzTranslation(1, 3) = m_position.y;
	xyzTranslation(2, 3) = m_position.z;

	//Element (2,2) Represents scaling factor along Z-axis. (Inverts Q/E controls)
	xyzTranslation(2, 2) = -1;
	
	
	//m_cameraToWorldTransform = xyzRotation * xyzTranslation;
	m_cameraToWorldTransform = xyzTranslation * xyzRotation;

}

// Gets the colour of a given pixel based on the closest object as stored in the pixel buffer
// Params:
//	i, j	Pixel x, y coordinates
Colour Camera::getColourAtPixel(unsigned i, unsigned j)
{
	Colour colour;
	// TODO: update this to make the colouring more interesting!

	// You can use the object information stored in m_pixelBuf
	// for the object and its intersection; if you want to add more information
	// from the intersection test, you'll need to:
	// 1. calculate and pass the values back from Object::getIntersection() (and the derived class overrides)
	// 2. update the ObjectInfo struct in PixelBuffer.h to store the appropriate value types
	// 3. update the marked section in updatePixelBuffer() to store the values in m_pixelBuf

	// You can also retrieve the direction of the ray that intersects the object
	// by calling getRayDirectionThroughPixel(i, j); remember that the ray originates
	// from the camera, whose position is just the origin in camera space.

	// The ObjectInfo struct (defined in PixelBuffer.h) contains
	// information about the closest object to the pixel 
	const ObjectInfo& objInfo = m_pixelBuf.getObjectInfoForPixel(i, j);

	const Object* object = objInfo.object;
	if (object != nullptr)
		colour = object->m_colour;

	return colour;
}