#ifndef TEMP_H
#define TEMP_H

#include <interfaces/Interface_Input.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <atomic>

#include <classes/World/ChunkDefault.hpp>
#include <classes/World/ChunkRLE.hpp>
#include <classes/Window.hpp>
#include <classes/Game/InputHandler.hpp>
#include <classes/ShaderHandler.hpp>
#include <classes/VAO/VertexArrayObjectHandler.hpp>
#include <classes/World/ChunkInstantiator.hpp>
#include <classes/Texture/Texture.hpp>
#include <classes/Texture/TextureArray.hpp>
#include <classes/Game/SkyBox.hpp>
#include <classes/Game/CrossHair.hpp>

#include <mutex>

class Game : I_Input
{
	private:
		Window *window;
		InputHandler *inputHandler;
		ShaderHandler *shaderHandler;
		VertexArrayObjectHandler *vertexArrayObjectHandler;
		ChunkInstantiator *instantiator;

		std::mutex chunkStableMutex;
		std::vector<Chunk*> *chunkStableState;

		void Loop();

		static glm::vec3 const cameraUp;
		int renderDistance = 15;
		int chunkLoadingSize = renderDistance * 2 + 1;
		const float speed = 0.35f;
		const float sensitivity = 0.05f;

		std::mutex cameraMutex;

		float yaw = -90;
		float pitch = 0;
		glm::vec3 cameraPosition = glm::vec3(0, 100, 0);
		glm::vec3 cameraDirection = glm::vec3(	cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
												sin(glm::radians(pitch)),
												sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
		glm::mat4 view = glm::lookAt(	cameraPosition,
										cameraPosition + cameraDirection,
										glm::vec3(0, 1, 0));
	public:
		Game();

	int tmp = 0;

		SkyBox *skyBox;
		CrossHair *crossHair;
		void SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY) override;
		void StartLoop();

		glm::mat4 GetCameraView() const;
		int	GetRenderDistance() const;
		int	GetChunkLoadingSize() const;

		Texture blockTexture;
		TextureArray blockTextureArray;


		bool	putBlock(glm::vec3 pos, u_char type);
		void deleteBlock();
		glm::vec4 findColorFilter();
		virtual ~Game();
};

#endif