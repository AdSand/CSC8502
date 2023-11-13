#pragma once
#include "Plane.h"

class SceneNode; // Let compiler know these keywords are classes
class Matrix4; // without including the whole header

class Frustum
{
public:
	Frustum(void) {};
	~Frustum(void) {};

	void FromMatrix(const Matrix4& mvp); // generate the planes
	bool InsideFrustum(SceneNode& n); // is the node in the frustum?


protected:
	Plane planes[6];
};

