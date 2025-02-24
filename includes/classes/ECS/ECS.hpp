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



class ECS
{
	private :
		std::vector<Entity> entities;

		std::unordered_map<std::string, Component*> components;

		std::vector<ASystem*> systems;

		std::vector<std::vector<AChunk*>> 	&tabChunks;
		std::mutex 							&tabChunks_mutex;

		std::deque<info_VAO*> 				&to_VAO;
		std::mutex 							&to_VAO_mutex;

		std::deque<glm::ivec2> 				&toDeleteVAO;
		std::mutex 							&toDeleteVAO_mutex;

		std::vector<unsigned char> 				**entityPos;
		std::mutex 							&entityPos_mutex;

		bool &endThread;
		std::mutex &endThread_mutex;

	public :
		ECS(std::vector<std::vector<AChunk*>> 	&tabChunks, std::mutex &tabChunks_mutex,
			bool &endThread, std::mutex &endThread_mutex,
			std::deque<info_VAO*> &to_VAO, 			std::mutex &to_VAO_mutex,
			std::deque<glm::ivec2> &toDeleteVAO, 	std::mutex &toDeleteVAO_mutex,
			std::vector<unsigned char> **entityPos, std::mutex &entityPos_mutex);
		~ECS();
		
		void addEntity();
		void addEntity(int x, int y, int z);
		void removeEntity(int index);

		void update();
		void cycle();

		void printAll();


};


#endif