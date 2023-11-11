#include "stdafx.h"
#include "Matrix3D.h"

//--------------------------------------------------------------------------------------------------------------------//

// Returns the inverse of this transformation matrix, such that this * this->inverseTransform() gives the identity matrix.
Matrix3D Matrix3D::inverseTransform() const
{
	Matrix3D inverse;

	// From http://www.graphics.stanford.edu/courses/cs248-98-fall/Final/q4.html
	inverse(0, 0) = m_elements[0][0];
	inverse(1, 0) = m_elements[0][1];
	inverse(2, 0) = m_elements[0][2];
	inverse(0, 1) = m_elements[1][0];
	inverse(1, 1) = m_elements[1][1];
	inverse(2, 1) = m_elements[1][2];
	inverse(0, 2) = m_elements[2][0];
	inverse(1, 2) = m_elements[2][1];
	inverse(2, 2) = m_elements[2][2];
	inverse(0, 3) = -m_elements[0][0] * m_elements[0][3] - m_elements[1][0] * m_elements[1][3] - m_elements[2][0] * m_elements[2][3];
	inverse(1, 3) = -m_elements[0][1] * m_elements[0][3] - m_elements[1][1] * m_elements[1][3] - m_elements[2][1] * m_elements[2][3];
	inverse(2, 3) = -m_elements[0][2] * m_elements[0][3] - m_elements[1][2] * m_elements[1][3] - m_elements[2][2] * m_elements[2][3];

	return inverse;
}

//--------------------------------------------------------------------------------------------------------------------//

// Multiplies the components of a point/vector
void Matrix3D::multiply(float& x, float& y, float& z, float& w) const
{
	float x_ = m_elements[0][0] * x + m_elements[0][1] * y + m_elements[0][2] * z + m_elements[0][3] * w,
		y_ = m_elements[1][0] * x + m_elements[1][1] * y + m_elements[1][2] * z + m_elements[1][3] * w,
		z_ = m_elements[2][0] * x + m_elements[2][1] * y + m_elements[2][2] * z + m_elements[2][3] * w,
		w_ = m_elements[3][0] * x + m_elements[3][1] * y + m_elements[3][2] * z + m_elements[3][3] * w;
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}
