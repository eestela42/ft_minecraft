#include <classes/Game/Game.hpp>
#include <classes/Profiler.hpp>
#include <thread>

int main() {	
	unsigned int numThreads = std::thread::hardware_concurrency();
	if (PROFILER_ON)
		Profiler::StartTracking("Game Constructor");
	Game game;
	if (PROFILER_ON)
		Profiler::StopTracking("Game Constructor");
	
	game.StartLoop();
	std::cout << "Game loop ended" << std::endl;
	if (PROFILER_ON)
		Profiler::LogData();
	std::cout << "Profiler data logged" << std::endl;
	// delete game;
	return 0;
}