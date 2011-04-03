#ifndef BOX2D_UTIL_HPP
#define BOX2D_UTIL_HPP

#include <ymse/mouse_handler.hpp>

class b2PolygonShape;
class b2CircleShape;
class b2Body;
class b2World;
class b2MouseJoint;

namespace ymse {
	template<int, typename>
	class sq_matrix;

	typedef sq_matrix<3, float> matrix33f;
}

namespace box2d_util {

void renderPolygon(b2PolygonShape* polygon, const ymse::matrix33f& m);
void renderCircle(b2CircleShape* circle, const ymse::matrix33f& m);
void renderBody(b2Body* body, const ymse::matrix33f& transformation);
void renderWorld(b2World& world, const ymse::matrix33f& m);

class mouse_handler : public ymse::mouse_handler {
	b2World& world;
	b2Body* ground_body;
	b2MouseJoint* mouse_joint;

public:
	ymse::matrix33f pixel_to_world;

	mouse_handler(b2World&, b2Body* ground);

	void mouse_motion(int dx, int dy, int x, int y);
	void mouse_button_down(int button, int x, int y);
	void mouse_button_up(int button, int x, int y);
};

};

#endif // BOX2D_UTIL_HPP
