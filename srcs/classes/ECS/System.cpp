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

	glm::vec3 modPos = glm::vec3((int)pos->x % 16, (int)pos->y % 16, (int)pos->z - 2);
	if (modPos.x < 0)
		modPos.x += 16;
	if (modPos.y < 0)
		modPos.y += 16;
	bool isFilled = chunk->pubIsFilled(modPos.x, modPos.y, modPos.z);
	// bool isField2 = chunk->pubIsFilled((int)pos->x % 16, (int)pos->y % 16, (int)pos->z - 1);

	
	// isFilled = isFilled || isField2;

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

	if (!flag_info->test(0) && movement->z <= 0) //is on ground
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
	flag_info.set(1);
}

SystemMove::~SystemMove()
{
}

void SystemMove::apply(std::vector<void*> &data)
{
	glm::vec3* pos = (glm::vec3*)data[1];
	glm::vec3* movement = (glm::vec3*)data[2];

	std::mutex *entityPos_mutex = (std::mutex*)data[3];

	entityPos_mutex->lock();
	pos->x += movement->x;
	pos->y += movement->y;
	pos->z += movement->z;
	entityPos_mutex->unlock();
}

SystemChase::SystemChase()
{
	flag_components.set(0);
	flag_components.set(1);
	flag_info.set(0);
	flag_info.set(2);
}

SystemChase::~SystemChase()
{
}

void SystemChase::apply(std::vector<void*> &data)
{
	std::bitset<8> *flag_info = (std::bitset<8>*)data[0];
	glm::vec3 pos = *(glm::vec3*)data[1];
	glm::vec3* movement = (glm::vec3*)data[2];
	// glm::vec3* pos = static_cast<glm::vec3*>(data[0]);
	// std::bitset<8> *flag = (std::bitset<8>*)data[1];
	std::vector<std::vector<AChunk*>> *tabChunks = (std::vector<std::vector<AChunk*>>*)data[3];
	std::mutex *tabChunks_mutex = (std::mutex*)data[4];
	glm::vec3 playerPos = *(glm::vec3*)data[5];

	movement->x = 0;
	movement->y = 0;

	glm::vec3 posChunk = glm::vec3((int)pos.x / 16, (int)pos.y / 16, (int)pos.z / 256); //get chunk pos
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

	glm::vec3 direction = playerPos - pos;
	direction.z = 0; // Ignore the Z component
	direction = glm::normalize(direction);
	direction *= 0.5f;
	
	int modx = pos.x / 16;
	int mody = pos.y / 16;
	
	pos += direction;
	if (modx != (int)pos.x / 16 || mody != (int)pos.y / 16)
	{
			tabChunks_mutex->lock();
			chunk->deleter();
			tabChunks_mutex->unlock();
			posChunk = glm::vec3((int)pos.x / 16, (int)pos.y / 16, (int)pos.z / 256); //get chunk pos
			//borrowed from ChunkInstanciator

			tabChunks_mutex->lock();
			int size_tab = tabChunks->size();

			chunk = (*tabChunks)[mod_floor(posChunk.x, size_tab)][mod_floor(posChunk.y, size_tab)];

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

	}

	glm::vec3 modPos = glm::vec3((int)pos.x % 16, (int)pos.y % 16, (int)pos.z);
	if (modPos.x < 0)
		modPos.x += 16;
	if (modPos.y < 0)
		modPos.y += 16;
	bool isFilled = chunk->pubIsFilled(modPos.x, modPos.y, modPos.z);

	if (isFilled)
	{
			movement->z = 0.5;
			tabChunks_mutex->lock();
			chunk->deleter();
			tabChunks_mutex->unlock();
			return ;
	}
	// std::cout << "movement " << movement->x << " " << movement->y << " " << movement->z << std::endl;
	// std::cout << "direction " << direction.x << " " << direction.y << " " << direction.z << std::endl;
	// std::cout << "bfr chase" << std::endl;
	// movement->y = 0.1;
	// std::cout << "after chase" << std::endl;
	// std::cout << "movement " << movement->x << " " << movement->y << " " << movement->z << std::endl;
	
	movement->x = direction.x;
	movement->y = direction.y;


	tabChunks_mutex->lock();
	chunk->deleter();
	tabChunks_mutex->unlock();
	// std::cout << "out chase" << std::endl;
}



