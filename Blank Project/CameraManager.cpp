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
		// increase by 2 for every added camera movement
		if (currentCam >= 6)
		{
			currentCam = 0;
		}
		else
		{
			currentCam += 2;
		}

		// increase by 1 for every added camera movement
		if (currentPitchYaw >= 3)
		{
			currentPitchYaw = 0;
		}
		else
		{
			currentPitchYaw++;
		}
	}
}

void Renderer::ToggleSecondCamera()
{
	secondCamera++;
	if (secondCamera >= 4)
	{
		secondCamera = 0;
	}
}

void Renderer::ToggleGreyscale()
{
	useGreyscale = useGreyscale ? false : true;
}