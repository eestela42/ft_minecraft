#include <classes/Game/Game.hpp>
#include <classes/Game/mesh.hpp>

unsigned int depthMapFBO;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

unsigned int depthMap;


Game::Game()
{
	window = new Window("Minecraft", DrawMode::FILL);
	inputHandler = new InputHandler(window->GetWindow());
	inputHandler->AddCallback((I_Input*)this); // c'est quoi c'truc
	inputHandler->AddCallback((I_Input*)window);
	shaderHandler = new ShaderHandler("shaders");

	vertexArrayObjectHandler = new VertexArrayObjectHandler();

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

	skyBox = new SkyBox(shaderHandler->GetShader("cubemap"));

	chunkInstanciator = new ChunkInstanciator(renderDistance, cameraPosition, playerPos_mutex,
													dequeueVAO, dequeueVAO_mutex,
													dequeueDeleteVAO, dequeueDeleteVAO_mutex,
													playerHasMoved, playerHasMoved_mutex,
													endThreads, endThreads_mutex);


	ecs = new ECS(chunkInstanciator->getTabChunks(), chunkInstanciator->getTabChunks_mutex(),
					cameraPosition, playerPos_mutex,
					endThreads, endThreads_mutex,
					&entityPos, entityPos_mutex);



	for (int i = 0; i < amount; i++)
		ecs->addEntity(std::rand() % 500 - 250, std::rand() % 500 - 250, 255 + std::rand() % 50);


	mesh *mesh42 = new mesh("object3d/sheep.obj");

	t_vertexData dataStruct = {(u_char*)(mesh42->vertexes.data()), mesh42->vertexes.size() * (sizeof(glm::vec3) + 4 * sizeof(glm::vec4))};

	std::vector<unsigned int>* indices = new std::vector<unsigned int>();
	for (std::size_t i = 0; i < mesh42->triangles.size(); i++)
	{
		indices->push_back(mesh42->triangles[i].v[0]);
		indices->push_back(mesh42->triangles[i].v[1]);
		indices->push_back(mesh42->triangles[i].v[2]);
	}

	model_VAO = new VertexArrayObject(new VertexBufferObject(dataStruct), new ElementBufferObject(*indices), shaderHandler->GetShader("entity"));
	glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
	entityPos_mutex.lock();
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		modelMatrices[i] = model;
    }
	entityPos_mutex.unlock();

    // configure instanced array
	
    buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	unsigned int VAO = model_VAO->GetVAO();
	glBindVertexArray(VAO);

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

	endThreads_mutex.lock();
	endThreads = true;
	endThreads_mutex.unlock();
	
	playerHasMoved_mutex.lock();
	playerHasMoved = true;
	playerHasMoved_mutex.unlock();

	chunkThread.join();
	ecsThread.join();
}

void Game::Loop() {
	window->Clear();

	inputHandler->HandleInput();

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

	manageVaoEntity();
	drawEntity();
	

	window->SwapBuffersAndPollEvents();
}

void Game::draw()
{
	vertexArrayObjectHandler->DrawAll(1);

}

void Game::drawEntity()
{
	glBindVertexArray(model_VAO->GetVAO());
	
	glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(model_VAO->GetEBO()->GetSize()), GL_UNSIGNED_INT, 0, amount);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Game::manageVaoEntity()
{
	std::vector<glm::mat4> modelMatrices(amount);
	entityPos_mutex.lock();
	glm::vec3 *data = (glm::vec3*)entityPos->data();

	for (unsigned int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 pos = data[i];
		model = glm::translate(model, glm::vec3(pos.x, pos.z, pos.y));
		modelMatrices[i] = model;
	}
	entityPos_mutex.unlock();
	unsigned int VAO = model_VAO->GetVAO();
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	
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

//for the stream, auto-move
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
	

	if (keyState[KEY_DELETE_ONE_BLOCK] & KEY_PRESS)
		Gogogo = !Gogogo;
	if (Gogogo)
	{
		glm::vec3 flatDrirection = cameraDirection;
		flatDrirection.y = 0;
		cameraPosition += 2 * speed * speedMultiplier * flatDrirection;
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
		playerHasMoved_mutex.lock();
		playerHasMoved = true;
		playerHasMoved_mutex.unlock();
	}

	view = glm::lookAt(	cameraPosition, 
						cameraPosition + glm::normalize(cameraDirection),
						cameraUp);
}