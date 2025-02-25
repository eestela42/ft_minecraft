#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// #include <classes/ECS/ECS.hpp>
#include <bitset>
#include <iostream>
#include <vector>
#include <classes/ECS/Entity.hpp>
#include <classes/World/AChunk.hpp>
#include <map>

class ECS;

class ASystem
{
	public :
		std::bitset<8> flag_components;// pos:0 movement:1
		std::bitset<8> flag_info; 		//tabChunk:0

		ASystem();
		~ASystem();

		
		std::bitset<8> getFlagCompo();
		std::bitset<8> getFlagInfo();

		virtual void apply(std::vector<void*> &data);
};

class SystemIsOnGround : public ASystem
{
	private :
	
	public :
		SystemIsOnGround();
		~SystemIsOnGround();
		void apply(std::vector<void*> &data);
};

class SystemGetChunk : public ASystem
{
	private :
	
	public :
		SystemGetChunk();
		~SystemGetChunk();
		void apply(std::vector<void*> &data);
};

class SystemGarvity : public ASystem
{
	private :
	
	public :
		SystemGarvity();
		~SystemGarvity();
		void apply(std::vector<void*> &data);
};

class SystemChase : public ASystem
{
	private :
	
	public :
		SystemChase();
		~SystemChase();
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

#endif

