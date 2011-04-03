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

// From Box2D Testbed:
struct QueryCallback : public b2QueryCallback {
	QueryCallback(const b2Vec2& point) {
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody) {
			bool inside = fixture->TestPoint(m_point);
			if (inside) {
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

mouse_handler::mouse_handler(b2World& world_, b2Body* ground_body_) :
	world(world_),
	ground_body(ground_body_),
	mouse_joint(0)
{
}

void mouse_handler::mouse_motion(int dx, int dy, int x, int y) {
	if (mouse_joint) {
		ymse::vec3f mouse = pixel_to_world * ymse::vec3f(x, y, 1);
		b2Vec2 p(mouse[0], mouse[1]);
		mouse_joint->SetTarget(p);
	}
}

void mouse_handler::mouse_button_down(int button, int x, int y) {
	if (button == 1) {
		ymse::vec3f mouse = pixel_to_world * ymse::vec3f(x, y, 1);
		b2Vec2 p(mouse[0], mouse[1]);

		// Make a small box.
		b2AABB aabb;
		b2Vec2 d;
		d.Set(0.001f, 0.001f);
		aabb.lowerBound = p - d;
		aabb.upperBound = p + d;

		// Query the world for overlapping shapes.
		QueryCallback callback(p);
		world.QueryAABB(&callback, aabb);

		if (callback.m_fixture) {
			b2Body* body = callback.m_fixture->GetBody();
			b2MouseJointDef md;
			md.bodyA = ground_body;
			md.bodyB = body;
			md.target = p;
			md.maxForce = 1000.0f * body->GetMass();
			mouse_joint = (b2MouseJoint*)world.CreateJoint(&md);
			body->SetAwake(true);
		}
	}
}

void mouse_handler::mouse_button_up(int button, int x, int y) {
	if (button == 1 && mouse_joint) {
		world.DestroyJoint(mouse_joint);
		mouse_joint = 0;
	}
}

}
