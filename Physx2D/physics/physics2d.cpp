#include "physics2d.h"

namespace Physx2D{
	using namespace Math;

	CollisionData PhysicsHandler::checkCollision(Collider* c1, Transform* t1, BodyType s1, Collider* c2, Transform* t2, BodyType s2) {
		CollisionData data;

		//for preconditions
		std::type_index box = std::type_index(typeid(BoxCollider2D));
		std::type_index circle = std::type_index(typeid(CircleCollider));
		std::type_index aabb = std::type_index(typeid(AABB));
		std::type_index bcircle = std::type_index(typeid(BoundingCircle));

		//check respective collider types
		if (c1->typeIndex == circle) {
			if (c2->typeIndex == circle) 
				data = _check((CircleCollider*)c1, t1, (CircleCollider*)c2, t2);
			else if (c2->typeIndex == box)
				data = _check((CircleCollider*)c1, t1, (BoxCollider2D*)c2, t2);
			else
				throw("Bounding collider is not supposed to be on quadtree");
		}
		else if (c1->typeIndex == box) {
			if (c2->typeIndex == circle) {
				data = _check((CircleCollider*)c2, t2, (BoxCollider2D*)c1, t1);
				data.axis = -data.axis;
			}
			else if (c2->typeIndex == box)
				data = _check((BoxCollider2D*)c1, t1, (BoxCollider2D*)c2, t2);
			else
				throw("Bounding collider is not supposed to be on quadtree");
		}
		else if (c1->typeIndex == aabb) {
			if (c2->typeIndex == circle) {
				data = _check((CircleCollider*)c2, t2, (AABB*)c1, t1);
				data.axis = -data.axis;
			}
			else if (c2->typeIndex == box) {
				data = _check((BoxCollider2D*)c2, t2, (AABB*)c1, t1);
				data.axis = -data.axis;
			}
			else
				throw("invalid colliders");
		}
		else if (c1->typeIndex == bcircle) {
			if (c2->typeIndex == circle) {
				data = _check((CircleCollider*)c2, t2, (BoundingCircle*)c1, t1);
				data.axis = -data.axis;
			}
			else if (c2->typeIndex == box) {
				data = _check((BoxCollider2D*)c2, t2, (BoundingCircle*)c1, t1);
				data.axis = -data.axis;
			}
			else
				throw("invalid colliders");
		}
		else
			throw("invalid colliders");

		//resolve collision
		if (!data.status) {
			data.axis = vec2(0.f);
			return data;
		}

		if (s2 == DYNAMIC) {
			if (s1 == DYNAMIC) {
				data.axis *= 0.5f;
				t1->Position -= data.axis;
			}
			t2->Position += data.axis;
		}
		else {
			if (s1 == DYNAMIC) {
				t1->Position -= data.axis;
			}
		}

		data.axis = data.axis.normalized();
		return data;
	}

	void PhysicsHandler::collisionPhysics(CollisionData data, RigidBody2D* body1, RigidBody2D* body2) {

		if (body2->Type != STATIC) {
			Math::vec2 del = data.axis * Math::dot(body2->Velocity - body1->Velocity, data.axis) * (2.f / (body1->mass + body2->mass));

			body1->Velocity +=  del * body1->coef_restitution * body2->mass;
			body2->Velocity -= del * body2->coef_restitution * body1->mass;
		}
		else {
			body1->Velocity -= data.axis * 2.f * body1->coef_restitution * Math::dot(body1->Velocity, data.axis);
		}
	}

	void PhysicsHandler::updateRigidBody(RigidBody2D* body, Transform* transform, float delta_time) {
		if (body->Type == STATIC) return;
		transform->Position += body->Velocity * delta_time;
		body->Velocity += body->Acceleration *(1.0f - body->coef_drag) * delta_time;

		body->Acceleration = vec2(0.f);
	}
	
	CollisionData PhysicsHandler::_check(CircleCollider* c1, Transform* t1, CircleCollider* c2, Transform* t2)
	{
		vec2 c_axis = (t2->Position + c2->Offset) - (t1->Position + c1->Offset);
		float overlap = c1->Radius + c2->Radius - c_axis.length();
		return CollisionData(overlap > 0.f, c_axis.normalized() * overlap);
	}

