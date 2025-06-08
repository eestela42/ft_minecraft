#include <classes/Game/Game.hpp>
#include <classes/Game/mesh.hpp>
#include <algorithm>
#include "../../../imgui/imgui.h"
#include "../../../imgui/backends/imgui_impl_glfw.h"
#include "../../../imgui/backends/imgui_impl_opengl3.h"

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
													endThreads, endThreads_mutex,
													casse_block,
													displayDistance);


	ecs = new ECS(chunkInstanciator->getTabChunks(), chunkInstanciator->getTabChunks_mutex(),
					cameraPosition, playerPos_mutex,
					endThreads, endThreads_mutex,
					&entityPos, entityPos_mutex);



	for (int i = 0; i < amount; i++)
		ecs->addEntity(cameraPosition.x + std::rand() % 500 - 250,cameraPosition.z + std::rand() % 500 - 250, 250 + std::rand() % 20);
	
	modelMatrices.resize(amount);


	mesh *mesh42 = new mesh("object3d/cube.obj");

	t_vertexData dataStruct = {(u_char*)(mesh42->vertexes.data()), mesh42->vertexes.size() * (sizeof(glm::vec3) + 4 * sizeof(glm::vec4))};

	std::vector<unsigned int>* indices = new std::vector<unsigned int>();
	for (std::size_t i = 0; i < mesh42->triangles.size(); i++)
	{
		indices->push_back(mesh42->triangles[i].v[0]);
		indices->push_back(mesh42->triangles[i].v[1]);
		indices->push_back(mesh42->triangles[i].v[2]);
	}

	model_VAO = new VertexArrayObject(new VertexBufferObject(dataStruct), new ElementBufferObject(*indices), shaderHandler->GetShader("entity"));
	modelMatrices;
	entityPos_mutex.lock();
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		modelMatrices[i] = model;
    }
	

	oldPos = new glm::vec3[amount];
	memcpy(oldPos, entityPos->data(), amount * sizeof(glm::vec3));

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

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark(); // Optional

	const GLubyte* version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (version) {
        std::cout << "GLSL version: " << version << std::endl;
    } else {
        std::cout << "Failed to get GLSL version (no GL context?)" << std::endl;
    }

    ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

	seedUI = chunkInstanciator->getCurrentSeed();


	// configure compute shader

    glGenFramebuffers(1, &frontGroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, frontGroundFBO);

    glGenTextures(1, &frontColorTexture);
    glBindTexture(GL_TEXTURE_2D, frontColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frontColorTexture, 0);

    glGenTextures(1, &frontDepthTexture);
    glBindTexture(GL_TEXTURE_2D, frontDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,  DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frontDepthTexture, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup backgroundFBO (backColorTexture, backDepthTexture)
    glGenFramebuffers(1, &backgroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFBO);

    glGenTextures(1, &backColorTexture);
    glBindTexture(GL_TEXTURE_2D, backColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backColorTexture, 0);

    glGenTextures(1, &backDepthTexture);
    glBindTexture(GL_TEXTURE_2D, backDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,  DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, backDepthTexture, 0);

    glDrawBuffers(1, drawBuffers);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "backgroundFBO incomplete: " << status << std::endl;
    } else {
        std::cout << "backgroundFBO complete!" << std::endl;
    }

	glGenFramebuffers(1, &outFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, outFBO);

    glGenTextures(1, &outTexture);
    glBindTexture(GL_TEXTURE_2D, outTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,  DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTexture, 0);


    glDrawBuffers(1, drawBuffers);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "outFBO incomplete: " << status << std::endl;
    } else {
        std::cout << "outFBO complete!" << std::endl;
    }


    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


	shaderHandler->Use("compute");
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);


	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	// std::cout << "Max work group sizes" <<
	// 	" x:" << work_grp_size[0] <<
	// 	" y:" << work_grp_size[1] <<
	// 	" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	// std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";
	
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
	
	static auto lastTime = std::chrono::steady_clock::now();

	while(window->ShouldContinue())
	{
		fps++;
		Loop();

		//fps
		auto now = std::chrono::steady_clock::now();
		float deltaSeconds = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;

		fpsCounter = 1.0f / deltaSeconds;
	}

	endThreads_mutex.lock();
	endThreads = true;
	endThreads_mutex.unlock();
	
	playerHasMoved_mutex.lock();
	playerHasMoved = true;
	playerHasMoved_mutex.unlock();

	std::cout << "Joining threads..." << std::endl;
	chunkThread.join();
	std::cout << "Chunk thread joined." << std::endl;
	ecsThread.join();
}

