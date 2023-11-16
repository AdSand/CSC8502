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
const int LIGHT_NUM = 100;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void ToggleCamera();
	 void TogglePostProcessing();
	 void ToggleSecondCamera();
	 void GetCameraInfo();
	 void AutoCameraUpdates();
protected:

	Camera* camera;
	Camera* minimap;
	Camera* spaceCamera;
	int secondCamera = 0;
	bool cameraUnlocked = false;
	float timer = 0;

	Vector3 cameraCheckpoints[8] = 
	{
		Vector3(0, 500, 0),
		Vector3(1200, 500, 1240),
		Vector3(3215, 300, 0),
		Vector3(1786, 450, 300),
		Vector3(800, 424, 1996),
		Vector3(1397, 424, 2095),
		Vector3(1982, 140, 1354),
		Vector3(1982, 100, 1354)
	};

	float pitches[4] =
	{
		0, 7, -14, 7
	};

	float yaws[4] =
	{
		223, 168, 261, 289
	};

	int currentCam = 0;
	int currentPitchYaw = 0;

	HeightMap* heightMap;
	Vector3 heightmapSize;
	Light* light;

	// Scene graph and management
	void BuildNodeLists(SceneNode* from, Frustum* currentF, Camera* currentC);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
	SceneNode* root;
	MoonManager* moonManager;
	Frustum frameFrustum;

	Frustum currentFrustum;
	Camera* currentCameraF;
	

	// shaders
	Shader* lightShader;
	Shader* reflectShader; // reflect for water
	Shader* skyboxShader; // draw the skybox
	Shader* nodeShader;
	Shader* bumpShader;
	Shader* processShader;
	Shader* sceneShader;
	Shader* basicShader;

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

	void ViewPlanetScene();
	void ViewSpaceScene();
	void ViewMinimap();

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

	void SetupSpaceScene();
	SceneNode* spaceRoot;
	MoonManager* spaceMoonManager;

	// deferred
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Shader* deferredSceneShader;
	Shader* pointLightShader;
	Shader* combineShader;

	GLuint deferredBufferFBO; // FBO for our G-buffer pass
	GLuint deferredBufferColourTex; // albedo goes here
	GLuint deferredBufferNormalTex; // normals go here
	GLuint deferredBufferDepthTex; // depth goes here

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightDiffuseTex; // Store diffuse lighting
	GLuint lightSpecularTex; // store specular lighting

	Light* pointLights; // Array of lighting data
};
