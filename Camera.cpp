#include "stdafx.h"
#include "Camera.h"
#include "Object.h"
#include <iostream>
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
			updateLightTransform();
			m_worldTransformChanged = false;
		}

		// Transform the objects to the camera's coordinate system
		const Matrix3D worldToCameraTransform = m_cameraToWorldTransform.inverseTransform();


		for (auto obj : objects) {
			obj->applyTransformation(worldToCameraTransform);
			
		}
		
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
		for (auto obj : objects) {
			obj->applyTransformation(m_cameraToWorldTransform);
		}
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
	yRotation(0, 2) = sinf(m_rotation.y);
	yRotation(1, 1) = 1;
	yRotation(2, 0) = -sinf(m_rotation.y);
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
	
	
	//combines the transformation and rotation matrices to give a final camera to world transformation that we can apply to our 3D objects.
	m_cameraToWorldTransform = xyzRotation * xyzTranslation;
	//m_cameraToWorldTransform = xyzTranslation * xyzRotation;

}

void Camera::updateLightTransform()
{
	// Initialize matrices for X, Y, and Z rotations
	Matrix3D xRotation = Matrix3D();
	Matrix3D yRotation = Matrix3D();
	Matrix3D zRotation = Matrix3D();

	// Set values in xRotation matrix
	xRotation(0, 0) = 1;
	xRotation(1, 1) = cosf(m_rotation.x);
	xRotation(1, 2) = -sinf(m_rotation.x);
	xRotation(2, 1) = sinf(m_rotation.x);
	xRotation(2, 2) = cosf(m_rotation.x);

	// Set values in yRotation matrix
	yRotation(0, 0) = cosf(m_rotation.y);
	yRotation(0, 2) = sinf(m_rotation.y);
	yRotation(1, 1) = 1;
	yRotation(2, 0) = -sinf(m_rotation.y);
	yRotation(2, 2) = cosf(m_rotation.y);

	// Set values in zRotation matrix
	zRotation(0, 0) = cosf(m_rotation.z);
	zRotation(0, 1) = -sinf(m_rotation.z);
	zRotation(1, 0) = sinf(m_rotation.z);
	zRotation(1, 1) = cosf(m_rotation.z);
	zRotation(2, 2) = 1;
	
	Matrix3D xyzRotation = xRotation * yRotation * zRotation;

	

	//Translation matrix
	Matrix3D xyzTranslation = Matrix3D(); //Initialises translation matrix and fills with identity matrix (1 in diagonals top left to bottom right)

	//In 4x4 Matrices, right most columns store	translations along the X,Y and Z axes respectively:
	xyzTranslation(0, 3) = m_position.x;
	xyzTranslation(1, 3) = m_position.y;
	xyzTranslation(2, 3) = m_position.z;

	//Element (2,2) Represents scaling factor along Z-axis. (Inverts Q/E controls)
	xyzTranslation(2, 2) = -1;
	
	// Multiply the matrices together to get the final transformation matrix
	
	m_distantLight.lightToWorld = xyzRotation * xyzTranslation;


}

//Converts point in world space into camera space coordinates
Point3D Camera::worldToCameraSpace(Point3D p) {

	//Scaling the world point to pixel sizes
	float scaledX = p.x * m_pixelWidth;
	float scaledY = p.y * m_pixelHeight;
	
	//X&Y Values are scaled then translated by half the relative viewplane axis. 
	//Z axis does not need to be scaled as depth is already represented correctly
	Point3D CameraPoint = (scaledX - m_viewPlane.halfWidth, scaledY - m_viewPlane.halfHeight, p.z + m_viewPlane.distance);
	return CameraPoint;
}


