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

Vector3 Lerp(Vector3& start, Vector3 end, float time)
{
	Vector3 newPos = Vector3(
		start.x + (end.x - start.x) * time * 0.15,
		start.y + (end.y - start.y) * time * 0.15,
		start.z + (end.z - start.z) * time * 0.15
	);
	return newPos;
}

void Renderer::AutoCameraUpdates()
{
	camera->SetPosition(Lerp(cameraCheckpoints[currentCam], cameraCheckpoints[currentCam + 1], timer));
	camera->SetPitch(pitches[currentPitchYaw]);
	camera->SetYaw(yaws[currentPitchYaw]);

	if (timer > 5.0f)
	{
		timer = 0;
		if (currentCam >= 4)
		{
			currentCam = 0;
		}
		else
		{
			currentCam += 2;
		}

		if (currentPitchYaw >= 2)
		{
			currentPitchYaw = 0;
		}
		else
		{
			currentPitchYaw++;
		}
	}
}

void Renderer::ToggleSmallCamera()
{
	smallCamera++;
	if (smallCamera >= 2)
	{
		smallCamera = 0;
	}
}