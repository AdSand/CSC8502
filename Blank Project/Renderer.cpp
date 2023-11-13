#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	quad = Mesh::GenerateQuad();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cube = Mesh::LoadFromMeshFile("Cube.msh");

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



	// generate scene depth texture
	//GenerateScreenTexture(bufferDepthTex, true); // true means we are generating a depth or colour texture
	//GenerateScreenTexture(bufferColourTex);
	//GenerateScreenTexture(bufferNormalTex);
	//GenerateScreenTexture(lightDiffuseTex);
	//GenerateScreenTexture(lightSpecularTex);

	heightMap = new HeightMap(TEXTUREDIR"testspacetexture.png");
	heightmapSize = heightMap->GetHeightmapSize();

	root = new SceneNode();
	// create the moon manager
	moonManager = new MoonManager();
	root->AddChild(moonManager);

	SetTextures();
	SetShaders();
	//SetFBOs();
	SetupPlanetScene();

	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));

	light = new Light(heightmapSize * Vector3(0.5f, 5.0f, 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 1.5);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	//We’re going to use alpha-blending on the water
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;
	timer = -6.0f;
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

	//glDeleteFramebuffers(1, &bufferFBO);
	//glDeleteFramebuffers(1, &pointLightFBO);

	//glDeleteTextures(1, &bufferColourTex);
	//glDeleteTextures(1, &bufferNormalTex);
	//glDeleteTextures(1, &bufferDepthTex);
	//glDeleteTextures(1, &lightDiffuseTex);
	//glDeleteTextures(1, &lightSpecularTex);
}

void Renderer::UpdateScene(float dt)
{
	timer += dt;
	if (cameraUnlocked)
	{
		timer = 0;
		currentCam = 0;
		camera->UpdateCamera(dt);
	}
	else
	{
		AutoCameraUpdates(dt);
	}

	viewMatrix = camera->BuildViewMatrix();
	waterRotate += dt * 2.0f;
	waterCycle += dt * 0.005f;
	frameTime -= dt;
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

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	DrawRoleT();
	DrawNodes();
	ClearNodeLists();
}

