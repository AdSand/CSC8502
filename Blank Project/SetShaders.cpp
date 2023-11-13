#include "Renderer.h"

void Renderer::SetShaders()
{
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	if (!skyboxShader->LoadSuccess())
	{
		return;
	}

	lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	if (!lightShader->LoadSuccess())
	{
		return;
	}

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	if (!reflectShader->LoadSuccess())
	{
		return;
	}

	nodeShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!nodeShader->LoadSuccess())
	{
		return;
	}

	animShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	if (!animShader->LoadSuccess())
	{
		return;
	}

	bumpShader = new Shader("BumpVertex.glsl", "BumpFragment.glsl");
	if (!bumpShader)
	{
		return;
	}

}
