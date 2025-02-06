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
	public :
		std::bitset<8> flag_components;
		ASystem();
		~ASystem();
		
		virtual std::bitset<8> getFlag();
		virtual void apply(std::vector<void*> &data);
};

class SystemGarvity : public ASystem
{
	private :
	
	public :
		SystemGarvity();
		~SystemGarvity();
		void apply(std::vector<void*> &data);
};

class SystemIsOnGround : public ASystem
{
	private :
	
	public :
		SystemIsOnGround();
		~SystemIsOnGround();
		void apply(std::vector<void*> &data);
};

class SystemMove : public ASystem
{
	private :
	
	public :
		SystemMove();
		~SystemMove();
		void apply(std::vector<void*> &data);
};

class SystemDraw : public ASystem
{
	private :
	
	public :
		SystemDraw();
		~SystemDraw();
		void apply(std::vector<void*> &data);
};

#endif

