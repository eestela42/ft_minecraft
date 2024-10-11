#include <classes/Game/Game.hpp>

Game::Game() {
	window = new Window("Minecraft", DrawMode::FILL);
	inputHandler = new InputHandler(window->GetWindow());
	inputHandler->AddCallback((I_Input*)this); // c'est quoi c'truc
	inputHandler->AddCallback((I_Input*)window);
	shaderHandler = new ShaderHandler("shaders");

	
	vertexArrayObjectHandler = new VertexArrayObjectHandler();


	std::cout << "render distance : " << renderDistance << std::endl;
	chunkInstanciator = new ChunkInstanciator(renderDistance, cameraPosition, playerPos_mutex,
													dequeueVAO, dequeueVAO_mutex,
													dequeueDeleteVAO, dequeueDeleteVAO_mutex);
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

	
}
Game::~Game() {
	delete window;
	delete inputHandler;
	delete shaderHandler;
}

void Game::StartLoop() {
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	u_int fps = 0;

	//lancer le thread de generation de chunks
	std::thread chunkThread(&ChunkInstanciator::update, chunkInstanciator);
		
	while(window->ShouldContinue())
	{
		Loop();
	}
}

void Game::Loop() {
	inputHandler->HandleInput();
	window->Clear();

	glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH/(float)DEFAULT_WINDOW_HEIGHT, 0.1f, 16000.0f);
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = proj * view;
	skyBox->drawSkybox(view, proj, cameraPosition);

	shaderHandler->Use("default");
	Shader::GetActiveShader()->Setmat4("matrix", matrix);
	Shader::GetActiveShader()->SetFloat4("color", 0.6, 0.32, 0.84, 0);


	manageVAO();
	
	
	draw();
	// vertexArrayObjectHandler->DrawAll(0);

	// chunks[0]->draw();

	window->SwapBuffersAndPollEvents();

}

void Game::draw()
{
	// chunks[0]->draw();
	for (auto const& x : Vao_draw)
	{
		if (map_VAO.find(std::make_pair(x.pos.x, x.pos.y)) == map_VAO.end())
			continue ;
		glBindVertexArray(x.VAO);
		glDrawElements(GL_TRIANGLES, x.indices_size	, GL_UNSIGNED_INT, 0);
	}

}

void Game::manageVAO()
{
	dequeueVAO_mutex.lock();
	while (!dequeueVAO.empty())
	{
		// t_vertexData vbo_data = {(u_char*)info->vertices->data(), info->vertices->size() * sizeof(Vertex)};
		
		info_VAO *info = dequeueVAO.front();
		dequeueVAO.pop_front();

		u_int VBO, EBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);


		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
		glBufferData(GL_ARRAY_BUFFER, info->vertices->size() * sizeof(Vertex), &(*info->vertices)[0], GL_STATIC_DRAW);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, info->indices->size() * sizeof(unsigned int), &(*info->indices)[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, light));
		glBindVertexArray(0);
		map_VAO[std::make_pair(info->pos.x, info->pos.y)] = VAO;
		Vao_draw.push_back({VAO, info->indices->size(), info->pos});
	}
	dequeueVAO_mutex.unlock();

	dequeueDeleteVAO_mutex.lock();
	while (!dequeueDeleteVAO.empty())
	{
		std::pair<int, int> pos = std::make_pair(dequeueDeleteVAO.front().x, dequeueDeleteVAO.front().y);
		dequeueDeleteVAO.pop_front();
		if (map_VAO.find(pos) != map_VAO.end())
		{
			glDeleteVertexArrays(1, &map_VAO[pos]);
			map_VAO.erase(pos);
		}

	}
	dequeueDeleteVAO_mutex.unlock();

}

void Game::SendKeys(u_char *keyState, double mouseMoveX, double mouseMoveY) {
	float speedMultiplier = (keyState[KEY_MOVE_UPWARD] & KEY_HOLD) ? 20 : 1;
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

	view = glm::lookAt(	cameraPosition, 
						cameraPosition + glm::normalize(cameraDirection),
						cameraUp);

}