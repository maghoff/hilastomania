#include <iostream>
#include <ymse/sdl_core.hpp>
#include <ymse/game.hpp>


class Game : public ymse::game {
public:

	void render() {
	}

	void tick() {
	}
};


int main(int argc, char** argv) {
	ymse::sdl_core core;
	core.init(argc, argv);

	Game game;

	core.set_game_object(&game);

	return core.run();
}