void Game::manageUI()
{
	/*
		Render distance
		Seed
		Speed
		Number of Entities
		Chunk Type
		
	
	
	*/
	ImGui::Begin("This is nice UI", nullptr, ImGuiWindowFlags_AlwaysAutoResize);;

	ImGui::Text("FPS: %d", fpsCounter);
	ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	ImGui::Text("Seed: %ld", chunkInstanciator->getCurrentSeed());
	ImGui::Text("Entities: %d", amount);

	if (ImGui::CollapsingHeader("Actions"))
	{
		if (ImGui::Button("Unload all chunks"))
		chunkInstanciator->deleteAllChunks();
		
		
		
		ImGui::DragScalar("Seed", ImGuiDataType_S64, &seedUI, 0.1f);
		ImGui::Checkbox("Instant update seed", &instantChange);
		if (instantChange || ImGui::Button("Change seed"))
		{
			chunkInstanciator->changeSeed(seedUI);
		}

		ImGui::DragInt("Render distance", &renderDistance, 0.1f, 1, 100);
		if (ImGui::Button("Change render distance"))
		{
			currentDisplayDistance = 0.f;
			chunkInstanciator->changeRenderDistance(renderDistance);
		}
		
		
		if (ImGui::Button("Switch cursors mode"))
		window->switchCursorMode();
	}

	
	ImGui::Text("Toogle free mouse: Press left.ALT");
	if (ImGui::CollapsingHeader("Inputs"))
	{
		ImGui::Text("WASD: Move");
		ImGui::Text("Space: Go up");
		ImGui::Text("Left Control: Go down");
		ImGui::Text("Left Shift: Sprint");
		ImGui::Text("Left ALT: Toggle free mouse");
	}


	ImGui::End();
}

void Game::drawUI()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::Loop() {
	window->Clear();

	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Your UI
	manageUI();

	// Render
	ImGui::Render();

	
	inputHandler->HandleInput();
	
	glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraUp);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH/(float)DEFAULT_WINDOW_HEIGHT, 0.1f, 16000.0f);
	glm::mat4 matrix = glm::mat4(1.0f);
	matrix = proj * view;
	
	glBindFramebuffer(GL_FRAMEBUFFER, backgroundFBO);
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	skyBox->drawSkybox(view, proj, cameraPosition);

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, frontGroundFBO);
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
	
	

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

	glBindFramebuffer(GL_FRAMEBUFFER, outFBO);
    glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	currentDisplayDistance += (float)renderDistance/360.f * 16;
	
	int startFog = currentDisplayDistance;
	sizeFog = 100;
	
	
	

	
	shaderHandler->Use("compute");
	if (Shader::GetActiveShader())
	{
		Shader::GetActiveShader()->SetTexture("frontTex", frontColorTexture, GL_TEXTURE0);
		Shader::GetActiveShader()->SetTexture("depthTex", frontDepthTexture, GL_TEXTURE1);
		Shader::GetActiveShader()->SetTexture("backTex", backColorTexture, GL_TEXTURE2);
		Shader::GetActiveShader()->SetInt("startFog", startFog);
		Shader::GetActiveShader()->SetInt("endFog", startFog + sizeFog);
	}

	glBindImageTexture(0, outTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute((DEFAULT_WINDOW_WIDTH + 15) / 16, (DEFAULT_WINDOW_HEIGHT + 15) / 16, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, outFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	drawUI();
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

void interpolatePositions(const glm::vec3* oldPos, 
	const glm::vec3* newPos, 
	std::vector<glm::vec3>& interpolatedPos, 
	float alpha,
	int amount) {
	// Ensure vectors have the same size
	// if (!oldPos || !newPos || oldPos->size() != newPos->size()) return;

	// Resize output vector
	interpolatedPos.resize(amount);

	// Perform interpolation
	for (size_t i = 0; i < amount; i++) {
		interpolatedPos[i] = glm::mix((oldPos)[i], (newPos)[i], alpha);
}
}

void Game::manageVaoEntity()
{
	std::vector<glm::mat4> usedModelMatrices(amount);
	entityPos_mutex.lock();
	glm::vec3 *newPos = (glm::vec3*)entityPos->data();

	// float alpha = (currentTime - lastTickTime) / tickInterval;

	std::vector<glm::vec3> interpolatedPos;
	interpolatePositions(oldPos, newPos, interpolatedPos, 0.5f, amount);
	memcpy(oldPos, newPos, amount * sizeof(glm::vec3));

	for (unsigned int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 pos = interpolatedPos[i];

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
	if (keyState[KEY_SWITCH_CURSOR_MODE] & KEY_PRESS)
	{
		window->switchCursorMode();
	}

	
	playerPos_mutex.unlock();
	
	if (oldCamPos.x / AChunk::sizeX != cameraPosition.x / AChunk::sizeX || oldCamPos.z / AChunk::sizeY != cameraPosition.z / AChunk::sizeY)
	{
		playerHasMoved_mutex.lock();
		playerHasMoved = true;
		playerHasMoved_mutex.unlock();
	}


	// if (window->getCursorMode() == GLFW_CURSOR_DISABLED)
	// {

	// 	yaw += mouseMoveX * sensitivity;
	// 	pitch -= mouseMoveY * sensitivity;
	// 	if(pitch > 89.0f)
	// 		pitch =  89.0f;
	// 	if(pitch < -89.0f)
	// 		pitch = -89.0f;
	// }

	double xpos, ypos;
	glfwGetCursorPos(window->GetWindow(), &xpos, &ypos);


	if (window->getCursorMode() == GLFW_CURSOR_DISABLED)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		mouseMoveX = xpos - lastX;
		mouseMoveY = ypos - lastY;

		lastX = xpos;
		lastY = ypos;

		yaw += mouseMoveX * sensitivity;
		pitch -= mouseMoveY * sensitivity;
		pitch = glm::clamp(pitch, -89.0f, 89.0f);
	}
	else
	{
		firstMouse = true;  // Reset when leaving FPS mode
	}
	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	view = glm::lookAt(	cameraPosition, 
						cameraPosition + glm::normalize(cameraDirection),
						cameraUp);
						


}