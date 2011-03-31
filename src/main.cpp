#include <ymse/gl.h>
#include <iostream>
#include <boost/bind.hpp>
#include <ymse/sdl_core.hpp>
#include <ymse/game.hpp>
#include <ymse/bindable_keyboard_handler.hpp>
#include <ymse/keycodes.hpp>
#include <ymse/box_reshaper.hpp>
#include <ymse/matrix2d_homogenous.hpp>
#include <ymse/vec.hpp>


class Game : public ymse::game {
	ymse::bindable_keyboard_handler keyboard;
	ymse::box_reshaper box;

	double ang;

public:
	Game() { }
	~Game() { }

	void attach_to_core(ymse::sdl_core& core) {
		keyboard.bind_pressed(ymse::KEY_Q, boost::bind(&ymse::sdl_core::stop, &core, 0));
		keyboard.bind_pressed(ymse::KEY_F, boost::bind(&ymse::sdl_core::toggle_fullscreen, &core));

		core.set_game_object(this);
		core.set_keyboard_handler(&keyboard);
		core.set_reshaper_object(&box);
	}

	void render() {
		using namespace ymse;
		using namespace ymse::matrix2d::homogenous;

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		matrix33f m = box.get_transformation() * rotate(ang) * scale(0.5);

		vec2f pts[] = {
			vec2f(-1, -1),
			vec2f( 1, -1),
			vec2f( 1,  1),
			vec2f(-1,  1)
		};

		glBegin(GL_LINE_LOOP);
		for (unsigned i=0; i<sizeof(pts)/sizeof(*pts); ++i) {
			vec3f v = m * vec3f(pts[i][0], pts[i][1], 1);
			glVertex2f(v[0], v[1]);
		}
		glEnd();
	}

	void tick_10ms() {
		ang += 0.01;
	}
};


int main(int argc, char** argv) {
	ymse::sdl_core core;
	core.init(argc, argv);

	Game game;
	game.attach_to_core(core);

	return core.run();
}
