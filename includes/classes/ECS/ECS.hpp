#ifndef ECS_HPP
#define ECS_HPP

# include <vector>
# include <unordered_map>
# include <classes/ECS/Entity.hpp>
# include <classes/ECS/Component.hpp>
# include <classes/ECS/System.hpp>
# include <mutex>
# include <classes/World/AChunk.hpp>
#include <deque>
#include <thread>
#include <map>



class ECS
{
	private :
		std::vector<Entity> entities;

		std::vector<Component*> components;

		std::vector<ASystem*> systems;

		//stockage chunks needed
		std::unordered_map<glm::ivec2, AChunk*,  IVec2Hash, IVec2Equal> chunks_needed;
		std::vector<std::vector<AChunk*>> tabChunks_needed;

		unsigned int playerEntity;

		std::vector<std::vector<AChunk*>> 	&tabChunks;
		std::mutex 							&tabChunks_mutex;

		glm::vec3 							&playerPos;
		std::mutex 							&playerPos_mutex;

		std::vector<unsigned char> 				**entityPos;
		std::mutex 							&entityPos_mutex;

		bool &endThread;
		std::mutex &endThread_mutex;

	public :
		ECS(std::vector<std::vector<AChunk*>> 	&tabChunks, std::mutex &tabChunks_mutex,
			glm::vec3 &playerPos, std::mutex &playerPos_mutex,
			bool &endThread, std::mutex &endThread_mutex,
			std::vector<unsigned char> **entityPos, std::mutex &entityPos_mutex);
		~ECS();
		
		void addEntity();
		void addEntity(int x, int y, int z);
		void removeEntity(int index);

		void update();
		void cycle();

		void makeData(int i, std::vector<void*> &data, std::vector<int> &order, glm::vec3 &cp_playerPose,
			std::vector<Component*> &vec_components, std::unordered_map<glm::ivec2, AChunk*,  IVec2Hash, IVec2Equal> &chunks_needed);

		void printAll();


};


#endif