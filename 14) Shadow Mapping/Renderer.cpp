#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"

#define SHADOWSIZE 2048

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	camera = new Camera(-3.00f, 315.0f, Vector3(-8.0f, 5.0f, 8.0f));
	light = new Light(Vector3(-20.0f, 10.0f, -20.0f), Vector4(1, 1, 1, 1), 250.0f);

	sceneShader = new Shader("shadowSceneVert.glsl", "shadowSceneFrag.glsl");
	shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");

	if (!sceneShader->LoadSuccess() || !shadowShader->LoadSuccess())
	{
		return;
	}

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	//GL NONE as the parameter to glDrawBuffer - we don’t actually need any colour information for our shadow map pass
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//define a series of meshes
	sceneMeshes.emplace_back(Mesh::GenerateQuad());
	sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Sphere.msh"));
	sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cylinder.msh"));
	sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cone.msh"));

	// load a diffuse (colours) and bump map to apply to objects
	sceneDiffuse = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sceneBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	SetTextureRepeating(sceneDiffuse, true);
	SetTextureRepeating(sceneBump, true);
	glEnable(GL_DEPTH_TEST);

	sceneTransforms.resize(4);
	sceneTransforms[0] = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(10, 10, 1));
	sceneTime = 0.0f;
	init = true;
}

Renderer::~Renderer(void)
{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	for (auto& i : sceneMeshes)
	{
		delete i;
	}
	delete camera;
	delete sceneShader;
	delete shadowShader;
}

void Renderer::UpdateScene(float dt)
{
	// As well as updating the camera, we’re going to increment a ’timer’ float, which we then use to generate
	// some model matrices for the three moving objects in the scene.
	// We don’t create a view matrix here - you’ll see why shortly

	camera->UpdateCamera(dt);
	sceneTime += dt;

	for (int i = 1; i < 4; ++i) // skip the floor
	{
		Vector3 t = Vector3(-10 + (5 * i), 2.0f + sin(sceneTime * i), 0);
		sceneTransforms[i] = Matrix4::Translation(t) * Matrix4::Rotation(sceneTime * 10 * i, Vector3(1, 0, 0));
	}
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawShadowScene();
	DrawMainScene();
}

void Renderer::DrawShadowScene()
{
	// generate a depth buffer image from the pointlight’s point of view, using a frame buffer object
	//  first bind it (line 86) and clear whatever was previously in its buffers(line 88).
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// shadow map depth buffer is much bigger than the screen resolution(by default 2048 * 2048).
	// So, to render into all of it, we must temporarily increase OpenGL’s virtual window size
	// Then, we enable the simple shader program
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	BindShader(shadowShader);

	// create a view matrix, and calculate the correct ’shadow’ matrix to transform world space coordinates into ’light space’.
	// takes two vectors - a viewpoint position, and a point in space for the view matrix to look at.
	// In this case, we want our view matrix to have the light’s position as the origin, and be looking at the objects around the ’world’ origin

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix; // used later

	for (int i = 0; i < 4; ++i)
	{
		//  set the correct model matrix uniform, and draw the mesh
		modelMatrix = sceneTransforms[i];
		UpdateShaderMatrices();
		sceneMeshes[i]->Draw();
	}

	// These draws should go into the shadow map and fill it with depth information
	// once finished, we can restore OpenGL to a state suitable for rendering into the
	// back buffer with - the colour mask is re - enabled(making colours writes work!), the viewport is set
	// back to the screen resolution, and the shadow FBO is disabled.
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawMainScene()
{
	BindShader(sceneShader);
	SetShaderLight(*light);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(sceneShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneDiffuse);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sceneBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (int i = 0; i < 4; ++i)
	{
		modelMatrix = sceneTransforms[i];
		UpdateShaderMatrices();
		sceneMeshes[i]->Draw();
	}
}