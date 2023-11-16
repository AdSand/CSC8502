#include "Renderer.h"

void Renderer::BuildNodeLists(SceneNode* from, Frustum* currentF, Camera* currentC)
{
	if (currentF->InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - currentC->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i), currentF, currentC);
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(),
		transparentNodeList.rend(),
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes()
{
	for (const auto& i : nodeList)
	{
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList)
	{
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		if (n->GetTexture() == planetTex)
		{
			BindShader(nodeShader);
			UpdateShaderMatrices();
			GLuint texture = n->GetTexture();

			glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "diffuseTex"), 0);

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(nodeShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(nodeShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			glUniform1i(glGetUniformLocation(nodeShader->GetProgram(), "useTexture"), texture);

			n->Draw(*this);
		}

		if (n->GetTexture() == moonTex || n->GetTexture() == crystalTex)
		{
			BindShader(lightShader);
			SetShaderLight(*light);
			GLuint texture = n->GetTexture();
			UpdateShaderMatrices();

			glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "diffuseTex"), 0);

			Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
			glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "modelMatrix"), 1, false, model.values);
			glUniform4fv(glGetUniformLocation(lightShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "useTexture"), texture);

			n->Draw(*this);
		}
	}
}

void Renderer::DrawShadowNodes()
{
	for (int i = 0; i < 6; i++)
	{
		modelMatrix = Matrix4::Translation(treePositions[i]) * Matrix4::Scale(Vector3(100, 100, 100));
		UpdateShaderMatrices();
		tree->Draw();
	}
}