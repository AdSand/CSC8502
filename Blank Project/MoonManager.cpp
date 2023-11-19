#include "MoonManager.h"

void MoonManager::Update(float dt)
{
	transform = transform * Matrix4::Rotation(10.0f * dt, Vector3(0, 1, 0));
	SceneNode::Update(dt);
}