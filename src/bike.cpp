#include <Box2D/Box2D.h>
#include "bike.hpp"

b2Body* Bike::createChassis(float x, float y) {
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0;
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(2, 1);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 2;
	fixtureDef.friction = 0.003;

	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body* Bike::createWheel(float x, float y) {
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0;
	b2Body* body = world.CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = 0.5;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 3;
	fixtureDef.friction = 5.3;

	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body* Bike::createHinge(float x, float y) {
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.angle = 0;
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.1, 0.1);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 15;
	fixtureDef.friction = 0.3;

	body->CreateFixture(&fixtureDef);

	return body;
}

b2RevoluteJoint* Bike::connectWheel(b2Body* chassis, b2Body* wheel, const b2Vec2& pos) {
	const b2Vec2& wpos = wheel->GetWorldCenter();

	b2Body* hinge = createHinge(wpos.x, wpos.y);

	b2DistanceJointDef jointDef;
	jointDef.Initialize(hinge, chassis, wpos, pos);
	jointDef.collideConnected = false;
	jointDef.frequencyHz = 7.f;
	jointDef.dampingRatio = 0.1;
	world.CreateJoint(&jointDef);

	b2PrismaticJointDef pDef;
	pDef.Initialize(hinge, chassis, wpos, pos - wpos);
	pDef.lowerTranslation = -5.0f;
	pDef.upperTranslation = 2.5f;
	pDef.enableLimit = false;
	pDef.motorSpeed = 0.f;
	pDef.enableMotor = false;
	world.CreateJoint(&pDef);

	b2RevoluteJointDef rDef;
	rDef.Initialize(hinge, wheel, wpos);
	rDef.motorSpeed = 0;
	rDef.maxMotorTorque = 1000.;
	rDef.enableLimit = false;
	rDef.enableMotor = false;
	return (b2RevoluteJoint*)world.CreateJoint(&rDef);
}

Bike::Bike(b2World& world_, float x, float y) :
	world(world_)
{
	chassis = createChassis(x, y);

	wheel[0].wheel = createWheel(x-2, y-2);
	wheel[0].motor = connectWheel(chassis, wheel[0].wheel, b2Vec2(x-1, y-1));

	wheel[1].wheel = createWheel(x+2, y-2);
	wheel[1].motor = connectWheel(chassis, wheel[1].wheel, b2Vec2(x+1, y-1));
}

void Bike::brakes(bool on) {
	wheel[0].motor->EnableMotor(on);
	wheel[1].motor->EnableMotor(on);
}

void Bike::apply_acceleration() {
	wheel[0].wheel->ApplyTorque(-50.);
}

void Bike::apply_rotation(int direction) {
	chassis->ApplyAngularImpulse(100 * direction);
}
