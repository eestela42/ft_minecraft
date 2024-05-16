#include <classes/VAO/VertexArrayObjectHandler.hpp>

VertexArrayObjectHandler::VertexArrayObjectHandler() {}

VertexArrayObjectHandler::~VertexArrayObjectHandler()
{
	for (auto const &x : vaoMap)
	{
		delete x.second;
	}
}

void VertexArrayObjectHandler::Draw(u_int type)
{
	switch (type)
	{
	case 0:
		DrawElements();
		break;
	case 1:
		DrawArray();
		break;
	}
}

void VertexArrayObjectHandler::DrawElements()
{
	if (activeVAO)
	{
		glDrawElements(GL_TRIANGLES, vaoMap[activeVAO - 1]->GetIndicesDataSize(), GL_UNSIGNED_INT, 0);
	}
}

void VertexArrayObjectHandler::DrawArray()
{
	if (activeVAO)
	{
		glDrawArrays(GL_POINTS, 0, vaoMap[activeVAO - 1]->GetIndicesSize());
	}
}

void VertexArrayObjectHandler::DrawAll(u_int type)
{
	switch (type)
	{
	case 0:
		DrawAllElements();
		break;
	case 1:
		DrawAllArray();
		break;
	}
}

void VertexArrayObjectHandler::DrawAllElements()
{
	Unbind();
	for (auto const &x : vaoMap)
	{

		x.second->Bind();
		glDrawElements(GL_TRIANGLES, x.second->GetIndicesDataSize(), GL_UNSIGNED_INT, 0);
		x.second->Unbind();
	}
}

void VertexArrayObjectHandler::DrawAllArray()
{
	Unbind();
	for (auto const &x : vaoMap)
	{

		x.second->Bind();
		glDrawArrays(GL_POINTS, 0, x.second->GetIndicesSize());
		x.second->Unbind();
	}
}

bool isInFrustum(const glm::vec4 &clipSpacePosition)
{
	// Check if the position is inside the normalized device coordinates (NDC)
	if (clipSpacePosition.x >= -1.0f && clipSpacePosition.x <= 1.0f &&
		clipSpacePosition.y >= -1.0f && clipSpacePosition.y <= 1.0f)
	{
		return true;
	}
	return false;
}

u_int VertexArrayObjectHandler::AddVAO(VertexArrayObject *vao)
{
	indexCount++;
	vaoMap[indexCount] = vao;
	activeVAO = indexCount;
	return indexCount;
}

void VertexArrayObjectHandler::RemoveVAO(u_int VAO)
{
	delete vaoMap[VAO];
	vaoMap.erase(VAO);
	if (VAO == activeVAO)
	{
		activeVAO = 0;
	}
}

void VertexArrayObjectHandler::Bind(u_int VAO)
{
	if (VAO && activeVAO != VAO)
	{
		if (activeVAO)
		{
			vaoMap[activeVAO]->Unbind();
		}
		vaoMap[VAO]->Bind();
		activeVAO = VAO;
	}
}

void VertexArrayObjectHandler::Unbind()
{
	if (activeVAO)
	{
		vaoMap[activeVAO]->Unbind();
	}
	glBindVertexArray(0);
}

VertexArrayObject *VertexArrayObjectHandler::GetVAO(u_int VAO)
{
	return (vaoMap[VAO]);
}
