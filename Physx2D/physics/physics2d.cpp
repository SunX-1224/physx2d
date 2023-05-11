#include "physics2d.h"

namespace Physx2D{
	using namespace Math;

	void collisionPhysics(CollisionData data, RigidBody2D* body1, RigidBody2D* body2) {

		if (body2->Type != STATIC) {
			Math::vec2 del = data.axis * Math::dot(body2->Velocity - body1->Velocity, data.axis) * (2.f / (body1->mass + body2->mass));

			body1->Velocity +=  del * body1->coef_restitution * body2->mass;
			body2->Velocity -= del * body2->coef_restitution * body1->mass;
		}
		else {
			body1->Velocity -= data.axis * 2.f * body1->coef_restitution * Math::dot(body1->Velocity, data.axis);
		}
	}

	void updatePhysics(RigidBody2D* body, float delta_time) {
		body->transform->Position = body->transform->Position + body->Velocity * delta_time;
		body->transform->Rotation += body->AngularVelocity * delta_time;

		body->Velocity = body->Velocity + body->Acceleration *(1.0f - body->coef_drag) * delta_time;
		body->AngularVelocity += body->AngularAcceleration *(1.0f - body->coef_angularDrag) * delta_time;

		body->clearAcceleration();
	}
}