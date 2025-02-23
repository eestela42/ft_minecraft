#include <classes/Game/Game.hpp>
#include <classes/Game/mesh.hpp>




Game::Game()
{
	std::cout << "start game constructor" << std::endl;

	window = new Window("Minecraft", DrawMode::FILL);
	inputHandler = new InputHandler(window->GetWindow());
	inputHandler->AddCallback((I_Input*)this); // c'est quoi c'truc
	inputHandler->AddCallback((I_Input*)window);
	shaderHandler = new ShaderHandler("shaders");




	vertexArrayObjectHandler = new VertexArrayObjectHandler();


	std::cout << "render distance : " << renderDistance << std::endl;
	chunkInstanciator = new ChunkInstanciator(renderDistance, cameraPosition, playerPos_mutex,
													dequeueVAO, dequeueVAO_mutex,
													dequeueDeleteVAO, dequeueDeleteVAO_mutex,
													playerHasMoved, playerHasMoved_mutex,
													endChunkInstanciator, endChunkInstanciator_mutex);

	ecs = new ECS(chunkInstanciator->getTabChunks(), chunkInstanciator->getTabChunks_mutex(),
					endChunkInstanciator, endChunkInstanciator_mutex,
					dequeueVAO, dequeueVAO_mutex,
					dequeueDeleteVAO, dequeueDeleteVAO_mutex);
	for (int i = 0; i < 1000; i++)
		ecs->addEntity(std::rand() % 100, std::rand() % 100, std::rand() % 10000);
	// ecs->addEntity();
	// ecs->addEntity();
	// ecs->addEntity();

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




	mesh *mesh42 = new mesh("object3d/42.obj");

	t_vertexData dataStruct = {(u_char*)(mesh42->vertexes.data()), mesh42->vertexes.size() * (sizeof(glm::vec3) + 4 * sizeof(glm::vec4))};

	std::vector<unsigned int>* indices = new std::vector<unsigned int>();
	for (std::size_t i = 0; i < mesh42->triangles.size(); i++)
	{
		indices->push_back(mesh42->triangles[i].v[0]);
		indices->push_back(mesh42->triangles[i].v[1]);
		indices->push_back(mesh42->triangles[i].v[2]);
	}
	// std::vector<unsigned int>* indices = (std::vector<unsigned int>*)&(mesh42->triangles);
	
	model_VAO = new VertexArrayObject(new VertexBufferObject(dataStruct), new ElementBufferObject(*indices), shaderHandler->GetShader("entity"));

	
	glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 150.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	unsigned int VAO = model_VAO->GetVAO();
	glBindVertexArray(VAO);
	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
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
	std::thread ecsThread(&ECS::update, ecs);
	
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
	endChunkInstanciator_mutex.lock();
	endChunkInstanciator = true;
	endChunkInstanciator_mutex.unlock();
	
	playerHasMoved_mutex.lock();
	playerHasMoved = true;
	playerHasMoved_mutex.unlock();

	chunkThread.join();
	ecsThread.join();
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

	shaderHandler->Use("entity");
	if (Shader::GetActiveShader()) {
		Shader::GetActiveShader()->Setmat4("view", view);
		Shader::GetActiveShader()->Setmat4("proj", proj);
		Shader::GetActiveShader()->Setmat4("model", matrix);
		Shader::GetActiveShader()->SetInt("TextureArraySize", 6);
	}

	glBindVertexArray(model_VAO->GetVAO());
	
	glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(model_VAO->GetEBO()->GetSize()), GL_UNSIGNED_INT, 0, amount);
	// glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(model_VAO->GetEBO()->GetSize()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

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

bool Gogogo = false;

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
	if (Gogogo || keyState[KEY_DELETE_ONE_BLOCK] & KEY_PRESS)
	{
		Gogogo = true;
		glm::vec3 flatDrirection = cameraDirection;
		flatDrirection.y = 0;
		cameraPosition += 2 * speed * speedMultiplier * flatDrirection;
		// cameraPosition += speed * 0.5f * speedMultiplier * glm::normalize(glm::cross(cameraDirection, cameraUp));
		mouseMoveX += 0.5;
	}
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