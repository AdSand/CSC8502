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

void Renderer::AutoCameraUpdates(float dt)
{
	Vector3 direction = (cameraCheckpoints[currentCam + 1] - cameraCheckpoints[currentCam]).Normalised();
	camera->SetPosition(Vector3(
		direction.x * timer * 80,
		cameraCheckpoints[currentCam].y,
		direction.z * timer * 80));
	camera->SetPitch(pitches[currentCam]);
	camera->SetYaw(yaws[currentCam]);
	std::cout << currentCam << " and the timer: " << timer << std::endl;
	if (timer > 5.0f)
	{
		timer = 0;
		if (currentCam >= 2)
		{
			currentCam = 0;
		}
		else
		{
			currentCam += 1;
		}
	}
}