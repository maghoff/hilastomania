#include <ymse/gl.h>
#include <iostream>
#include <boost/bind.hpp>
#include <ymse/sdl_core.hpp>
#include <ymse/game.hpp>
#include <ymse/bindable_keyboard_handler.hpp>
#include <ymse/keycodes.hpp>
#include <ymse/key.hpp>
#include <ymse/box_reshaper.hpp>
#include <ymse/matrix2d_homogenous.hpp>
#include <ymse/vec.hpp>

#include <Box2D.h>


class Game : public ymse::game {
	ymse::bindable_keyboard_handler keyboard;
	ymse::box_reshaper box;

	ymse::key acc;

	b2World world;
	b2Body *groundBody, *body;

	void createGroundBox() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, -10);

		groundBody = world.CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(50, 10);

		groundBody->CreateFixture(&groundBox, 0);
	}

	void createDynamicBody() {
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0, 4);
		bodyDef.angle = 1.1;
		body = world.CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(1, 1);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1;
		fixtureDef.friction = 0.3;

		body->CreateFixture(&fixtureDef);
	}

public:
	Game() :
		acc(keyboard, ymse::KEY_UP),
		world(b2Vec2(0, -10), true)
	{
		box.set_box(-1, 0, 1, 10);

		createGroundBox();
		createDynamicBody();
	}

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

		matrix33f m =
			box.get_transformation() *
			translate(body->GetPosition().x, body->GetPosition().y) *
			rotate(-body->GetAngle())
		;

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
		//ang += 0.01;

		if (acc.val()) body->ApplyTorque(-50.);

		float32 timeStep = 10. / 1000.;
		int32 velocityIterations = 10;
		int32 positionIterations = 10;
		world.Step(timeStep, velocityIterations, positionIterations);
	}
};


int main(int argc, char** argv) {
	ymse::sdl_core core;
	core.init(argc, argv);

	Game game;
	game.attach_to_core(core);

	return core.run();
}
