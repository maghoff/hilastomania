#ifndef BIKE_HPP
#define BIKE_HPP

class b2World;
class b2Body;
class b2RevoluteJoint;
class b2Vec2;

class Bike {
	b2World& world;

	struct Wheel {
		b2Body *hinge, *wheel;
		b2RevoluteJoint *motor;
	};

	b2Body* chassis;
	Wheel wheel[2];

	b2Body* createChassis(float x, float y);
	b2Body* createWheel(float x, float y);
	b2Body* createHinge(float x, float y);
	b2RevoluteJoint* connectWheel(b2Body* chassis, b2Body* wheel, const b2Vec2& pos);

public:
	Bike(b2World& world_, float x, float y);

	void brakes(bool on);
	void apply_acceleration();
	void apply_rotation(int direction);
};

#endif // BIKE_HPP