	CollisionData PhysicsHandler::_check(CircleCollider* c, Transform* t1, BoxCollider2D* b, Transform* t2)
	{
		float depth = MAX_float;
		vec2 c_axis = vec2();

		Math::vec2 dir = (*t2).Position + (*b).Offset - (*t1).Position - (*c).Offset;
		Math::vec2 naxis = Math::vec2(0.f);
		float len = Math::MAX_float;

		for (int i = 0; i < 4; i++) {
			Math::vec2 v = (*b).Size * ((Math::vec2)UNIT_RECT[i]).rotate((*b).Rotation + (*t2).Rotation) + (*b).Offset + (*t2).Position;

			Math::vec2 temp = v - (*t1).Position - (*c).Offset;
			if (temp.length() < len) {
				naxis = temp;
				len = temp.length();
			}
		}

		float minA = Math::MAX_float, maxA = Math::MIN_float;
		for (int j = 0; j < 4; j++) {
			Math::vec2 v = (*b).Size * ((Math::vec2)UNIT_RECT[j]).rotate((*b).Rotation + (*t2).Rotation) + (*b).Offset + (*t2).Position;
			float dot = Math::dot(v, naxis);
			if (dot < minA) minA = dot;
			if (dot > maxA) maxA = dot;
		}
		float minB = Math::dot((*c).Offset + (*t1).Position, naxis) - (*c).Radius;
		float maxB = minB + 2 * (*c).Radius;

		if (minA > maxB || minB > maxA) return CollisionData();
		if (Math::dot(-naxis, dir) > 0.f) {
			float temp = min(maxB, maxA) - max(minA, minB);
			if (temp < depth) {
				depth = temp;
				c_axis = -naxis;
			}
		}

		for (int i = 0; i < 4; i++) {
			Math::vec2 axis = (Math::vec2)UNIT_RECT[i] - (Math::vec2)UNIT_RECT[(i + 1) % 4];
			axis = axis.rotate((*b).Rotation + (*t2).Rotation);
			axis = Math::vec2(axis.y, -axis.x);

			float minA = Math::MAX_float, maxA = Math::MIN_float;
			for (int j = 0; j < 2; j++) {
				Math::vec2 v = (*b).Size * ((Math::vec2)UNIT_RECT[(j + i + 1) % 4]).rotate((*b).Rotation + (*t2).Rotation) + (*b).Offset + (*t2).Position;
				float dot = Math::dot(v, axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}

			float minB = Math::dot((*c).Offset + (*t1).Position, axis) - (*c).Radius;
			float maxB = minB + 2 * (*c).Radius;

			if (minA > maxB || minB > maxA) return CollisionData();
			if (Math::dot(-axis, dir) > 0.f) {
				float temp = min(maxB, maxA) - max(minA, minB);
				if (temp < depth) {
					depth = temp;
					c_axis = -axis;
				}
			}
		}

		return CollisionData(true, c_axis * (depth / dot(c_axis, c_axis)));
	}

	CollisionData PhysicsHandler::_check(BoxCollider2D* b1, Transform* t1, BoxCollider2D* b2, Transform* t2)
	{
		Math::vec2 c_axis = Math::vec2(0.f);
		Math::vec2 dir = (*t2).Position + (*b2).Offset - (*t1).Position - (*b1).Offset;
		float depth = Math::MAX_float;

		for (int i = 0; i < 4; i++) {
			Math::vec2 axis = (Math::vec2)UNIT_RECT[i] - (Math::vec2)UNIT_RECT[(i + 1) % 4];
			axis = axis.rotate((*b1).Rotation + (*t1).Rotation);
			axis = Math::vec2(-axis.y, axis.x);

			float minA = Math::MAX_float, maxA = Math::MIN_float;
			for (int j = 0; j < 2; j++) {
				Math::vec2 v = (*b1).Size * ((Math::vec2)UNIT_RECT[(j + i + 1) % 4]).rotate((*b1).Rotation + (*t1).Rotation) + (*b1).Offset + (*t1).Position;
				float dot = Math::dot(v, axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}

			float minB = Math::MAX_float, maxB = Math::MIN_float;
			for (int j = 0; j < 4; j++) {
				Math::vec2 v = (*b2).Size * ((Math::vec2)UNIT_RECT[j]).rotate((*b2).Rotation + (*t2).Rotation) + (*b2).Offset + (*t2).Position;
				float dot = Math::dot(v, axis);
				if (dot < minB) minB = dot;
				if (dot > maxB) maxB = dot;
			}

			if (minA >= maxB || minB >= maxA) 
				return CollisionData();

			if (Math::dot(axis, dir) > 0.f) {
				float temp = min(maxB, maxA) - max(minA, minB);
				if (temp < depth) {
					depth = temp;
					c_axis = axis;
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			Math::vec2 axis = (Math::vec2)UNIT_RECT[(i + 1) % 4] - (Math::vec2)UNIT_RECT[i];
			axis = axis.rotate((*b2).Rotation + (*t2).Rotation);
			axis = Math::vec2(-axis.y, axis.x);

			float minA = Math::MAX_float, maxA = Math::MIN_float;
			for (int j = 0; j < 4; j++) {
				Math::vec2 v = (*b1).Size * ((Math::vec2)UNIT_RECT[j]).rotate((*b1).Rotation + (*t1).Rotation) + (*b1).Offset + (*t1).Position;
				float dot = Math::dot(v, axis);
				if (dot < minA) minA = dot;
				if (dot > maxA) maxA = dot;
			}

			float minB = Math::MAX_float, maxB = Math::MIN_float;
			for (int j = 0; j < 2; j++) {
				Math::vec2 v = (*b2).Size * ((Math::vec2)UNIT_RECT[(j + i + 1) % 4]).rotate((*b2).Rotation + (*t2).Rotation) + (*b2).Offset + (*t2).Position;
				float dot = Math::dot(v, axis);
				if (dot < minB) minB = dot;
				if (dot > maxB) maxB = dot;
			}

			if (minA >= maxB || minB >= maxA)
				return CollisionData();
			if (Math::dot(-axis, dir) > 0.f) {
				float temp = min(maxB, maxA) - max(minA, minB);
				if (temp < depth) {
					depth = temp;
					c_axis = -axis;
				}
			}
		}

		Math::vec2 del = c_axis * (depth / Math::dot(c_axis, c_axis));
		
		return CollisionData(true, del);
		
	}

	CollisionData PhysicsHandler::_check(CircleCollider* c1, Transform* t1, AABB* c2, Transform* t2)
	{
		Math::vec2 minB = Math::vec2(UNIT_RECT[0]) * c2->Size + c2->Offset;
		Math::vec2 maxB = Math::vec2(UNIT_RECT[2]) * c2->Size + c2->Offset;

		Math::vec2 c_axis(0.0f);

		Math::vec2 A = t1->Position + c1->Offset;
		Math::vec2 radiusVec(c1->Radius);

		minB = A - minB - radiusVec;
		maxB = A - maxB + radiusVec;

		minB.x = min(0.f, minB.x);
		maxB.x = max(0.f, maxB.x);
		minB.y = min(0.f, minB.y);
		maxB.y = max(0.f, maxB.y);

		c_axis = minB + maxB;

		return CollisionData(c_axis.x!=0 || c_axis.y!=0, c_axis);
	}

	CollisionData PhysicsHandler::_check(CircleCollider* c1, Transform* t1, BoundingCircle* c2, Transform* t2)
	{
		Math::vec2 axis = (t1->Position + c1->Offset) - (t2->Position + c2->Offset);
		float overlap = axis.length() + c1->Radius - c2->Radius;
		
		if (overlap > 0.f) 
			return CollisionData(true, axis.normalized() * overlap);
		else 
			return CollisionData();
	}

	CollisionData PhysicsHandler::_check(BoxCollider2D* c1, Transform* t1, AABB* c2, Transform* t2)
	{
		Math::vec2 axis = (t1->Position + c1->Offset) - (t2->Position + c2->Offset);
		Math::vec2 ax1((axis.x)/abs(axis.x), 0.f);
		Math::vec2 ax2(0.f, (axis.y)/abs(axis.y));

		float max1 = Math::MIN_float, min1 = Math::MAX_float, max2 = Math::MIN_float, min2 = Math::MAX_float;
		for (int i = 0; i < 4; i++) {
			Math::vec2 vert(Math::vec2(UNIT_RECT[i]) * c1->Size + axis);
			float dot1 = Math::dot(vert, ax1);
			float dot2 = Math::dot(vert, ax2);

			max1 = max(max1, dot1);
			max2 = max(max2, dot2);
		}
		Math::vec2 norm = Math::vec2(max1, max2) - c2->Size * 0.5f;
		norm.x = max(0.f, norm.x);
		norm.y = max(0.f, norm.y);

		if(norm.x == 0.f && norm.y == 0.f) return CollisionData();
		return CollisionData(true, norm * (ax1 + ax2));
	}

	CollisionData PhysicsHandler::_check(BoxCollider2D* c1, Transform* t1, BoundingCircle* c2, Transform* t2)
	{
		Math::vec2 axis = (t1->Position + c1->Offset) - (t2->Position = c2->Offset);
		float maxA = Math::MIN_float, minA = Math::MAX_float;
		Math::vec2 norm = axis.normalized();
		for (int i = 0; i < 4; i++) {
			Math::vec2 vert(Math::vec2(UNIT_RECT[i]) * c1->Size + axis);
			float dot = Math::dot(vert, norm);
			maxA = max(dot, maxA);
		}
		float max = maxA - c2->Radius;
		if (max < 0.f)
			return CollisionData();
		return CollisionData(true, norm * max);
	}
}