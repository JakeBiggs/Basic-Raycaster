#pragma once
#include"Vector3D.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "PixelBuffer.h"
#include "Object.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <chrono>


//Base Class for lights
class Light
{
public:
    
    Light(const Matrix3D &l2w) : lightToWorld(l2w){}
    virtual ~Light();   
    Matrix3D lightToWorld;
    Colour colour;
    Vector3D vColour;
    float intensity;



    //Light(const Point3D& pos, const Colour& inten) : position(pos), intensity(inten) {}
    
};

// Subclass for point lights
class PointLight : public Light
{
public:
    // Constructor takes a light-to-world matrix, a colour and an intensity
    PointLight(const Matrix3D& l2w, const Vector3D & c = 1, const float& i = 1) : Light(l2w)
    {
        this->vColour = c;
        this->intensity = i;
        // Compute the light position in world space by multiplying the origin by the matrix

        pos = l2w * Vector3D(0, 0, 0);
        //l2w.multVecMatrix(Vector3D(0), pos);
    }
        // Member variable for light position
    Vector3D pos;
};

// Subclass for distant lights
class DistantLight : public Light
{
public:
    // Constructor takes a light-to-world matrix, a colour and an intensity[^3^][3]
    DistantLight(const Matrix3D& l2w, const Vector3D& c = 1, const float& i = 1) : Light(l2w)
    {
        this->vColour = c;
        this->intensity = i;
        // Compute the light direction in world space by multiplying the negative z-axis by the matrix
        //l2w.multDirMatrix(Vector3D(0, 0, -1), dir);
        dir = l2w * Vector3D(0, 0, -1);
        //l2w*(Vector3D(0, 0, -1),dir);
            // Normalize the light direction
            dir.normalise();
    }
        // Member variable for light direction
    Vector3D dir;
};


