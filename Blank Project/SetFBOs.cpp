#include "Renderer.h"

void Renderer::SetFBOs()
{
	//// buffers for first and second passes (draw scene and add lights)
	//glGenFramebuffers(1, &bufferFBO);
	//glGenFramebuffers(1, &pointLightFBO);

	//GLenum buffers[2] = {
	//GL_COLOR_ATTACHMENT0,
	//GL_COLOR_ATTACHMENT1
	//};

	//// start binding attachments to respective FBOs
	//glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	//glDrawBuffers(2, buffers);
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	return;
	//}

	//glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);;
	//glDrawBuffers(2, buffers);
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//{
	//	return;
	//}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}