#ifndef GAME_HPP
# define GAME_HPP

#include <iostream>
#include <iostream>
#include <map>
#include <chrono>
#include <deque>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <classes/Window.hpp>
#include <classes/Game/InputHandler.hpp>
#include <classes/ShaderHandler.hpp>

#include <classes/Texture/TextureLoader.hpp>

#include <classes/World/ChunkClassic.hpp>
#include <classes/World/ChunkInstanciator.hpp>

#include <classes/Game/SkyBox.hpp>

#include <classes/types.hpp>

#include <classes/ECS/ECS.hpp>

#include <thread>

struct DrawInfo
{
	u_int VAO;
	size_t indices_size;
	glm::ivec2 pos;
};



struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;  
    }
};

class Game: public I_Input
{
	//tmp
	unsigned int buffer;
	VertexArrayObject *model_VAO;
	int amount = 10000;

private :

	Window *window;
	InputHandler *inputHandler;
	ShaderHandler *shaderHandler;
	ChunkInstanciator *chunkInstanciator;
	VertexArrayObjectHandler *vertexArrayObjectHandler;

	ECS *ecs;
	
	void SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY) /*override*/;


	//config
	int renderDistance = 50;
	int chunkLoadingSize = renderDistance * 2 + 1;

	TextureArray blockTextureArray;

	//player

	const float speed = 0.35f;
	const float sensitivity = 0.05f;

	float yaw = -90;
	float pitch = 0;

	std::mutex playerPos_mutex;

	glm::vec3 const cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPosition = glm::vec3(0, 100, 0);
	glm::vec3 cameraDirection = glm::vec3(	cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
											sin(glm::radians(pitch)),
											sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
	// glm::vec3 cameraDirection = glm::vec3(0, 0, 0);
	glm::mat4 view = glm::lookAt(	cameraPosition,
									cameraPosition + cameraDirection,
									glm::vec3(0, 1, 0));


	void manageVaoEntity();
									
	void manageVAO();


	std::mutex 					playerHasMoved_mutex;
	bool 						playerHasMoved = false;

	std::mutex					dequeueVAO_mutex;
	std::deque<info_VAO*>		dequeueVAO;

	std::mutex					dequeueDeleteVAO_mutex;
	std::deque<glm::ivec2> 		dequeueDeleteVAO;

	std::mutex					endThreads_mutex;
	bool 						endThreads = false;

	std::mutex					entityPos_mutex;
	std::vector<unsigned char>		*entityPos = NULL;

	std::vector<DrawInfo> Vao_draw;

	void draw();

	std::unordered_map<std::pair<int, int>, u_int, pair_hash> map_VAO;


	//HOTFIX
	std::map<std::pair<int, int>, VAO_data> pos_to_vao;

	int vao_counter = 0;

public :

	Game();
	~Game();

	void StartLoop();
	void Loop();
	std::vector<AChunk*> chunks;

	SkyBox *skyBox;


};

#endif