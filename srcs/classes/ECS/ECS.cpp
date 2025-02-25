#include <classes/ECS/ECS.hpp>
#include <glm/glm.hpp>



ECS::ECS(std::vector<std::vector<AChunk*>> 	&tabChunks, std::mutex &tabChunks_mutex,
			glm::vec3 &playerPos, std::mutex &playerPos_mutex,
			bool &endThread, std::mutex &endThread_mutex,
			std::vector<unsigned char> **entityPos, std::mutex &entityPos_mutex)

		: tabChunks(tabChunks), 	tabChunks_mutex(tabChunks_mutex),
		playerPos(playerPos), 		playerPos_mutex(playerPos_mutex),
		endThread(endThread), 		endThread_mutex(endThread_mutex),
		entityPos(entityPos), 		entityPos_mutex(entityPos_mutex)
{
	// systems.push_back(new SystemGetListChunk());
	// systems.push_back(new SystemGetChunk());
	systems.push_back(new SystemIsOnGround());
	systems.push_back(new SystemChase());
	systems.push_back(new SystemGarvity());
	systems.push_back(new SystemMove());	

	components.push_back(new Component(0, sizeof(glm::vec3)));
	components.push_back(new Component(1, sizeof(glm::vec3)));
	*entityPos = components[0]->getComponents();

}

ECS::~ECS()
{
}

void ECS::printAll()
{
	int i = 0;
	for (auto entity : entities)
	{
		std::cout << "Entity : " << i++ << std::endl;
		for (auto compo : components)
		{
			std::cout << "Component : " << i << std::endl;
			std::vector<id> compo_id = entity.getComponents();
			bool found = false;
			for (auto id : compo_id)
			{
				if (id.type == compo->getId())
				{
					found = true;
					glm::vec3 *pos = (glm::vec3*)compo->getComponent(id.value);
					std::cout << "value : " << (*pos).x << " "  << (*pos).y << " " << (*pos).z << std::endl;
				}
			}
			if (!found)
				std::cout << "not found" << std::endl;
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
	//not working i guess
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
	endThread_mutex.lock();
	int nbrTick = 20;
	int timeSleepToTick = 1000 / nbrTick;
	while (!endThread)
	{
		if (std::chrono::steady_clock::now() - lastFrame < std::chrono::milliseconds(timeSleepToTick))
		{
			//sleep remainign time
			std::this_thread::sleep_for(std::chrono::milliseconds(timeSleepToTick) - ( std::chrono::steady_clock::now() - lastFrame));
		}
		lastFrame = std::chrono::steady_clock::now();

		if (PROFILER_ON)
			Profiler::StartTracking("ECS cycle");
		nbr++;
		endThread_mutex.unlock();
		cycle();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() >= 1000)
		{
			std::cout << "ECS cycles : " << nbr << std::endl;
			nbr = 0;
			begin = std::chrono::steady_clock::now();
		}
		if (PROFILER_ON)
			Profiler::StopTracking("ECS cycle");
		endThread_mutex.lock();
	}
	endThread_mutex.unlock();
	// std::cout << "END THREAD : " << y << std::endl;

}

void ECS::makeData(int i, std::vector<void*> &data, std::vector<int> &order, glm::vec3 &cp_playerPose,
					std::vector<Component*> &vec_components, std::map<glm::vec2, AChunk*, Vec2Comparator> &chunks_needed)
{

	std::vector<id> compo_id = entities[i].getComponents();
	data.clear();

	data.push_back((void*)entities[i].getFlagInfoAddr()); //data[0] == flag_info
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
				data.push_back((void*)&tabChunks);
				data.push_back((void*)&tabChunks_mutex);
			}
			if (id == -2)
				data.push_back((void*)&entityPos_mutex);
			if (id == -3)
				data.push_back((void*)&cp_playerPose);
			if (id == -4)
				data.push_back((void*)&chunks_needed);
		}
	}
}

void ECS::cycle()
{
	playerPos_mutex.lock();
	glm::vec3 cp_playerPose = playerPos;
	playerPos_mutex.unlock();



	

	cp_playerPose = {cp_playerPose.x, cp_playerPose.z, cp_playerPose.y};
	
	int y = 0;
	for (auto system : systems)
	{
		// std::cout << "system : " << y++ << std::endl;
		std::bitset<8> system_flag_compo = system->getFlagCompo();
		std::bitset<8> system_flag_info = system->getFlagInfo();
		
		for (int i = 0; i < entities.size(); i++)
		{
			std::vector<void*> data =std::vector<void*>();
			std::bitset<8> flag_compo;

			flag_compo = entities[i].getFlagCompo();

			if ((flag_compo & system_flag_compo) != system_flag_compo)
				continue;
			
			std::vector<int> order = getDataOrder(system_flag_compo, system_flag_info);

			makeData(i, data, order, cp_playerPose, components, chunks_needed);
			
			system->apply(data);
		}
	}

	// tabChunks_mutex.lock();
	// for (auto chunk : chunks_needed)
	// {
	// 	// std::cout << "chunk pos : " << chunk.first.x << " " << chunk.first.y << std::endl;
	// 	chunk.second->deleter();
	// 	chunk.second = NULL;
	// }
	// tabChunks_mutex.unlock();
}


