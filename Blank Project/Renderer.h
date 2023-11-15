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

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void ToggleCamera();
	 void TogglePostProcessing();
	 void ToggleMinimap();
	 void GetCameraInfo();
	 void AutoCameraUpdates();
protected:

	Camera* camera;
	Camera* minimap;
	bool showingMinimap = true;
	bool cameraUnlocked = false;
	float timer = 0;

	Vector3 cameraCheckpoints[6] = 
	{
		Vector3(0, 500, 0),
		Vector3(1200, 500, 1240),
		Vector3(3215, 300, 0),
		Vector3(1786, 450, 300),
		Vector3(800, 424, 1996),
		Vector3(1397, 424, 2095)
	};

	float pitches[3] =
	{
		0, 7, -14
	};

	float yaws[3] =
	{
		223, 203, 261
	};

	int currentCam = 0;
	int currentPitchYaw = 0;

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
	bool usePostProcessing = false;

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
	Mesh* tree;

	// animated soldier
	Mesh* roleTmesh;
	Shader* animShader;
	MeshAnimation* roleTanim;
	MeshMaterial* roleTmaterial;
	vector<GLuint> roleTmatTextures;

	int currentFrame;
	float frameTime;
	float walkForwardTimer = 0;

	// water
	float waterRotate;
	float waterCycle;

	// trees
	Vector3 treePositions[7] =
	{
		Vector3(2382, 230, 703),
		Vector3(2979, 230, 1540),
		Vector3(2026, 230, 1759),
		Vector3(1498, 230, 2758),
		Vector3(438, 230, 3197),
		Vector3(1057, 230, 605),
		Vector3(880, 230, 1654)
	};

	// shadows
	GLuint shadowTex;
	GLuint shadowFBO;

	Shader* shadowSceneShader;
	Shader* shadowShader;
	void DrawShadowScene();
	void DrawShadowNodes();
};
