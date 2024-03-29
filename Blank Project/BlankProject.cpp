#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	//Window w("My planet!", 1920, 1200, true);
	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
		{
			renderer.ToggleCamera();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_V))
		{
			renderer.TogglePostProcessing();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_B))
		{
			renderer.ToggleSecondCamera();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_G))
		{
			renderer.ToggleGreyscale();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_I))
		{
			renderer.GetCameraInfo();
		}
	}
	return 0;
}