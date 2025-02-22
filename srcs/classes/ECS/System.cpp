#include <classes/ECS/System.hpp>


ASystem::ASystem()
{
}

ASystem::~ASystem()
{
}


std::bitset<8> ASystem::getFlagCompo()
{

	return flag_components;
}

std::bitset<8> ASystem::getFlagInfo()
{
	return flag_info;
}

void ASystem::apply(std::vector<void*> &data)
{
	return ;
}

SystemIsOnGround::SystemIsOnGround()
{
	flag_components.set(0);
	flag_info.set(0);
}

SystemIsOnGround::~SystemIsOnGround()
{
}

void SystemIsOnGround::apply(std::vector<void*> &data)
{
	std::bitset<8> *flag_info = (std::bitset<8>*)data[0];
	glm::vec3* pos = (glm::vec3*)data[1];
	// glm::vec3* pos = static_cast<glm::vec3*>(data[0]);
	// std::bitset<8> *flag = (std::bitset<8>*)data[1];
	std::vector<std::vector<AChunk*>> *tabChunks = (std::vector<std::vector<AChunk*>>*)data[2];
	std::mutex *tabChunks_mutex = (std::mutex*)data[3];

	glm::vec3 posChunk = glm::vec3((int)pos->x / 16, (int)pos->y / 16, (int)pos->z / 256); //get chunk pos
	//borrowed from ChunkInstanciator
	tabChunks_mutex->lock();
	int size_tab = tabChunks->size();

	AChunk* chunk = (*tabChunks)[mod_floor(posChunk.x, size_tab)][mod_floor(posChunk.y, size_tab)];
	if (chunk == NULL)
	{
		tabChunks_mutex->unlock();
		return ;
	}
	chunk->borrow();
	tabChunks_mutex->unlock();

	if (chunk->getPos().x != posChunk.x || chunk->getPos().y != posChunk.y)
	{
		flag_info->reset(0);
		tabChunks_mutex->lock();
		chunk->deleter();
		tabChunks_mutex->unlock();
		return ;
	}

	bool isFilled = chunk->pubIsFilled((int)pos->x % 16, (int)pos->y % 16, (int)pos->z);

	if (isFilled)
	{
		flag_info->reset(0);

		tabChunks_mutex->lock();
		chunk->deleter();
		tabChunks_mutex->unlock();
		return ;
	}
	flag_info->set(0);
	tabChunks_mutex->lock();
	chunk->deleter();
	tabChunks_mutex->unlock();
}


SystemGarvity::SystemGarvity()
{
	flag_components.set(1);
}

SystemGarvity::~SystemGarvity()
{
}

void SystemGarvity::apply(std::vector<void*> &data)
{
	std::bitset<8> *flag_info = (std::bitset<8>*)data[0];
	

	glm::vec3* movement = (glm::vec3*)data[1];

	if (!flag_info->test(0)) //is on ground
	{
		movement->z = 0;
		return ;
	}
	if (movement->z > -1)
		movement->z -= 0.1;

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
	glm::vec3* pos = (glm::vec3*)data[1];
	glm::vec3* movement = (glm::vec3*)data[2];

	pos->x += movement->x;
	pos->y += movement->y;
	pos->z += movement->z;
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



