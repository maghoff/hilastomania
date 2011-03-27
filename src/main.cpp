#include <ymse/gl.h>
#include <iostream>
#include <ymse/sdl_core.hpp>
#include <ymse/game.hpp>
#include <ymse/reshaper.hpp>
#include <ymse/matrix.hpp>
#include <ymse/matrix3d.hpp>
#include <ymse/vec.hpp>

typedef ymse::vec<2, GLfloat> vec2f;
typedef ymse::vec<3, GLfloat> vec3f;
using ymse::matrix33f;

class Game : public ymse::game, public ymse::reshaper {
	double ang;
	matrix33f aspect;

public:

	void render() {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		using namespace ymse::matrix3d;
		matrix33f m = aspect * rotate_z(ang) * scale(0.5);

		vec2f pts[] = {
			vec2f(-1, -1),
			vec2f( 1, -1),
			vec2f( 1,  1),
			vec2f(-1,  1)
		};

		glBegin(GL_LINE_LOOP);
		for (unsigned i=0; i<sizeof(pts)/sizeof(*pts); ++i) {
			vec3f v = m * vec3f(pts[i][0], pts[i][1], 0);
			glVertex2f(v[0], v[1]);
		}
		glEnd();
	}

	void tick() {
		ang += 0.01;
	}

	void reshape(int width, int height) {
		glViewport(0, 0, width, height);
		aspect = ymse::matrix3d::scale(height/(double)width, 1, 1);
	}
};


int main(int argc, char** argv) {
	ymse::sdl_core core;
	core.init(argc, argv);

	Game game;

	core.set_game_object(&game);
	core.set_reshaper_object(&game);

	return core.run();
}
