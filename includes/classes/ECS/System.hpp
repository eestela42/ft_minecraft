#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// #include <classes/ECS/ECS.hpp>
#include <bitset>
#include <iostream>
#include <vector>
#include <classes/ECS/Entity.hpp>

class ECS;

class ASystem
{
	private :
		
	public :
		ASystem();
		~ASystem();
		
		virtual std::bitset<8> getFlag() = 0;
		virtual void apply(ECS &ecs, Entity &entity) = 0;
};

class SystemGarvity : public ASystem
{
	private :
		std::bitset<8> flag_components = 0b00000001;
	
	public :
		SystemGarvity();
		~SystemGarvity();
		
		
		std::bitset<8> getFlag();

		void apply(ECS &ecs, Entity &entity);
};

#endif

