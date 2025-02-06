#include <classes/ECS/System.hpp>


ASystem::ASystem()
{
}

ASystem::~ASystem()
{
}

SystemGarvity::SystemGarvity()
{
	flag_components.set(1);
}

SystemGarvity::~SystemGarvity()
{
}

std::bitset<8> ASystem::getFlag()
{

	return flag_components;
}

void ASystem::apply(std::vector<void*> &data)
{
	return ;
}

void SystemGarvity::apply(std::vector<void*> &data)
{
	glm::vec3* movement = (glm::vec3*)data[0];

	movement->z -= 0.1;
}

SystemIsOnGround::SystemIsOnGround()
{
	flag_components.set(0);
}

SystemIsOnGround::~SystemIsOnGround()
{
}

void SystemIsOnGround::apply(std::vector<void*> &data)
{
	glm::vec3* pos = (glm::vec3*)data[0];
	// glm::vec3* pos = static_cast<glm::vec3*>(data[0]);
	// std::bitset<8> *flag = (std::bitset<8>*)data[1];

	if (pos->z < 100)
		;
}

SystemMove::SystemMove()
{
	flag_components.set(0);
	flag_components.set(1);
}

SystemMove::~SystemMove()
{
}

void SystemMove::apply(std::vector<void*> &data)
{
	glm::vec3* pos = (glm::vec3*)data[0];
	glm::vec3* movement = (glm::vec3*)data[1];

	pos->x += movement->x;
	pos->y += movement->y;
	pos->z += movement->z;
	std::cout << "pos z " << pos->z << std::endl;
	std::cout << "SystemMove apply end" << std::endl;
}

SystemDraw::SystemDraw()
{
}

SystemDraw::~SystemDraw()
{
}

void SystemDraw::apply(std::vector<void*> &data)
{

	/*

	on veut check si y a un vao pour l'entity
	=> si non on le creer => vaoHandler
	=> si oui rien

	=> donner la pos au VAOHandler


	*/
	
	/*

	VAOHandler

	on va avoir 1 VAO par entity
	on va avoir la data de son uniform -> modified by the System
	
	
	*/

}



