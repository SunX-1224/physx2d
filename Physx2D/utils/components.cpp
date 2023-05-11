#include "components.h"

namespace Physx2D {
	void RigidBody2D::addForce(Math::vec2 force) {
		Acceleration += force * (1.0f / mass);
	}

	void RigidBody2D::addAcceleration(Math::vec2 acc) {
		Acceleration += acc;
	}

	void RigidBody2D::clearAcceleration() {
		Acceleration = 0.0f;
	}

	CollisionData Collider::checkCollision(Collider* collider, BodyType type)
	{ return CollisionData(); }
	
	CollisionData Collider::constraintCollision(Collider* collider)
	{ return CollisionData(); }

	CollisionData BoxCollider2D::checkCollision(Collider* collider, BodyType type) {
		BoxCollider2D* bc = (BoxCollider2D*)collider;
		Math::vec2 c_axis = Math::vec2(0.f);
		Math::vec2 dir = bc->transform->Position + bc->Offset - transform->Position - Offset;
		float depth = Math::MAX_float;

		for (int i = 0; i < 4; i++) {
			Math::vec2 axis = (Math::vec2)QUAD_VERTICES[i] - (Math::vec2)QUAD_VERTICES[(i + 1) % 4];
			axis = axis.rotate(Rotation + transform->Rotation);
			axis = Math::vec2(axis.y, -axis.x);

			float minA = Math::MAX_float, maxA = Math::MIN_float;
			for (int j = 0; j < 2; j++) {
				Math::vec2 v = Size * ((Math::vec2)QUAD_VERTICES[(j + i + 1) % 4]).rotate(Rotation + transform->Rotation) + Offset + transform->Position;
				float dot = Math::dot(v, axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}

			float minB = Math::MAX_float, maxB = Math::MIN_float;
			for (int j = 0; j < 4; j++) {
				Math::vec2 v = bc->Size * ((Math::vec2)QUAD_VERTICES[j]).rotate(bc->Rotation + bc->transform->Rotation) + bc->Offset + bc->transform->Position;
				float dot = Math::dot(v, axis);
				if (dot < minB) minB = dot;
				if (dot > maxB) maxB = dot;
			}

			if (minA >= maxB || minB >= maxA) return CollisionData();
			if (Math::dot(axis, dir) > 0.f) {
				float temp = min(maxB, maxA) - max(minA, minB);
				if (temp < depth) {
					depth = temp;
					c_axis = axis;
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			Math::vec2 axis = (Math::vec2)QUAD_VERTICES[(i + 1) % 4] - (Math::vec2)QUAD_VERTICES[i];
			axis = axis.rotate(bc->Rotation + bc->transform->Rotation);
			axis = Math::vec2(axis.y, -axis.x);

			float minA = Math::MAX_float, maxA = Math::MIN_float;
			for (int j = 0; j < 4; j++) {
				Math::vec2 v = Size * ((Math::vec2)QUAD_VERTICES[j]).rotate(Rotation + transform->Rotation) + Offset + transform->Position;
				float dot = Math::dot(v, axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}

			float minB = Math::MAX_float, maxB = Math::MIN_float;
			for (int j = 0; j < 2; j++) {
				Math::vec2 v = bc->Size * ((Math::vec2)QUAD_VERTICES[(j + i + 1) % 4]).rotate(bc->Rotation + bc->transform->Rotation) + bc->Offset + bc->transform->Position;
				float dot = Math::dot(v, axis);
				if (dot < minB) minB = dot;
				if (dot > maxB) maxB = dot;
			}

			if (minA >= maxB || minB >= maxA) return CollisionData();
			if (Math::dot(-axis, dir) > 0.f) {
				float temp = min(maxB, maxA) - max(minA, minB);
				if (temp < depth) {
					depth = temp;
					c_axis = -axis;
				}
			}
		}

		Math::vec2 del = c_axis * (depth / dot(c_axis, c_axis));
		if (type == DYNAMIC) {
			del *= 0.5f;
			bc->transform->Position += del;
		}
		transform->Position -= del;
		return CollisionData(true, c_axis.normalized());
	}

	//TODO
	CollisionData BoxCollider2D::constraintCollision(Collider* collider) 
	{ return CollisionData(); }

	CollisionData CircleCollider::checkCollision(Collider* collider, BodyType type){
			Math::vec2 c_axis = Math::vec2(0.f);
			float overlap = 0.f;
			if (collider->Type == CIRCLE) {
				CircleCollider* cc = (CircleCollider*) collider;
				c_axis = (cc->transform->Position + cc->Offset) - (transform->Position + Offset);
				overlap = Radius + cc->Radius - c_axis.length();
				if (overlap <= 0.0f)
					return CollisionData();
				overlap *= 1.f/c_axis.length();
			}
			else {
				BoxCollider2D* bc = (BoxCollider2D*)collider;
				float depth = Math::MAX_float;

				Math::vec2 dir = bc->transform->Position + bc->Offset - transform->Position - Offset;
				Math::vec2 naxis = Math::vec2(0.f);
				float len = Math::MAX_float;

				for (int i = 0; i < 4; i++) {
					Math::vec2 v = bc->Size * ((Math::vec2)QUAD_VERTICES[i]).rotate(bc->Rotation + bc->transform->Rotation) + bc->Offset + bc->transform->Position;

					Math::vec2 temp = v - transform->Position - Offset;
					if (temp.length() < len) {
						naxis = temp;
						len = temp.length();
					}
				}

				float minA = Math::MAX_float, maxA = Math::MIN_float;
				for (int j = 0; j < 4; j++) {
					Math::vec2 v = bc->Size * ((Math::vec2)QUAD_VERTICES[j]).rotate(bc->Rotation + bc->transform->Rotation) + bc->Offset + bc->transform->Position;
					float dot = Math::dot(v, naxis);
					if (dot < minA) minA = dot;
					if (dot > maxA) maxA = dot;
				}
				float minB = Math::dot(Offset + transform->Position, naxis) - Radius;
				float maxB = minB + 2 * Radius;

				if (minA > maxB || minB > maxA) return CollisionData();
				if (Math::dot(-naxis, dir) > 0.f) {
					float temp = min(maxB, maxA) - max(minA, minB);
					if (temp < depth) {
						depth = temp;
						c_axis = -naxis;
					}
				}

				for (int i = 0; i < 4; i++) {
					Math::vec2 axis = (Math::vec2)QUAD_VERTICES[i] - (Math::vec2)QUAD_VERTICES[(i + 1) % 4];
					axis = axis.rotate(bc->Rotation + bc->transform->Rotation);
					axis = Math::vec2(axis.y, -axis.x);

					float minA = Math::MAX_float, maxA = Math::MIN_float;
					for (int j = 0; j < 2; j++) {
						Math::vec2 v = bc->Size * ((Math::vec2)QUAD_VERTICES[(j + i + 1)%4]).rotate(bc->Rotation + bc->transform->Rotation) + bc->Offset + bc->transform->Position;
						float dot = Math::dot(v, axis);
						if (dot < minA) minA = dot;
						if (dot > maxA) maxA = dot;
					}

					float minB = Math::dot(Offset + transform->Position, axis) - Radius;
					float maxB = minB + 2 * Radius;

					if (minA > maxB || minB > maxA) return CollisionData();
					if (Math::dot(-axis, dir) > 0.f) {
						float temp = min(maxB, maxA) - max(minA, minB);
						if (temp < depth) {
							depth = temp;
							c_axis = -axis;
						}
					}
				}

				overlap = (depth / dot(c_axis, c_axis));
			}

			Math::vec2 del = c_axis * overlap;
			if (type == DYNAMIC) {
				del *= 0.5f;
				collider->transform->Position += del;
			}
			transform->Position -= del;
			return CollisionData(true, c_axis.normalized());
	}

	CollisionData CircleCollider::constraintCollision(Collider* boundingObject) {
		if (boundingObject->Type == ColliderType::BOUNDING_CIRCLE) {
			BoundingCircle* constraint = (BoundingCircle*)boundingObject;
			Math::vec2 axis = (constraint->transform->Position + constraint->Offset) - (transform->Position + Offset);
			float overlap = (axis.length() + Radius) - constraint->Radius;
			if (overlap > 0.f) {
				axis = axis.normalized();
				transform->Position += axis * overlap;
				return CollisionData(true, axis);
			}
		}
		else if(boundingObject->Type == ColliderType::AABB) {
			AxisAlignedBB* constraint = (AxisAlignedBB*)boundingObject;
			Math::vec2 minB = ((Math::vec2)QUAD_VERTICES[0]) * constraint->Size;
			Math::vec2 maxB = ((Math::vec2)QUAD_VERTICES[2]) * constraint->Size;

			Math::vec2 c_axis = 0.0f;

			Math::vec2 A = transform->Position + Offset;
			minB = A - minB - Math::vec2(Radius);
			maxB = A - maxB + Math::vec2(Radius);

			if (minB.x > 0.f) minB.x = 0.f;
			if (maxB.x < 0.f) maxB.x = 0.f;
			
			if (minB.y > 0.f) minB.y = 0.f;
			if (maxB.y < 0.f) maxB.y = 0.f;

			c_axis = minB + maxB;
			if (c_axis.length() == 0.f) return CollisionData();
			transform->Position -= c_axis;
			return CollisionData(true, c_axis.normalized());

		}
		return CollisionData();
	}

}