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
#include <Box2D/Box2D.h>
#include "bike.hpp"
#include "box2d_util.hpp"


class Game : public ymse::game {
	ymse::bindable_keyboard_handler keyboard;
	ymse::box_reshaper box;

	ymse::key acc;

	b2World world;
	Bike bike;

	void createGroundBox() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, -10);

		b2Body* groundBody = world.CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(50, 2);

		groundBody->CreateFixture(&groundBox, 0);


		b2BodyDef airBodyDef;
		airBodyDef.position.Set(30, -7);

		b2Body* airBody = world.CreateBody(&airBodyDef);

		b2PolygonShape airBox;
		airBox.SetAsBox(2, 0.5);

		airBody->CreateFixture(&airBox, 0);
	}

public:
	Game() :
		acc(keyboard, ymse::KEY_UP),
		world(b2Vec2(0, -10), true),
		bike(world, 0, 5)
	{
		box.set_box(-10, -1, 70, 10);

		createGroundBox();

		keyboard.bind_pressed(ymse::KEY_LEFT, boost::bind(&Bike::apply_rotation, &bike, 1));
		keyboard.bind_pressed(ymse::KEY_RIGHT, boost::bind(&Bike::apply_rotation, &bike, -1));

		keyboard.bind(ymse::KEY_DOWN, boost::bind(&Bike::brakes, &bike, _1));
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
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		box2d_util::renderWorld(world, box.get_transformation());
	}

	void tick_10ms() {
		if (acc.val()) bike.apply_acceleration();

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
