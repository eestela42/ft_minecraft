#include <classes/Game/Game.hpp>
#include <classes/Tools/Profiler.hpp>
#include <thread>

int main()
{
	Game game;

	game.StartLoop();

	if (PROFILER_ON)
		Profiler::LogData();

	return 0;
}