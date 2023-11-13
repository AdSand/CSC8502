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
	 void AutoCameraUpdates(float dt);
protected:

	Camera* camera;
	bool cameraUnlocked = false;
	float timer = 0;

	Vector3 cameraCheckpoints[4] = 
	{
		Vector3(0, 500, 500), // index 0
		Vector3(680, 250, 1350), // index 1
		Vector3(2400, 1135, 1000), // index 2
		Vector3(0, 300, 700)  // index 3
	};
	int currentCam = 0;

	float pitches[3] =
	{
		0, 0, -15
	};

	float yaws[3] =
	{
		220, 220, 225
	};

	const float cameraSpeed = 80.0f;

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
	Shader* processShader;
	Shader* sceneShader;

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
	void PresentScene();
	void DrawPostProcess();

	// FBOs
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

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