// Gets the colour of a given pixel based on the closest object as stored in the pixel buffer
// Params:
//	i, j	Pixel x, y coordinates
Colour Camera::getColourAtPixel(unsigned i, unsigned j, std::vector<Object*> m_objects)
{
	Colour colour;
	
	// You can use the object information stored in m_pixelBuf
	// for the object and its intersection; if you want to add more information
	// from the intersection test, you'll need to:
	// 1. calculate and pass the values back from Object::getIntersection() (and the derived class overrides)
	// 2. update the ObjectInfo struct in PixelBuffer.h to store the appropriate value types
	// 3. update the marked section in updatePixelBuffer() to store the values in m_pixelBuf


	const ObjectInfo& objInfo = m_pixelBuf.getObjectInfoForPixel(i, j);

	const Object* object = objInfo.object;
	if (object != nullptr)
	{
		//Point3D hitPoint = objInfo.intersectionPoint;
		Vector3D rayDir = getRayDirectionThroughPixel(i, j);
		Vector3D hitNormal;
		Point3D origin = m_position;

		//bool intersection = object->getIntersection(objInfo.object->intersectionPoint, rayDir);

		//hitColor = object.albedo / M_PI * light->intensity * light->color * std::max(0, hitNormal.dot(L));

		colour = object->m_colour;
		colour = Phong(object, colour, origin, rayDir, &m_distantLight);
	}
	return colour;
	
}

Vector3D  ColourToVector(Colour c) {
	return Vector3D(c.r, c.b, c.g);
}
Colour VectorToColour(Vector3D v) {
	return Colour(v.x, v.y, v.z);
}

float clamp(float input, float lb, float ub){ 
	//return (input < lb) ? lb : ((input > ub) ? ub : input); 
	if (input < lb) { return lb; }
	else if (input > ub) { return ub; }
	else return input;
}

//Calculates the refelection vector of a given incident ray, using surface normals
Vector3D Camera::getReflectionVector(Vector3D& U, Vector3D& N) {
	
	//Using the reflection Formula:
	// Reflected Ray Vector = Incident Ray Vector - 2*(Incident Dot Normal)*Normal
	// R = U - 2(U .DOT. N)N
	// R: the final reflected vector
	// U: The incident ray (vector being refelcted)
	// N: Surface normal (found using object helper functions for sphere/plane)

	Vector3D R = U - (2 * N * (U.dot(N)));
	return R;

}

