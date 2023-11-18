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
	if (!bumpShader->LoadSuccess())
	{
		return;
	}

	processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	if (!processShader->LoadSuccess())
	{
		return;
	}

	sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!sceneShader->LoadSuccess())
	{
		return;
	}

	greyscaleShader = new Shader("TexturedVertex.glsl", "greyscaleFrag.glsl");
	if (!greyscaleShader->LoadSuccess())
	{
		return;
	}

	basicShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!basicShader->LoadSuccess())
	{
		return;
	}

	deferredSceneShader = new Shader("bumpVertex.glsl", "bufferFragment.glsl");
	pointLightShader = new Shader("pointLightVert.glsl", "pointLightFrag.glsl");
	combineShader = new Shader("combineVert.glsl", "combineFrag.glsl");

	// check all deferred rendering shaders at once
	if (!deferredSceneShader->LoadSuccess() || !pointLightShader->LoadSuccess() || !combineShader->LoadSuccess())
	{
		return;
	}
}
