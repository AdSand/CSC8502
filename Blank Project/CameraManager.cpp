#include "Renderer.h"

void Renderer::ToggleCamera()
{
	cameraUnlocked = cameraUnlocked ? false : true;
}

void Renderer::GetCameraInfo()
{
	// print camera information to the console
	std::cout << "Position: " << camera->GetPosition() << std::endl;
	std::cout << "Pitch: " << camera->GetPitch() << std::endl;
	std::cout << "Yaw: " << camera->GetYaw() << std::endl;
}