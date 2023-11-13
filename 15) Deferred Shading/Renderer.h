#pragma once
#include "../nclgl/OGLRenderer.h"
class Camera;
class Mesh;
class HeightMap;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	void FillBuffers(); // G-buffer fill render pass
	void DrawPointLights(); // Lighting render pass
	void CombineBuffers(); // Combination render pass

	// Make a new texture...
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	Shader* sceneShader; // shader to fill our GBuffer
	Shader* pointLightShader; // shader to calculate lighting
	Shader* combineShader; // Combination render pass

	GLuint bufferFBO; // FBO for our G-buffer pass
	GLuint bufferColourTex; // albedo goes here
	GLuint bufferNormalTex; // normals go here
	GLuint bufferDepthTex; // depth goes here

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightDiffuseTex; // Store diffuse lighting
	GLuint lightSpecularTex; // store specular lighting

	HeightMap* heightMap; // Terrain!
	Light* pointLights; // Array of lighting data
	Mesh* sphere; // Light volume
	Mesh* quad; // Draw a full screen quad
	Camera* camera;
	GLuint earthTex;
	GLuint earthBump;
};

