#include <classes/ECS/System.hpp>


AChunk *getChunk(std::vector<std::vector<AChunk*>> &tabChunks, std::mutex &tabChunks_mutex, glm::vec3 pos)
{
	tabChunks_mutex.lock();
	int size_tab = tabChunks.size();

	glm::vec3 posChunk = glm::vec3((int)pos.x / 16, (int)pos.y / 16, (int)pos.z / 256);

	AChunk* chunk = tabChunks[mod_floor(posChunk.x, size_tab)][mod_floor(posChunk.y, size_tab)];
	if (chunk == NULL)
	{
		tabChunks_mutex.unlock();
		return NULL;
	}
	chunk->borrow();
	tabChunks_mutex.unlock();

	if (chunk->getPos().x != posChunk.x || chunk->getPos().y != posChunk.y)
	{
		tabChunks_mutex.lock();
		chunk->deleter();
		tabChunks_mutex.unlock();
		return NULL;
	}

	return chunk;
}

void checkChangedChunk(bool &changedChunkX, bool &changedChunkY, glm::ivec3 pos, glm::ivec3 newPos)
{
	if (newPos.x / 16 != pos.x / 16)
		changedChunkX = true;
	if (newPos.y / 16 != pos.y / 16)
		changedChunkY = true;
}

glm::ivec3 getModPos(glm::vec3 pos)
{
	glm::ivec3 modPos = glm::vec3(((int)pos.x % 16 + 16) % 16, ((int)pos.y % 16 + 16) % 16, pos.z);
	// if (modPos.x < 0)
	// 	modPos.x *= -1;
	// if (modPos.y < 0)
	// 	modPos.y *= -1;
	return modPos;
}


int collide(glm::vec3* pos,glm::vec3* movement, std::vector<std::vector<AChunk*>> *tabChunks, std::mutex *tabChunks_mutex, AChunk *chunk, bool &collideX, bool &collideY, bool &collideZ)
{
	glm::vec3 newPos = *pos + *movement;
	AChunk *chunkDirX = chunk;
	
	if (movement->x)
	{
		int checkPos;
		if (movement->x > 0)
			checkPos = newPos.x + 1;
		else
			checkPos = newPos.x;
		if (checkPos / 16 != (int)pos->x / 16)
		{
			chunkDirX = getChunk(*tabChunks, *tabChunks_mutex, glm::vec3(checkPos, pos->y, pos->z));
		}
	}

	AChunk *chunkDirY = chunk;
	if (movement->y)
	{
		int checkPos;
		if (movement->y > 0)
			checkPos = newPos.y + 1;
		else
			checkPos = newPos.y;
		if (checkPos / 16 != (int)pos->y / 16)
		{
			chunkDirY = getChunk(*tabChunks, *tabChunks_mutex, glm::vec3(pos->x, checkPos, pos->z));
		}
	}

	AChunk *chunkDirXY = chunk;
	if (chunkDirX && chunk != chunkDirX && chunkDirY && chunk != chunkDirY)
	{
		glm::vec3 tmpPos = newPos;
		if (movement->x > 0)
			tmpPos.x += 1;
		if (movement->y > 0)
			tmpPos.y += 1;
		chunkDirXY = getChunk(*tabChunks, *tabChunks_mutex, tmpPos);
	}

	
	glm::vec3 offsets[] = {
		{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 0},
		{0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}
	};

	if (!chunkDirX || !chunkDirY || !chunkDirXY)
	{
		if (chunkDirX && chunkDirX != chunk)
			chunkDirX->deleter();
		if (chunkDirY && chunkDirY != chunk)
			chunkDirY->deleter();
		if (chunkDirXY && chunkDirXY != chunk)
			chunkDirXY->deleter();
		// std::cout << "chunk not found" << std::endl;
		return 0;
	}

	glm::ivec3 modPos = getModPos(*pos);

	for (glm::vec3& offset : offsets) {
		AChunk *chunk_test = chunk;
		glm::ivec3 modTestPos;
		glm::ivec3 testPos;

		bool changedChunkX, changedChunkY;
		changedChunkX = changedChunkY = false;
		
		testPos = {newPos.x + offset.x, pos->y + offset.y, pos->z + offset.z};
		modTestPos = getModPos(testPos);
		checkChangedChunk(changedChunkX, changedChunkY, *pos, testPos);
		if (changedChunkX && changedChunkY)
			chunk_test = chunkDirXY;
		else if (changedChunkX)
			chunk_test = chunkDirX;
		else if (changedChunkY)
			chunk_test = chunkDirY;
		
		if (chunk_test->pubIsFilled(modTestPos.x, modTestPos.y, modTestPos.z))
			collideX = true;

		testPos = {pos->x + offset.x, newPos.y + offset.y, pos->z + offset.z};
		modTestPos = getModPos(testPos);
		checkChangedChunk(changedChunkX, changedChunkY, *pos, testPos);
		if (changedChunkX && changedChunkY)
			chunk_test = chunkDirXY;
		else if (changedChunkX)
			chunk_test = chunkDirX;
		else if (changedChunkY)
			chunk_test = chunkDirY;

		if (chunk_test->pubIsFilled(modTestPos.x, modTestPos.y, modTestPos.z))
			collideY = true;
		
		testPos = {pos->x + offset.x, pos->y + offset.y, newPos.z + offset.z};
		modTestPos = getModPos(testPos);
		checkChangedChunk(changedChunkX, changedChunkY, *pos, testPos);
		if (changedChunkX && changedChunkY)
			chunk_test = chunkDirXY;
		else if (changedChunkX)
			chunk_test = chunkDirX;
		else if (changedChunkY)
			chunk_test = chunkDirY;
			
		if (chunk_test->pubIsFilled(modTestPos.x, modTestPos.y, modTestPos.z))
			collideZ = true;
	}
	return 1;
}

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
	std::vector<std::vector<AChunk*>> *tabChunks = (std::vector<std::vector<AChunk*>>*)data[2];
	std::mutex *tabChunks_mutex = (std::mutex*)data[3];

	AChunk *chunk = getChunk(*tabChunks, *tabChunks_mutex, *pos);

	if (!chunk)
	{
		flag_info->reset(0);
		return ;
	}

	glm::vec3 modPos = getModPos(*pos);

	bool isFilled = chunk->pubIsFilled(modPos.x, modPos.y, modPos.z - 1);

	if (isFilled)
	{
		flag_info->set(0);

		tabChunks_mutex->lock();
		chunk->deleter();
		tabChunks_mutex->unlock();
		return ;
	}
	flag_info->reset(0);

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

	if (flag_info->test(0) && movement->z <= 0) //is on ground
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
	flag_info.set(0);
	flag_info.set(1);
}

