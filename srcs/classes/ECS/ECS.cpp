#include <classes/ECS/ECS.hpp>

ECS::ECS(std::vector<ASystem*> systems) : systems(systems)
{
}

ECS::~ECS()
{
}

void ECS::addEntity()
{
	Entity entity;
	entities.push_back(entity);
}

void ECS::removeEntity(int index)
{
	entities.erase(entities.begin() + index);
}



void ECS::cycle()
{
	for (auto systems : systems)
	{
		std::bitset<8> system_flag = systems->getFlag();

		for (int i = 0; i < entities.size(); i++)
		{
			std::vector<id> compo = entities[i].getComponents();

			std::bitset<8> flag_compo = entities[i].getFlagCompo();

			// std::bitset<8> flag_system = entities[i].getFlagInfo();

			if (true || (flag_compo & system_flag) == system_flag) //true is for the moment
			{
				systems->apply(*this, entities[i]);
			}
		}


	}
}
