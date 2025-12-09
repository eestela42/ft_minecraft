#include <classes/ECS/ECS.hpp>
#include <classes/Tools/Logger.hpp>
#include <glm/glm.hpp>

ECS::ECS(std::vector<std::vector<AChunk *>> &tabChunks, std::mutex &tabChunks_mutex,
		 glm::vec3 &playerPos, std::mutex &playerPos_mutex,
		 bool &endThread, std::mutex &endThread_mutex,
		 std::vector<unsigned char> **entityPos, std::mutex &entityPos_mutex)

	: tabChunks(tabChunks), tabChunks_mutex(tabChunks_mutex),
	  playerPos(playerPos), playerPos_mutex(playerPos_mutex),
	  endThread(endThread), endThread_mutex(endThread_mutex),
	  entityPos(entityPos), entityPos_mutex(entityPos_mutex)
{
	// systems.push_back(new SystemGetChunk());
	systems.push_back(new SystemIsOnGround());
	systems.push_back(new SystemChase());
	systems.push_back(new SystemGarvity());
	systems.push_back(new SystemMove());

	components.push_back(new Component(0, sizeof(glm::vec3)));
	components.push_back(new Component(1, sizeof(glm::vec3)));
	*entityPos = components[0]->getComponents();



	
}

void ECS::Initialize(int amount, Shader *entityShader,
					 std::vector<glm::mat4> &modelMatrices,
					 VertexArrayObject **model_VAO,
					 glm::vec3 **oldPos,
					 unsigned int &buffer)
{
	for (int i = 0; i < amount; i++)
		addEntity(playerPos.x + std::rand() % 500 - 250, playerPos.z + std::rand() % 500 - 250, 250 + std::rand() % 20);
	modelMatrices.resize(amount);

	// move to texture handler
	mesh *mesh42 = new mesh("object3d/cube.obj");

	t_vertexData dataStruct = {(u_char *)(mesh42->vertexes.data()), mesh42->vertexes.size() * (sizeof(glm::vec3) + 4 * sizeof(glm::vec4))};

	std::vector<unsigned int> *indices = new std::vector<unsigned int>();
	for (std::size_t i = 0; i < mesh42->triangles.size(); i++)
	{
		indices->push_back(mesh42->triangles[i].v[0]);
		indices->push_back(mesh42->triangles[i].v[1]);
		indices->push_back(mesh42->triangles[i].v[2]);
	}

	*model_VAO = new VertexArrayObject(new VertexBufferObject(dataStruct), new ElementBufferObject(*indices), entityShader);
	modelMatrices;
	entityPos_mutex.lock();
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		modelMatrices[i] = model;
	}

	*oldPos = new glm::vec3[amount];
	memcpy(*oldPos, (*entityPos)->data(), amount * sizeof(glm::vec3));

	entityPos_mutex.unlock();

	// configure instanced array

	buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	unsigned int VAO = (*model_VAO)->GetVAO();
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
}

ECS::~ECS()
{
}

void ECS::printAll()
{
	int i = 0;
	for (auto entity : entities)
	{
		Logger::info("Entity : " + std::to_string(i++));
		for (auto compo : components)
		{
			Logger::info("Component : " + std::to_string(i));
			std::vector<id> compo_id = entity.getComponents();
			bool found = false;
			for (auto id : compo_id)
			{
				if (id.type == compo->getId())
				{
					found = true;
					glm::vec3 *pos = (glm::vec3 *)compo->getComponent(id.value);
					Logger::info("value : " + std::to_string((*pos).x) + " " + std::to_string((*pos).y) + " " + std::to_string((*pos).z));
				}
			}
			if (!found)
				Logger::warn("not found");
			i++;
		}
	}
}

void ECS::addEntity()
{
	Entity entity;
	glm::vec3 tmp_pos(0, 0, 300);
	entity.addComponent(0, components[0]->addComponent(&tmp_pos));

	glm::vec3 tmp_mov(0, 0, 0);
	entity.addComponent(1, components[1]->addComponent(&tmp_mov));
	entities.push_back(entity);
}

void ECS::addEntity(int x, int y, int z)
{
	Entity entity;
	glm::vec3 tmp_pos(x, y, z);
	entity.addComponent(0, components[0]->addComponent(&tmp_pos));

	glm::vec3 tmp_mov(0, 0, 0);
	entity.addComponent(1, components[1]->addComponent(&tmp_mov));
	entities.push_back(entity);
}

