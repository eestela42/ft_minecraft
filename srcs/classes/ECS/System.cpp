#include <classes/ECS/System.hpp>

ASystem::ASystem()
{
}

ASystem::~ASystem()
{
}

SystemGarvity::SystemGarvity()
{
}

SystemGarvity::~SystemGarvity()
{
}

std::bitset<8> SystemGarvity::getFlag()
{
	return flag_components;
}

void SystemGarvity::apply(ECS &ecs, Entity &entity)
{
	std::vector<id> compo = entity.getComponents();
	std::cout << "gravity" << std::endl;

	
}