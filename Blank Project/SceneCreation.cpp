#include "Renderer.h"

void Renderer::SetTextures()
{
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"spaceright.png", TEXTUREDIR"spaceleft.png",
		TEXTUREDIR"spacetop.png", TEXTUREDIR"spacebottom.png",
		TEXTUREDIR"spacefront.png", TEXTUREDIR"spaceback.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	planetTex = SOIL_load_OGL_texture(TEXTUREDIR"Mossy_Rock_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	planetBump = SOIL_load_OGL_texture(TEXTUREDIR"Mossy_Rock_BUMP.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	moonTex = SOIL_load_OGL_texture(TEXTUREDIR"Rock_04_DIFF.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	SetTextureRepeating(planetTex, true);
	SetTextureRepeating(planetBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(moonTex, true);

	if (!planetTex || !planetBump || !waterTex || !moonTex)
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
		s->SetTransform(Matrix4::Translation(Vector3(3000 + (rand() % 2000), -500 + (rand() % 800), -300.0f + 100.0f + 1000 * i)));
		s->SetModelScale(Vector3(300.0f, 300.0f, 300.f));
		s->SetBoundingRadius(300.0f);
		s->SetMesh(sphere);
		s->SetTexture(moonTex);
		moonManager->AddChild(s);
	}

	// create the crytal
	CrystalCube* c = new CrystalCube();
	c->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	c->SetTransform(Matrix4::Translation(Vector3(2800, 800, 2800)));
	c->SetModelScale(Vector3(700.0f, 700.0f, 700.f));
	c->SetBoundingRadius(500.0f);
	c->SetMesh(cube);
	c->SetTexture(moonTex);
	root->AddChild(c);
}

void Renderer::DrawHeightMap()
{
	BindShader(bumpShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(bumpShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(bumpShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planetTex);

	glUniform1i(glGetUniformLocation(bumpShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, planetBump);

	//reset the model and texture matrices back to identity - the water shader will be
	// modifying these matrices later, and we don’t want this change to affect the heightmap

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawWater()
{
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

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
}

void Renderer::DrawRoleT()
{
	BindShader(animShader);
	glUniform1i(glGetUniformLocation(animShader->GetProgram(), "diffuseTex"), 0);
	modelMatrix = Matrix4::Translation(Vector3(800, 230, 1640)) * Matrix4::Scale(Vector3(50, 50, 50)) * Matrix4::Rotation(180, Vector3(0, 1, 0));
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

//void Renderer::DrawPointLights()
//{
//	BindShader(pointLightShader);
//
//	glClearColor(0, 0, 0, 1);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glBlendFunc(GL_ONE, GL_ONE);
//	glCullFace(GL_FRONT);
//	glDepthFunc(GL_ALWAYS);
//	glDepthMask(GL_FALSE);
//
//	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "depthTex"), 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
//
//	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "normTex"), 1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
//
//	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
//	glUniform2f(glGetUniformLocation(pointLightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
//
//	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
//	glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);
//
//	UpdateShaderMatrices();
//
//	for (int i = 0; i < LIGHT_NUM; ++i)
//	{
//		Light& l = pointLights[i];
//		SetShaderLight(l);
//		sphere->Draw();
//	}
//
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glCullFace(GL_BACK);
//	glDepthFunc(GL_LEQUAL);
//
//	glDepthMask(GL_TRUE);
//
//	glClearColor(0.2f, 0.2f, 0.2f, 1);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}
//
//void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
//{
//	glGenTextures(1, &into);
//	glBindTexture(GL_TEXTURE_2D, into);
//
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
//	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;
//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}
//
//void Renderer::CombineBuffers()
//{
//	BindShader(combineShader);
//	modelMatrix.ToIdentity();
//	viewMatrix.ToIdentity();
//	projMatrix.ToIdentity();
//	UpdateShaderMatrices();
//
//	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
//
//	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);
//
//	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
//
//	quad->Draw();
//}
