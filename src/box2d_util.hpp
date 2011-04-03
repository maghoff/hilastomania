#ifndef BOX2D_UTIL_HPP
#define BOX2D_UTIL_HPP

class b2PolygonShape;
class b2CircleShape;
class b2Body;
class b2World;

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

};

#endif // BOX2D_UTIL_HPP
