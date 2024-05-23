#pragma once

#include <classes/VAO/VertexArrayObject.hpp>
#include <unordered_map>
#include <mutex>


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

typedef struct t_vaoData
{
	std::pair<int, int> pos;
	void *ptr;
	t_vertexData& vertexData;
	std::vector<u_int>& indices;
	Shader*	shader;
} t_vaoData;

class VertexArrayObjectHandler
{
public:
	VertexArrayObjectHandler();
	~VertexArrayObjectHandler();

	void Draw(u_int type);
	void DrawElements();
	void DrawArray();

	void DrawAll(u_int type);
	void DrawAllElements();
	void DrawAllArray();

	VertexArrayObject *GetVAO(u_int VAO);

	u_int AddVAO(VertexArrayObject *);
	void RemoveVAO(u_int VAO);
	void Bind(u_int VAO);
	void Unbind();

	std::mutex mutex_toAdd;
	void pushBackToAdd(t_vaoData data);
	std::vector<t_vaoData> toAdd;

	std::mutex mutex_toDelete;
	void pushBackToDelete(u_int VAO);
	std::vector<u_int> toDelete;


private:
	std::unordered_map<u_int, VertexArrayObject *> vaoMap;
	u_int activeVAO = 0;
	u_int indexCount = 0;
};