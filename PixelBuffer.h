#pragma once
#include "stdafx.h"

class Object;

// Structure holding an Object pointer and a float value to represent depth
struct ObjectInfo
{
	const Object* object;
	float distanceToIntersection;	// Distance along the ray from the origin of the intersection point
	ObjectInfo(const Object* obj = nullptr, float d = FLT_MAX)
		: object(obj), distanceToIntersection(d) {}
};

// Class to store information about the closest object to each pixel in a grid.
class PixelBuffer
{
public:
	// Initialises the buffer to the given dimensions
	void init(unsigned width, unsigned height)
	{
		m_width = width;
		m_height = height;
		m_pixels.resize(m_width * m_height);
		clear();
	}

	bool isInitialised() const { return !m_pixels.empty(); }

	unsigned width() const { return m_width; }
	unsigned height() const { return m_height; }

	// Get/set the object info for the pixel with the given indices
	const ObjectInfo&	getObjectInfoForPixel(unsigned i, unsigned j) const { return m_pixels[i + m_height * j]; }
	void				setObjectInfoForPixel(unsigned i, unsigned j, const ObjectInfo& value) { m_pixels[i + m_height * j] = value; }

	// Resets the buffer to the default values, maintaining its size
	void clear()
	{
		static const ObjectInfo clearValue = ObjectInfo();
		std::fill(m_pixels.begin(), m_pixels.end(), clearValue);
	}

private:
	unsigned m_width = 0;
	unsigned m_height = 0;

	std::vector<ObjectInfo> m_pixels;
};