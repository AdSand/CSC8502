#include "CrystalCube.h"

void CrystalCube::Update(float dt)
{
	transform = transform * Matrix4::Rotation(50.0f * dt, Vector3(0.7f, 0.6f, 0.5f));

	SceneNode::Update(dt);
}
