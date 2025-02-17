#include <classes/Game/Game.hpp>

Game::Game()
{
	std::cout << "start game constructor" << std::endl;

	window = new Window("Minecraft", DrawMode::FILL);
	inputHandler = new InputHandler(window->GetWindow());
	inputHandler->AddCallback((I_Input*)this); // c'est quoi c'truc
	inputHandler->AddCallback((I_Input*)window);
	shaderHandler = new ShaderHandler("shaders");

	ASystem *systemGravity = new SystemGarvity();
	std::vector<ASystem*> systems = {systemGravity};
	ecs = new ECS();
	ecs->addEntity();
	ecs->addEntity();


	vertexArrayObjectHandler = new VertexArrayObjectHandler();


	std::cout << "render distance : " << renderDistance << std::endl;
	chunkInstanciator = new ChunkInstanciator(renderDistance, cameraPosition, playerPos_mutex,
													dequeueVAO, dequeueVAO_mutex,
													dequeueDeleteVAO, dequeueDeleteVAO_mutex,
													playerHasMoved, playerHasMoved_mutex);
	blockTextureArray = TextureLoader::LoadTextureArray(
	{
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
	});

	VertexArrayObjectHandler *vertexArrayObjectHandler = new VertexArrayObjectHandler();

	skyBox = new SkyBox(shaderHandler->GetShader("cubemap"));
	std::cout << "end game constructor" << std::endl;

	
}
Game::~Game() {
	std::cout << "start game destructor" << std::endl;
	delete window;
	delete inputHandler;
	delete shaderHandler;
	std::cout << "end game destructor" << std::endl;
}

void Game::StartLoop() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	u_int fps = 0;

	//lancer le thread de generation de chunks
	std::thread chunkThread(&ChunkInstanciator::update, chunkInstanciator);
	
	int i = 0;
	while(window->ShouldContinue())
	{
		fps++;
		Loop();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() >= 500)
		{
			std::cout << "FPS: " << fps * 2 << std::endl;
			fps = 0;
			begin = std::chrono::steady_clock::now();
		}
	}
}

void Game::Loop() {
	inputHandler->HandleInput();

	// ecs->cycle();

	window->Clear();

	glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH/(float)DEFAULT_WINDOW_HEIGHT, 0.1f, 16000.0f);
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = proj * view;


	skyBox->drawSkybox(view, proj, cameraPosition);

	shaderHandler->Use("RLE-Geometry");
	if (Shader::GetActiveShader()) {
		Shader::GetActiveShader()->SetFloat4("cameraPos", cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);
		Shader::GetActiveShader()->Setmat4("matrix", matrix);
		Shader::GetActiveShader()->SetInt("chunk_size_x", AChunk::sizeX);
		Shader::GetActiveShader()->SetInt("chunk_size_y", AChunk::sizeY);
		Shader::GetActiveShader()->SetInt("TextureArraySize", 6);
	}


	manageVAO();

	draw();

	window->SwapBuffersAndPollEvents();


}

void Game::draw()
{
	vertexArrayObjectHandler->DrawAll(1);

}

void Game::manageVAO()
{
	int i = 0;

	i = 0;
	dequeueDeleteVAO_mutex.lock();
	while (!dequeueDeleteVAO.empty())
	{
		i++;
		std::pair<int, int> pos = std::make_pair(dequeueDeleteVAO.front().x, dequeueDeleteVAO.front().y);
		dequeueDeleteVAO.pop_front();
		vertexArrayObjectHandler->RemoveVAO(pos_to_vao[pos].VAO);
		pos_to_vao.erase(pos);
	}
	dequeueDeleteVAO_mutex.unlock();
	vao_counter -= i;

	i = 0;
	dequeueVAO_mutex.lock();
	while (!dequeueVAO.empty())
	{
		i++;
		info_VAO *info = dequeueVAO.front();
		dequeueVAO.pop_front();

		t_vertexData dataStruct = {(u_char*)(info->vertices.data), info->vertices.size};
		VertexArrayObject *VAO = new VertexArrayObject(new VertexBufferObject(dataStruct), new ElementBufferObject(*(info->indices)), shaderHandler->GetShader("RLE-Geometry"));
		
		u_int VAO_id = vertexArrayObjectHandler->AddVAO(VAO);

		pos_to_vao.insert({std::make_pair(info->pos.x, info->pos.y), {info->vertices.data, info->indices, VAO_id}});
	}
	dequeueVAO_mutex.unlock();
	vao_counter += i;
}

void Game::SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY) {
	float speedMultiplier = (keyState[KEY_MOVE_UPWARD] & KEY_HOLD) ? 20 : 1;

	glm::vec3 oldCamPos = cameraPosition;

	playerPos_mutex.lock();
	if(keyState[KEY_MOVE_FORWARD] & KEY_HOLD)
  		cameraPosition += speed * speedMultiplier * cameraDirection;
	if(keyState[KEY_MOVE_BACKWARD] & KEY_HOLD)
			cameraPosition -= speed * speedMultiplier * cameraDirection;
	if(keyState[KEY_MOVE_RIGHTWARD] & KEY_HOLD)
			cameraPosition += speed * speedMultiplier * glm::normalize(glm::cross(cameraDirection, cameraUp));
	if(keyState[KEY_MOVE_LEFTWARD] & KEY_HOLD)
	{
			cameraPosition -= speed * speedMultiplier * glm::normalize(glm::cross(cameraDirection, cameraUp));
	}
	if(keyState[KEY_SPACE] & KEY_HOLD)
			cameraPosition += glm::vec3(0, speed * speedMultiplier, 0);
	if(keyState[KEY_MOVE_DOWNWARD] & KEY_HOLD)
			cameraPosition += glm::vec3(0, -speed * speedMultiplier, 0);
	playerPos_mutex.unlock();

	yaw += mouseMoveX * sensitivity;
	pitch -= mouseMoveY * sensitivity;
	if(pitch > 89.0f)
		pitch =  89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	if (oldCamPos.x / AChunk::sizeX != cameraPosition.x / AChunk::sizeX || oldCamPos.z / AChunk::sizeY != cameraPosition.z / AChunk::sizeY)
	{
		// std::cout << "player has moved" << std::endl;
		playerHasMoved_mutex.lock();
		playerHasMoved = true;
		playerHasMoved_mutex.unlock();
	}

	view = glm::lookAt(	cameraPosition, 
						cameraPosition + glm::normalize(cameraDirection),
						cameraUp);

}