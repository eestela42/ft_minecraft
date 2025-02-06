#include <classes/ECS/ECS.hpp>
#include <glm/glm.hpp>

/*
	Gravity => mettre des mobs

	Flags: is_gravity, is_mobile

	Coponent : Pos, Movement

	System : is_on_ground, Gravity, Move, Draw

is_on_ground + is_gravity => SYS_gravity[in: pos, out: movement]

SYS_move[in: movement, out : pos]

SYS_draw[in: pos, out: draw]


*/

ECS::ECS()
{
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

void ECS::addEntity()
{
	Entity entity;
	glm::vec3 tmp_pos(0, 0, 1000);
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

void ECS::cycle()
{
	for (auto system : systems)
	{

		std::bitset<8> system_flag = system->getFlag();

		std::vector<Component*> components;

		if (system_flag.test(0))
			components.push_back(this->components["pos"]);

		if (system_flag.test(1))
			components.push_back(this->components["movement"]);




		for (int i = 0; i < entities.size(); i++)
		{
			std::bitset<8> flag_compo = entities[i].getFlagCompo();

			if ((flag_compo & system_flag) != system_flag)
				continue;
			
			std::vector<id> compo_id = entities[i].getComponents();
			std::vector<void*> data;
			for (auto compo : components)
			{
				for (auto id : compo_id)
				{
					if (id.type == compo->getId())
					{
						data.push_back(compo->getComponent(id.value));
						break;
					}
				}
			}

			system->apply(data);
			
			std::cout << "END apply" << std::endl;
			

			// systems->apply(*this, entities[i]);
		}
	}
}


