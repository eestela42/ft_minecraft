#ifndef ECS_HPP
#define ECS_HPP

# include <vector>
# include <unordered_map>
# include <classes/ECS/Entity.hpp>
# include <classes/ECS/Component.hpp>
# include <classes/ECS/System.hpp>


class ECS
{
	private :
		std::vector<Entity> entities;

		std::unordered_map<std::string, AComponent*> components;

		std::vector<ASystem*> systems;

	public :
		ECS(std::vector<ASystem*> systems);
		~ECS();
		
		void addEntity();
		void removeEntity(int index);

		void cycle();


};


#endif