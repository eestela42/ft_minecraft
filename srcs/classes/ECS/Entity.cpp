#include <classes/ECS/Entity.hpp>

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::addComponent(int type, int value)
{
	id new_id;
	new_id.type = type;
	new_id.value = value;

	ids.push_back(new_id);
	flag_compo.set(type);
}

void Entity::removeComponent(int type)
{
	for (int i = 0; i < ids.size(); i++)
	{
		if (ids[i].type == type)
		{
			ids.erase(ids.begin() + i);
			flag_compo.reset(type);
			break;
		}
	}
}

bool Entity::hasComponent(int type)
{
	return flag_compo.test(type);
}

int Entity::getComponent(int type)
{
	for (auto id : ids)
	{
		if (id.type == type)
		{
			return id.value;
		}
	}
	return -1;
}

std::vector<id> Entity::getComponents()
{
	return ids;
}

std::bitset<8> Entity::getFlagCompo()
{
	return flag_compo;
}


std::bitset<8> Entity::getFlagInfo()
{
	return flag_info;
}

std::bitset<8>* Entity::getFlagInfoAddr()
{
	return &flag_info;
}