SystemMove::~SystemMove()
{
}

void SystemMove::apply(std::vector<void*> &data)
{
	glm::vec3* pos = (glm::vec3*)data[1];
	glm::vec3* movement = (glm::vec3*)data[2];

	std::vector<std::vector<AChunk*>> *tabChunks = (std::vector<std::vector<AChunk*>>*)data[3];
	std::mutex *tabChunks_mutex = (std::mutex*)data[4];


	std::mutex *entityPos_mutex = (std::mutex*)data[5];

	AChunk *chunk = getChunk(*tabChunks, *tabChunks_mutex, *pos);

	if (!chunk)
		return ;

	entityPos_mutex->lock();

	bool collideX = false, collideY = false, collideZ = false;
	if (!collide(pos, movement, tabChunks, tabChunks_mutex, chunk, collideX, collideY, collideZ))
	{
		// std::cout << "move collide" << std::endl;
		entityPos_mutex->unlock();
		chunk->deleter();
		return ;
	}


    if (!collideX)
		pos->x += movement->x;
    if (!collideY)
		pos->y += movement->y;
    if (!collideZ)
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
	std::vector<std::vector<AChunk*>> *tabChunks = (std::vector<std::vector<AChunk*>>*)data[3];
	std::mutex *tabChunks_mutex = (std::mutex*)data[4];
	glm::vec3 playerPos = *(glm::vec3*)data[5];

	movement->x = 0;
	movement->y = 0;

	AChunk *chunk = getChunk(*tabChunks, *tabChunks_mutex, pos);

	if (!chunk)
		return ;

	

	glm::vec3 direction = playerPos - pos;
	direction.z = 0;
	direction = glm::normalize(direction);
	direction *= 0.5f;
	
	
	movement->x = direction.x;
	movement->y = direction.y;

	bool collideX = false, collideY = false, collideZ = false;
	if (!collide(&pos, movement, tabChunks, tabChunks_mutex, chunk, collideX, collideY, collideZ))
	{
		tabChunks_mutex->lock();
		chunk->deleter();
		tabChunks_mutex->unlock();
		return ;
	}

	if (flag_info->test(0) && (collideX || collideY))
		movement->z = 0.5;
	if (collideX)
		movement->x = 0;
	if (collideY)
		movement->y = 0;


	tabChunks_mutex->lock();
	chunk->deleter();
	tabChunks_mutex->unlock();
}