Colour Camera::Phong(const Object* object, Colour colour, Point3D raySrc, Vector3D rayDir, DistantLight* light) {
	
	//Casts the shape classes onto the object to see what type of object is actually is, will return nullptr if not that object
	const Sphere* sphere = dynamic_cast<const Sphere*>(object);
	const Plane* plane = dynamic_cast<const Plane*>(object);


	//Phong Shading Values:
	Vector3D diffuse=0, specular, ambient;
	Vector3D objectColourVector = ColourToVector(colour); //We need to convert the colour object to a vector so that we can include it in our maths

	float distToIntersection;
	if (sphere != nullptr) { //If sphere doesn't return a null pointer when cast onto object (i.e it IS a sphere)
		distToIntersection = sphere->getDistToIntersection(raySrc, rayDir);
	}
	if (plane != nullptr) { //If plane doesn't return a null pointer when cast onto object (i.e it IS a plane)
		distToIntersection = plane->getDistToIntersection(raySrc, rayDir);
	}

	// get the current object and distance to its intersection
	//TODO: fix null warning
	//object->getIntersection(raySrc, rayDir, distToIntersection);


	Point3D intersectionPoint;
	Vector3D lightDirection;
	Vector3D intersectionToLight;
	Vector3D lightIntensity, reflectionVector, lightColourVector;
	Vector3D normal;

	//Calculates point of intersection using:
	//Intersection = Origin + |Distance| dot(RayDirection) 
	//or I = O + |D|R
	float distMag = abs(distToIntersection);
	intersectionPoint = raySrc + distMag * rayDir;


	//Calculating Normal Vectors:
	if (sphere != nullptr) { //If sphere doesn't return a null pointer when cast onto object (i.e it IS a sphere)
		normal = sphere->calculateNormal(intersectionPoint);
		normal.normalise();
	}
	if (plane != nullptr) { //If plane doesn't return a null pointer when cast onto object (i.e it IS a plane)
		normal = plane->calculateNormal();
		normal.normalise();
	}

	//Calculates Light Direction Vector using:
	//LightDirection = IntersectionPoint - LightPosition
	//or L = I - P.
	// I: Point where ray intersects with object in scene
	// P: current Light's position value.
	// L: Resulting vector representing direction of Light ray
	lightDirection = (light->direction)*-1;
	lightDirection.normalise(); //normalised so that we can apply it to our final light vector

	//Finds vector pointing from the point of intersection to the light source using:
	//LightVector = LightPosition-IntersectionPoint
	//or L = P - I
	//Resulting vector representing the intersection to light vector
	//intersectionToLight = lights[l]->position() - intersectionPoint;
	//intersectionToLight.normalise();

	//Calculating Reflection Rays of current light:
	reflectionVector = getReflectionVector(lightDirection, normal);
	reflectionVector.normalise();

	//Calculating raySrc (origin) to camera vector
	//C = O - I
	//Using vector rules Camera = Origin - Intersection
	Vector3D srcToCam = raySrc - intersectionPoint;
	srcToCam.normalise();

	//Gets current light colour and converts to vector format (rgb = xyz)
	lightColourVector = ColourToVector(light->colour);


	//Diffuse Light is calculated using:
	//Diffuse = id * kd * (L DOT N)
	//Where:
	//id: diffuse intensity 
	//kd: diffuse reflection constant
	//L: Eye to Light Vector
	//N: Surface Normal Vector
	float globalDiffuseReflection = 0.3f;
	float diffuseCoeffecient = max(0.0f, normal.dot(lightDirection));
	//diffuse = diffuse + lightColourVector * lights[l]->ambientIntensity * abs(normal.dot(intersectionToLight)) * globalDiffuseReflection;
	diffuse = lightColourVector * light->intensity * diffuseCoeffecient;// *globalDiffuseReflection;


	//Specular Light is calculated using:
	// Specular = is * ks * (R DOT V)^n  * LightColour
	//Where:
	//is: specular intensity (in our simplified model we will use our ambient intensity value for all lights)
	//ks: specular reflection constant (we can ignore this term if we want uniform materials/reflections)
	//R: Reflected Vector 
	//V: origin to camera vector
	//N: the shininess exponent, controlling the size of the specular highlights
	//LightColour: colour of the light
	float globalSpecularReflection = 0.8f; 
	int shinyExp = 10;
	float specularInensity = light->intensity * 10;
	//specular = (specular + lights[l]->ambientIntensity * globalSpecularReflection * pow((reflectionVector.dot(srcToCam)), shinyExp) * lightColourVector);// *specScale;
	//specular = (lights[l]->ambientIntensity * globalSpecularReflection * pow((reflectionVector.dot(srcToCam)), shinyExp) * lightColourVector);// *specScale;
	float specularCoefficient = pow(max(0.0f,reflectionVector.dot(srcToCam)), shinyExp);
	specular = lightColourVector * specularInensity * specularCoefficient;
	//specular = light->intensity * globalSpecularReflection * pow((reflectionVector.dot(srcToCam)), shinyExp) * lightColourVector;// *specScale;

	//Ambient Light is calculated as:
	//Ambient = lightColour * ka * ia
	//Where:
	//ka: Ambient Reflection constant
	//ia: Amient intensity.
	float ka = 0.18f;
	//ambient = ambient + lightColourVector * lights[l]->ambientIntensity * ka;
	ambient = lightColourVector * light->intensity * ka;


	
	
	Vector3D phong = diffuse+specular + ambient;
	
	// combines the shading effects with the original color, ensuring that the resulting color values are in the normalized range 0-1.
	//The division by 255 is used to bring the intensity values back to a valid color range if they have been scaled during the shading calculations.
	objectColourVector = Vector3D(phong.x * objectColourVector.x / 255.0f, phong.y * objectColourVector.y / 255.0f, phong.z * objectColourVector.z / 255.0f);

	//Clamping values within RGB Range (hopefully stops plane phasing out of existence?)
	//objectColourVector.x = std::clamp()
	objectColourVector.x = clamp(objectColourVector.x, 0, 255);
	objectColourVector.y = clamp(objectColourVector.y, 0, 255);
	objectColourVector.z = clamp(objectColourVector.z, 0, 255);
	
	return VectorToColour(objectColourVector);


	//	return colour;
}

