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

	SetTextures();
	SetShaders();
	SetFBOs();
	SetupPlanetScene();

	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	minimap = new Camera(-90, 180, Vector3(heightmapSize.x / 2, 5000, heightmapSize.z / 2));
	spaceCamera = new Camera(-35.0f, 223.0f, Vector3(-1100, 2836, -1400));

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
	delete heightMap;
	delete root;
	delete quad;
	delete sphere;

	delete lightShader;
	delete reflectShader;
	delete skyboxShader;
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
	//light->SetRadius(timer * 150);
	root->Update(dt);
}

void Renderer::RenderScene()
{
	BuildNodeLists(root);
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

	// --- Draw the minimap ---

	if (showingMinimap)
	{
		BuildNodeLists(root);
		SortNodeLists();

		//DrawShadowScene();
		viewMatrix = minimap->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		DrawSkybox();
		DrawHeightMap();
		DrawWater();
		DrawNodes();
		ClearNodeLists();

		if (usePostProcessing)
		{
			DrawPostProcess();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width / 3, height / 3);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT); // don't clear the colour this time.
		PresentScene();
	}
	else
	{
		viewMatrix = spaceCamera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		DrawSkybox();
		DrawSmallPlanet();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width / 3, height / 3);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_DEPTH_BUFFER_BIT); // don't clear the colour this time.
		PresentScene();
	}
}

