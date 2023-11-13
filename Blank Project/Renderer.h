#pragma once
#include <algorithm>
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "MoonManager.h"
#include "CrystalCube.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void ToggleCamera();
	 void GetCameraInfo();
protected:

	Camera* camera;
	bool cameraUnlocked = false;
	float timer = 0;
	Vector3 cameraCheckpoints[4] = 
	{
		Vector3(45, 500, 500), // index 0
		Vector3(1, 1, 1), // index 1
		Vector3(2, 2, 2), // index 2
		Vector3(3, 3, 3)  // index 3
	};

	HeightMap* heightMap;
	Vector3 heightmapSize;
	Light* light;

	// Scene graph and management
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	SceneNode* root;
	MoonManager* moonManager;
	Frustum frameFrustum;

	// shaders
	Shader* lightShader;
	Shader* reflectShader; // reflect for water
	Shader* skyboxShader; // draw the skybox
	Shader* nodeShader;
	Shader* bumpShader;

	//Shader* sceneShader; // shader to fill our GBuffer
	//Shader* pointLightShader; // shader to calculate lighting
	//Shader* combineShader; // Combination render pass

	// Setup the project
	void SetFBOs();
	void SetShaders();

	// Drawing objects
	void SetTextures();
	void DrawSkybox();
	void DrawHeightMap();
	void DrawRoleT();
	void SetupPlanetScene();
	void DrawWater();
	//void DrawPointLights();

	// FBOs
	//GLuint bufferFBO; // FBO for our G-buffer pass
	//GLuint bufferColourTex; // albedo goes here
	//GLuint bufferNormalTex; // normals go here
	//GLuint bufferDepthTex; // depth goes here

	//GLuint pointLightFBO; // FBO for our lighting pass
	//GLuint lightDiffuseTex; // Store diffuse lighting
	//GLuint lightSpecularTex; // store specular lighting

	//void GenerateScreenTexture(GLuint& into, bool depth = false);
	//void CombineBuffers();

	// textures
	GLuint cubeMap;
	GLuint planetTex;
	GLuint planetBump;
	GLuint waterTex;
	GLuint moonTex;
	GLuint crystalTex;

	// meshes
	Mesh* quad;
	Mesh* sphere;
	Mesh* cube;

	// animated soldier
	Mesh* roleTmesh;
	Shader* animShader;
	MeshAnimation* roleTanim;
	MeshMaterial* roleTmaterial;
	vector<GLuint> roleTmatTextures;

	int currentFrame;
	float frameTime;

	// water
	float waterRotate;
	float waterCycle;
};
