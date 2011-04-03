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

using namespace ymse;
using namespace ymse::matrix2d::homogenous;


class Game : public ymse::game {
	ymse::bindable_keyboard_handler keyboard;
	ymse::box_reshaper box;

	ymse::key acc;

	b2World world;
	b2Body *groundBody;
	Bike bike;

	void createGroundBox() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0, -10);

		groundBody = world.CreateBody(&groundBodyDef);

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

	void renderPolygon(b2PolygonShape* polygon, const ymse::matrix33f& m) {
		glBegin(GL_LINE_LOOP);
		for (int i=0; i<polygon->GetVertexCount(); ++i) {
			const b2Vec2& v = polygon->GetVertex(i);
			vec3f v_tr = m * vec3f(v.x, v.y, 1);
			glVertex2f(v_tr[0], v_tr[1]);
		}
		glEnd();
	}

	void renderCircle(b2CircleShape* circle, const ymse::matrix33f& m) {
		glBegin(GL_LINE_STRIP);
		float r = circle->m_radius;

		vec3f center = m * vec3f(0, 0, 1);
		glVertex2f(center[0], center[1]);

		const int n = 50;
		for (int i=0; i<=n; ++i) {
			float a = i * 2.*M_PI / (float)n;
			vec3f v_tr = m * vec3f(cos(a)*r, sin(a)*r, 1);
			glVertex2f(v_tr[0], v_tr[1]);
		}

		glEnd();
	}

	void renderBody(const ymse::matrix33f& transformation, b2Body* body) {
		matrix33f m =
			transformation *
			translate(body->GetPosition().x, body->GetPosition().y) *
			rotate(-body->GetAngle())
		;

		for (b2Fixture* f = body->GetFixtureList(); f != 0; f = f->GetNext()) {
			b2Shape* sh = f->GetShape();
			b2Shape::Type t = sh->GetType();
			if (t == b2Shape::e_polygon) renderPolygon((b2PolygonShape*)sh, m);
			else if (t == b2Shape::e_circle) renderCircle((b2CircleShape*)sh, m);
		}
	}

	void render() {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		for (b2Body* body = world.GetBodyList(); body != 0; body = body->GetNext()) {
			renderBody(box.get_transformation(), body);
		}

		matrix33f m = box.get_transformation();
		for (b2Joint* joint = world.GetJointList(); joint != 0; joint = joint->GetNext()) {
			b2Vec2 a = joint->GetAnchorA();
			b2Vec2 b = joint->GetAnchorB();
			glBegin(GL_LINES);

			vec3f a_tr = m * vec3f(a.x, a.y, 1);
			glVertex2f(a_tr[0], a_tr[1]);

			vec3f b_tr = m * vec3f(b.x, b.y, 1);
			glVertex2f(b_tr[0], b_tr[1]);

			glEnd();
		}
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
