#include <ymse/gl.h>
#include <Box2D/Box2D.h>
#include <ymse/matrix.hpp>
#include <ymse/matrix2d_homogenous.hpp>
#include <ymse/vec.hpp>
#include "box2d_util.hpp"

using ymse::vec3f;
using ymse::matrix33f;
using namespace ymse::matrix2d::homogenous;

namespace box2d_util {

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

void renderBody(b2Body* body, const ymse::matrix33f& transformation) {
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

void renderWorld(b2World& world, const ymse::matrix33f& m) {
	for (b2Body* body = world.GetBodyList(); body != 0; body = body->GetNext()) {
		renderBody(body, m);
	}

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

}
