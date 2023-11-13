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

void Renderer::AutoCameraUpdates(float dt, float timer)
{
	//Vector3 direction = cameraCheckpoints[1] - cameraCheckpoints[0];
		//camera->SetPosition(Vector3(45 * timer, 500, 500));
	camera->SetPosition(cameraCheckpoints[0]);
	camera->SetPitch(0);
	camera->SetYaw(220);
}