#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");
	tree = Mesh::LoadFromMeshFile("Tree.msh");

	roleTmesh = Mesh::LoadFromMeshFile("Role_T.msh");
	roleTanim = new MeshAnimation("Role_T.anm");
	roleTmaterial = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < roleTmesh->GetSubMeshCount(); ++i)
	{
		const MeshMaterialEntry* matEntry = roleTmaterial->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		roleTmatTextures.emplace_back(texID);
	}
	currentFrame = 0;
	frameTime = 0.0f;

	heightMap = new HeightMap(TEXTUREDIR"testspacetexture.png");
	heightmapSize = heightMap->GetHeightmapSize();

	root = new SceneNode();
	moonManager = new MoonManager();
	root->AddChild(moonManager);

	spaceRoot = new SceneNode();
	spaceMoonManager = new MoonManager();
	spaceRoot->AddChild(spaceMoonManager);

	// generate the lights
	pointLights = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightmapSize.x, 150.0f, rand() % (int)heightmapSize.z));
		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX), 1));
		l.SetRadius(250.0f + (rand() % 2050));
	}

	SetTextures();
	SetShaders();
	SetFBOs();
	SetupPlanetScene();
	SetupSpaceScene();

	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	minimap = new Camera(-90, 180, Vector3(heightmapSize.x / 2, 5000, heightmapSize.z / 2));
	spaceCamera = new Camera(7, 210, Vector3(1530, 1150, 1125));

	light = new Light(heightmapSize * Vector3(0.5f, 5.0f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 1.5);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	timer -= 6;
	init = true;
}

Renderer::~Renderer(void)
{
	delete camera;
	delete minimap;
	delete spaceCamera;
	delete heightMap;
	delete light;
	delete[] pointLights;
	delete root;
	delete spaceRoot;

	delete lightShader;
	delete reflectShader;
	delete skyboxShader;
	delete nodeShader;
	delete bumpShader;
	delete processShader;
	delete sceneShader;
	delete basicShader;
	delete animShader;
	delete depthSceneShader;
	delete pointLightShader;
	delete combineShader;

	delete quad;
	delete sphere;
	delete cube;
	delete tree;

	delete roleTmesh;
	delete roleTanim;
	delete roleTmaterial;

	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &deferredBufferColourTex);
	glDeleteTextures(1, &deferredBufferNormalTex);
	glDeleteTextures(1, &deferredBufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteBuffers(1, &shadowFBO);
	glDeleteBuffers(1, &bufferFBO);
	glDeleteBuffers(1, &processFBO);
	glDeleteBuffers(1, &pointLightFBO);
	glDeleteBuffers(1, &deferredBufferFBO);

}

void Renderer::UpdateScene(float dt)
{
	timer += dt;
	if (cameraUnlocked)
	{
		timer = 0;
		currentCam = 0;
		currentPitchYaw = 0;
		camera->UpdateCamera(dt);
	}
	else
	{
		AutoCameraUpdates();
	}

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.005f;
	frameTime -= dt;
	walkForwardTimer += dt;
	if (walkForwardTimer > 30.0f)
	{
		walkForwardTimer = 0;
	}
	while (frameTime < 0.0f)
	{
		currentFrame = (currentFrame + 1) % roleTanim->GetFrameCount();
		frameTime += 1.0f / roleTanim->GetFrameRate();
	}
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
	spaceRoot->Update(dt);
}

void Renderer::RenderScene()
{
	ViewPlanetScene();

	switch (secondCamera)
	{
	case 0:
		// --- Don't show second camera ---
		break;

	case 1:
		// --- Draw the space view ---
		ViewSpaceScene();
		break;
	case 2:
		// --- Draw the minimap ---
		ViewMinimap();
		break;
	case 3:
		// --- Deferred shading ---
		FillBuffers();
		DrawPointLights();
		CombineBuffers();
	}
}

void Renderer::ViewPlanetScene()
{
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	currentCameraF = camera;
	currentFrustum.FromMatrix(projMatrix * viewMatrix);
	BuildNodeLists(root, &currentFrustum, currentCameraF);
	SortNodeLists();
	DrawShadowScene();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawRoleT();
	DrawNodes();
	ClearNodeLists();

	if (usePostProcessing)
	{
		DrawPostProcess();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	PresentScene();
}

void Renderer::ViewSpaceScene()
{
	viewMatrix = spaceCamera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	currentCameraF = spaceCamera;
	currentFrustum.FromMatrix(projMatrix * viewMatrix);
	BuildNodeLists(spaceRoot, &currentFrustum, currentCameraF);
	SortNodeLists();

	viewMatrix = spaceCamera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawSkybox();
	DrawNodes();
	ClearNodeLists();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width / 3, height / 3);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT); // don't clear the colour this time.
	PresentScene();
}

void Renderer::ViewMinimap()
{
	viewMatrix = minimap->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	currentCameraF = minimap;
	currentFrustum.FromMatrix(projMatrix * viewMatrix);
	BuildNodeLists(root, &currentFrustum, currentCameraF);
	SortNodeLists();

	DrawShadowScene();
	viewMatrix = minimap->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawNodes();
	DrawRoleT();
	ClearNodeLists();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width / 3, height / 3);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT); // don't clear the colour this time.
	PresentScene();
}

