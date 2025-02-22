#include <classes/ECS/ECS.hpp>
#include <glm/glm.hpp>



ECS::ECS(std::vector<std::vector<AChunk*>> 	&tabChunks, std::mutex &tabChunks_mutex,
			bool &endThread, std::mutex &endThread_mutex,
			std::deque<info_VAO*> &to_VAO, 			std::mutex &to_VAO_mutex,
			std::deque<glm::ivec2> &toDeleteVAO, 	std::mutex &toDeleteVAO_mutex
		)
		: tabChunks(tabChunks), tabChunks_mutex(tabChunks_mutex),
		endThread(endThread), endThread_mutex(endThread_mutex),
		to_VAO(to_VAO), to_VAO_mutex(to_VAO_mutex),
		toDeleteVAO(toDeleteVAO), toDeleteVAO_mutex(toDeleteVAO_mutex)
{
	// systems.push_back(new SystemGetChunk());
	systems.push_back(new SystemIsOnGround());
	systems.push_back(new SystemGarvity());
	systems.push_back(new SystemMove());	
	systems.push_back(new SystemDraw());

	components["pos"] = new Component(0, sizeof(glm::vec3));
	components["movement"] = new Component(1, sizeof(glm::vec3));

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
			std::cout << "Component : " << compo.first << std::endl;
			std::vector<id> compo_id = entity.getComponents();
			bool found = false;
			for (auto id : compo_id)
			{
				if (id.type == compo.second->getId())
				{
					found = true;
					glm::vec3 *pos = (glm::vec3*)compo.second->getComponent(id.value);
					std::cout << "value : " << (*pos).x << " "  << (*pos).y << " " << (*pos).z << std::endl;
				}
			}
			if (!found)
				std::cout << "not found" << std::endl;
		}
	}
}

void ECS::addEntity()
{
	Entity entity;
	glm::vec3 tmp_pos(0, 0, 300);
	entity.addComponent(0, components["pos"]->addComponent(&tmp_pos));

	glm::vec3 tmp_mov(0, 0, 0);
	entity.addComponent(1, components["movement"]->addComponent(&tmp_mov));
	entities.push_back(entity);
}

void ECS::addEntity(int x, int y, int z)
{
	Entity entity;
	glm::vec3 tmp_pos(x, y, z);
	entity.addComponent(0, components["pos"]->addComponent(&tmp_pos));

	glm::vec3 tmp_mov(0, 0, 0);
	entity.addComponent(1, components["movement"]->addComponent(&tmp_mov));
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
	int nbr = 0;
	endThread_mutex.lock();
	while (!endThread)
	{
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

void ECS::cycle()
{
	int y = 0;
	// std::cout << "START cycle" << std::endl;
	std::vector<void*> *data = new std::vector<void*>();
	std::vector<id> compo_id;
	std::bitset<8> flag_compo;
	
	// printAll();
	for (auto system : systems)
	{
		std::bitset<8> system_flag_compo = system->getFlagCompo();
		std::bitset<8> system_flag_info = system->getFlagInfo();

		std::vector<Component*> components;

		//NOT PRETTY
		if (system_flag_compo.test(0))
			components.push_back(this->components["pos"]);
		if (system_flag_compo.test(1))
			components.push_back(this->components["movement"]);
		
		
		for (int i = 0; i < entities.size(); i++)
		{

			flag_compo = entities[i].getFlagCompo();

			if ((flag_compo & system_flag_compo) != system_flag_compo)
				continue;
			
			std::vector<int> order = getDataOrder(system_flag_compo, system_flag_info);

			compo_id = entities[i].getComponents();

			data->clear();

			data->push_back((void*)entities[i].getFlagInfoAddr()); //data[0] == flag_info
			for (int id : order)
			{
				if (id >= 0)
				{
					for (auto compo : components)
					{
						if (id == compo->getId())
						{
							for (auto id1 : compo_id)
							{
								if (id1.type == compo->getId())
								{
									data->push_back(compo->getComponent(id1.value));
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
						data->push_back((void*)&tabChunks);
						data->push_back((void*)&tabChunks_mutex);
					}
				}
			}
			system->apply(*data);
		}
		y++;
	}
	// printAll();
}


