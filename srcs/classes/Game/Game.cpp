#include <main.hpp>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>

glm::vec3 const Game::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

Game::~Game()
{

	for (size_t i = 0; i < ChunkGenerator::noiseList.size(); i++)
		delete ChunkGenerator::noiseList[i];

	delete crossHair;
	delete skyBox;
	delete instantiator;
	delete vertexArrayObjectHandler;
	delete shaderHandler;
	delete inputHandler;
	delete window;
}

Game::Game()
{
	window = new Window("ft_minecrafr", DrawMode::FILL);

	inputHandler = new InputHandler(window->GetWindow());
	inputHandler->AddCallback((I_Input *)this);
	inputHandler->AddCallback((I_Input *)window);

	shaderHandler = new ShaderHandler("shaders");

	vertexArrayObjectHandler = new VertexArrayObjectHandler();

	Chunk::setRenderDistance(renderDistance);

	instantiator = new ChunkInstantiator(vertexArrayObjectHandler, renderDistance, shaderHandler);

	TextureLoader textureLoader;
	if (!textureLoader.LoadTexture("textures/minecraft.png"))
		assert(!"Failed to load texture");
	blockTexture = textureLoader.GetTexture();

	if (!textureLoader.LoadTextureArray({
			std::filesystem::path("textures/texturePack/dirt.jpg"),
			std::filesystem::path("textures/texturePack/dirt.jpg"),
			std::filesystem::path("textures/texturePack/grass.jpg"),
			std::filesystem::path("textures/texturePack/stone.jpg"),
			std::filesystem::path("textures/texturePack/sand.jpg"),
			std::filesystem::path("textures/texturePack/oak_wood_side.jpg"),
			std::filesystem::path("textures/texturePack/leaves_2.jpg"),
			std::filesystem::path("textures/texturePack/bedrock.jpg"),
			std::filesystem::path("textures/texturePack/water.jpg"),
			std::filesystem::path("textures/texturePack/snow.jpg"),
			std::filesystem::path("textures/texturePack/iron_ore.jpg"),
			std::filesystem::path("textures/texturePack/gold_ore.jpg"),
			std::filesystem::path("textures/texturePack/diamond_ore.jpg"),
			std::filesystem::path("textures/texturePack/UNKNOWN.jpg"),
		}))
		assert(!"Failed to load texture array");
	blockTextureArray = textureLoader.GetTextureArray();

	skyBox = new SkyBox(shaderHandler->GetShader("cubemap"));
	crossHair = new CrossHair(shaderHandler->GetShader("CrossHair"));
	chunkStableState = new std::vector<Chunk *>();
}

void print()
{
	while (true)
	{
		std::cout << "hello" << std::endl;
	}
}

void Game::StartLoop()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	u_int fps = 0;

	bool info = true;
	//lancer le thread de generation


	auto boundUpdateFunction = std::bind(&ChunkInstantiator::Update, this->instantiator,
	&cameraPosition, &chunkStableState, std::ref(cameraMutex), std::ref(chunkStableMutex));


	// Create the thread with the bound function
	std::thread updateThread(boundUpdateFunction);


	while (window->ShouldContinue())
	{
		fps++;
		Loop();
		if (info && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() >= 500)
		{
			// std::cout << "chunk : " << (int)cameraPosition.x / Chunk::sizeX << " " << (int)cameraPosition.z / Chunk::sizeY << " " << (int)cameraPosition.y << std::endl;
			// std::cout << "pos : " << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
			std::cout << "FPS: " << fps * 2 << std::endl;
			fps = 0;
			begin = std::chrono::steady_clock::now();
		}
	}
	updateThread.join();
}

