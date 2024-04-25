#include "Renderer.h"

void Renderer::SetTextures()
{
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"spaceright.png", TEXTUREDIR"spaceleft.png",
		TEXTUREDIR"spacetop.png", TEXTUREDIR"spacebottom.png",
		TEXTUREDIR"spacefront.png", TEXTUREDIR"spaceback.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	planetTex = SOIL_load_OGL_texture(TEXTUREDIR"Mossy_Rock_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	moonTex = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	crystalTex = SOIL_load_OGL_texture(TEXTUREDIR"crystal.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	rainTex = SOIL_load_OGL_texture(TEXTUREDIR"rain.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	SetTextureRepeating(planetTex, true);
	SetTextureRepeating(planetBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(moonTex, true);
	SetTextureRepeating(crystalTex, true);

	if (!planetTex || !planetBump || !waterTex || !moonTex || !crystalTex || !rainTex)
	{
		return;
	}
}

void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::SetupPlanetScene()
{
	//moonManager->SetModelScale(Vector3(50.0f, 50.0f, 50.f));
	//moonManager->SetBoundingRadius(50.0f);
	//moonManager->SetMesh(sphere);
	moonManager->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.5f, 5.0f, 0.5f)));

	// create the moons
	for (int i = 0; i < 3; ++i)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		s->SetTransform(Matrix4::Translation(Vector3(-3000 - (rand() % 2000), -500 + (rand() % 800), -300.0f + 100.0f + 1000 * i)));
		s->SetModelScale(Vector3(300.0f, 300.0f, 300.f));
		s->SetBoundingRadius(300.0f);
		s->SetMesh(sphere);
		s->SetTexture(moonTex);
		moonManager->AddChild(s);
	}

	// create the crytal
	CrystalCube* c = new CrystalCube();
	c->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	c->SetTransform(Matrix4::Translation(Vector3(2700, 750, 2700)));
	c->SetModelScale(Vector3(700.0f, 700.0f, 700.f));
	c->SetBoundingRadius(700.0f);
	c->SetMesh(cube);
	c->SetTexture(crystalTex);
	root->AddChild(c);

	// create the trees
	for (int i = 0; i < 23; i++)
	{
		SceneNode* t = new SceneNode();
		t->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		t->SetTransform(Matrix4::Translation(treePositions[i]) * Matrix4::Rotation((rand() % 360), Vector3(0, 1, 0)));
		float randomScale = 20 + (rand() % 30);
		t->SetModelScale(Vector3(randomScale, randomScale, randomScale));
		t->SetBoundingRadius(500.0f);
		t->SetMesh(tree);
		t->SetTexture(crystalTex);
		root->AddChild(t);
	}
}

void Renderer::DrawHeightMap()
{
	BindShader(bumpShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(bumpShader->GetProgram(), "cameraPos"), 1, (float*)&currentCameraF->GetPosition());

	glUniform1i(glGetUniformLocation(bumpShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planetTex);

	glUniform1i(glGetUniformLocation(bumpShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planetBump);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawWater(float transparency)
{
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&currentCameraF->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);
	glUniform1f(glGetUniformLocation(reflectShader->GetProgram(), "transparency"), transparency);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));

	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	quad->Draw();
	textureMatrix.ToIdentity();
}

void Renderer::DrawParticles()
{
	BindShader(particleShader);

	UpdateShaderMatrices();

	for (auto& ps : particleSystems)
	{
		glBindTexture(GL_TEXTURE_2D, ps->GetTexture());
		ps->DrawParticles();
	}
}

void Renderer::DrawRoleT()
{
	BindShader(animShader);
	glUniform1i(glGetUniformLocation(animShader->GetProgram(), "diffuseTex"), 0);
	modelMatrix = Matrix4::Translation(roleTposition) *
		Matrix4::Scale(Vector3(50, 50, 50)) * 
		Matrix4::Rotation(90, Vector3(0, 1, 0));
	UpdateShaderMatrices();


	vector<Matrix4> frameMatrices;

	const Matrix4* invBindPose = roleTmesh->GetInverseBindPose();
	const Matrix4* frameData = roleTanim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < roleTmesh->GetJointCount(); ++i)
	{
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(animShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	for (int i = 0; i < roleTmesh->GetSubMeshCount(); ++i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, roleTmatTextures[i]);
		roleTmesh->DrawSubMesh(i);
	}
}

void Renderer::SetupParticleSystems()
{
	rainParticles = new ParticleSystem({0, 1500, 0}, {0, 0, 0}, {3000, 1, 3000}, 20, 500.0f, 3, 35.25f, 0.25f, rainTex);
	particleSystems.push_back(rainParticles);
}

void Renderer::UpdateParticleSystems(float dt)
{
	for (auto& ps : particleSystems)
	{
		ps->CreateNewParticles(dt);
		ps->UpdateParticles(dt, camera->GetPosition());
	}
}

void Renderer::SetupSpaceScene()
{
	spaceMoonManager->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.5f, 5.0f, 0.5f)));

	// create the moons
	for (int i = 0; i < 3; ++i)
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		s->SetTransform(Matrix4::Translation(Vector3(-300 - (rand() % 200), -50 + (rand() % 80), -30.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(50.0f, 50.0f, 50.f));
		s->SetBoundingRadius(500.0f);
		s->SetMesh(sphere);
		s->SetTexture(moonTex);
		spaceMoonManager->AddChild(s);
	}

	// create the crytal
	CrystalCube* c = new CrystalCube();
	c->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	c->SetTransform(Matrix4::Translation(Vector3(-250, 100, -250)));
	c->SetModelScale(Vector3(30.0f, 30.0f, 30.f));
	c->SetBoundingRadius(100.0f);
	c->SetMesh(cube);
	c->SetTexture(crystalTex);
	spaceMoonManager->AddChild(c);

	// create the planet
	SceneNode* d = new SceneNode();
	d->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	d->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	d->SetModelScale(Vector3(300.0f, 300.0f, 300.f));
	d->SetBoundingRadius(300.0f);
	d->SetMesh(sphere);
	d->SetTexture(planetTex);
	spaceMoonManager->AddChild(d);
}