void ECS::removeEntity(int index)
{
	// not working i guess
	entities.erase(entities.begin() + index);
}

std::vector<int> getDataOrder(std::bitset<8> flag_compo, std::bitset<8> flag_info)
{
	std::vector<int> order;
	for (int i = 0; i < 8; i++)
	{
		if (flag_compo.test(i))
			order.push_back(i);
	}
	for (int i = 0; i < 8; i++)
	{
		if (flag_info.test(i))
			order.push_back(-i - 1);
	}
	return order;
}

void ECS::update()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();

	int nbr = 0;
	int nbrTick = 20;
	int timeSleepToTick = 1000 / nbrTick;
	endThread_mutex.lock();
	while (!endThread)
	{
		endThread_mutex.unlock();
		if (std::chrono::steady_clock::now() - lastFrame < std::chrono::milliseconds(timeSleepToTick))
		{
			// sleep remainign time
			std::this_thread::sleep_for(std::chrono::milliseconds(timeSleepToTick) - (std::chrono::steady_clock::now() - lastFrame));
		}
		lastFrame = std::chrono::steady_clock::now();

		if (PROFILER_ON)
			Profiler::StartTracking("ECS cycle");
		nbr++;
		cycle();
		if (false && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() >= 1000)
		{
			Logger::info("ECS cycles : " + std::to_string(nbr));
			nbr = 0;
			begin = std::chrono::steady_clock::now();
		}
		if (PROFILER_ON)
			Profiler::StopTracking("ECS cycle");
		// return ;
		endThread_mutex.lock();
	}
	endThread_mutex.unlock();
}

void ECS::makeData(int i, std::vector<void *> &data, std::vector<int> &order, glm::vec3 &cp_playerPose,
				   std::vector<Component *> &vec_components, std::unordered_map<glm::ivec2, AChunk *, IVec2Hash, IVec2Equal> &chunks_needed)
{

	std::vector<id> compo_id = entities[i].getComponents();
	data.clear();

	data.push_back((void *)entities[i].getFlagInfoAddr()); // data[0] == flag_info
	for (int id : order)
	{
		if (id >= 0)
		{
			for (auto compo : vec_components)
			{
				if (id == compo->getId())
				{
					for (auto id1 : compo_id)
					{
						if (id1.type == compo->getId())
						{
							data.push_back(compo->getComponent(id1.value));
							break;
						}
					}
				}
			}
		}
		else
		{
			if (id == -1)
			{
				data.push_back((void *)&tabChunks);
				data.push_back((void *)&tabChunks_mutex);
			}
			if (id == -2)
				data.push_back((void *)&entityPos_mutex);
			if (id == -3)
				data.push_back((void *)&cp_playerPose);
			if (id == -4)
				data.push_back((void *)&chunks_needed);
		}
	}
}

void ECS::cycle()
{
	playerPos_mutex.lock();
	glm::vec3 cp_playerPose = playerPos;
	playerPos_mutex.unlock();

	std::vector<const char *> systemeNames = {"SystemGetChunk", "SystemIsOnGround", "SystemChase", "SystemGarvity", "SystemMove"};

	cp_playerPose = {cp_playerPose.x, cp_playerPose.z, cp_playerPose.y};

	int y = 0;
	for (auto system : systems)
	{
		if (PROFILER_ON)
			Profiler::StartTracking(systemeNames[y]);
		// Logger::info("system : " + std::to_string(y++));
		std::bitset<8> system_flag_compo = system->getFlagCompo();
		std::bitset<8> system_flag_info = system->getFlagInfo();

		for (int i = 0; i < entities.size(); i++)
		{
			std::vector<void *> data = std::vector<void *>();
			std::bitset<8> flag_compo;

			flag_compo = entities[i].getFlagCompo();

			if ((flag_compo & system_flag_compo) != system_flag_compo)
				continue;

			std::vector<int> order = getDataOrder(system_flag_compo, system_flag_info);

			makeData(i, data, order, cp_playerPose, components, chunks_needed);

			system->apply(data);
		}
		if (PROFILER_ON)
			Profiler::StopTracking(systemeNames[y]);
		y++;
	}
	// tabChunks_mutex.lock();
	// for (auto chunk : chunks_needed)
	// {
	// 	chunk.second->deleter();
	// 	chunk.second = NULL;
	// }
	// chunks_needed.clear();
	// tabChunks_mutex.unlock();
}
