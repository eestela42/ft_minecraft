#include <classes/Game/Game.hpp>
#include <thread>

int main() {
	unsigned int numThreads = std::thread::hardware_concurrency();
	Game *game = new Game();
	
	game->StartLoop();
	delete game;
	return 0;
}