void Game::Loop()
{
	chunkStableMutex.lock();

	inputHandler->HandleInput();
	window->Clear();
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0), cameraDirection, cameraUp);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH / (float)DEFAULT_WINDOW_HEIGHT, 0.1f, 16000.0f);
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = proj * view;
	if (Shader::GetActiveShader())
	{
		Shader::GetActiveShader()->SetFloat4("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);
		Shader::GetActiveShader()->Setmat4("matrix", matrix);
		Shader::GetActiveShader()->SetInt("chunk_size_x", Chunk::sizeX);
		Shader::GetActiveShader()->SetInt("chunk_size_y", Chunk::sizeY);
		Shader::GetActiveShader()->SetInt("TextureArraySize", 6);
	}
	else
	{
		assert(!"The loop is running and there are no active shader");
	}

	skyBox->drawSkybox(proj * glm::mat4(glm::mat3(view)), cameraPosition);

	crossHair->draw();

	glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray.id);

	// vertexArrayObjectHandler->DrawAll(drawType);
	for (auto const x : *this->chunkStableState)
	{
		// x->Draw();
	}

	window->SwapBuffersAndPollEvents();

	chunkStableMutex.unlock();

	vertexArrayObjectHandler->mutex_toAdd.lock();
	for (auto const &x : vertexArrayObjectHandler->toAdd)
	{
		VertexArrayObject *VAO = new VertexArrayObject(new VertexBufferObject(x.vertexData), new ElementBufferObject(x.indices), x.shader);
		instantiator->chunkMap[(Chunk*)x.ptr] = vertexArrayObjectHandler->AddVAO(VAO);
	}
	vertexArrayObjectHandler->toAdd.clear();
	vertexArrayObjectHandler->mutex_toAdd.unlock();

	vertexArrayObjectHandler->mutex_toDelete.lock();
	for (auto const &x : vertexArrayObjectHandler->toDelete)
	{
		vertexArrayObjectHandler->RemoveVAO(x);
	}
	vertexArrayObjectHandler->toDelete.clear();
	vertexArrayObjectHandler->mutex_toDelete.unlock();

}

void Game::SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY)
{
	cameraMutex.lock();
	float speedMultiplier = (keyState[KEY_MOVE_UPWARD] & KEY_HOLD) ? 20 : 1;
	if (keyState[KEY_MOVE_FORWARD] & KEY_HOLD)
		cameraPosition += speed * speedMultiplier * cameraDirection;
	if (keyState[KEY_MOVE_BACKWARD] & KEY_HOLD)
		cameraPosition -= speed * speedMultiplier * cameraDirection;
	if (keyState[KEY_MOVE_RIGHTWARD] & KEY_HOLD)
		cameraPosition += speed * speedMultiplier * glm::normalize(glm::cross(cameraDirection, cameraUp));
	if (keyState[KEY_MOVE_LEFTWARD] & KEY_HOLD)
		cameraPosition -= speed * speedMultiplier * glm::normalize(glm::cross(cameraDirection, cameraUp));
	if (keyState[KEY_SPACE] & KEY_HOLD)
		cameraPosition += glm::vec3(0, speed * speedMultiplier, 0);
	if (keyState[KEY_MOVE_DOWNWARD] & KEY_HOLD)
		cameraPosition += glm::vec3(0, -speed * speedMultiplier, 0);

	// if(keyState[KEY_DELETE_ONE_BLOCK] & KEY_PRESS)
	//     deleteBlock();
	// if(keyState[KEY_DELETE_MORE_BLOCK] & KEY_HOLD)
	//     deleteBlock();
	if (keyState[KEY_DISPLAY_INFO] & KEY_PRESS)
	{
		std::cout << "chunk : " << (int)cameraPosition.x / Chunk::sizeX << " " << (int)cameraPosition.z / Chunk::sizeY << " " << (int)cameraPosition.y << std::endl;
		std::cout << "pos : " << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << std::endl;
	}

	yaw += mouseMoveX * sensitivity;
	pitch -= mouseMoveY * sensitivity;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	view = glm::lookAt(cameraPosition,
					   cameraPosition + glm::normalize(cameraDirection),
					   cameraUp);
	cameraMutex.unlock();
}

int Game::GetRenderDistance() const
{
	return renderDistance;
}

int Game::GetChunkLoadingSize() const
{
	return chunkLoadingSize;
}

glm::mat4 Game::GetCameraView() const
{
	return view;
}
