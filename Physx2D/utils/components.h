#pragma once

#include "defaults.h"

namespace Physx2D {

	struct Transform {
		Math::vec2 Position;
		Math::vec2 Scale ;
		float Rotation;

		Transform(Math::vec2 position =  Math::vec2(0.0f), Math::vec2 scale = Math::vec2(1.0f), float rotation = 0.f) {
			Position = position;
			Scale = scale;
			Rotation = rotation;
		}
	};

	struct SpriteRenderer {
		Transform transform;
		Math::vec4 color;

		SpriteRenderer(Math::vec4 _color = Math::vec4(1.0f)) :color(_color) {}
	};

	struct RigidBody2D {
		BodyType Type = BodyType::STATIC;
		Transform* transform = NULL;
		
		Math::vec2 Velocity = Math::vec2(0.0f);
		Math::vec2 Acceleration = Math::vec2(0.0f);
		
		float mass = 1.0f;
		float coef_drag = 1.0f;
		float coef_angularDrag = 1.0f;
		float coef_restitution = 1.0f;
		
		float AngularVelocity = 0.0f;
		float AngularAcceleration = 0.0f;
		
		RigidBody2D() = default;

		void addForce(Math::vec2 force);

		void addAcceleration(Math::vec2 acc);

		void clearAcceleration();
	};

	struct Collider {
		ColliderType Type;
		Transform* transform = NULL;
		Math::vec2 Offset;

		Collider(ColliderType type, Math::vec2 offset) :Type(type), Offset(offset) {}
		virtual CollisionData checkCollision(Collider* collider, BodyType type = DYNAMIC);
		virtual CollisionData constraintCollision(Collider* collider);
	};

	struct BoundingCircle : Collider {
		float Radius;

		BoundingCircle(Math::vec2 offset = Math::vec2(0.f), float radius = 0.5f) 
			:Collider(BOUNDING_CIRCLE, offset), Radius(radius) {}
	};

	struct AxisAlignedBB : Collider {
		Math::vec2 Size;

		AxisAlignedBB(Math::vec2 offset = Math::vec2(0.f), Math::vec2 size = Math::vec2(0.f)) 
			:Size(size), Collider(ColliderType::AABB, offset) {}
	};

	struct BoxCollider2D :Collider {
		Math::vec2 Size;
		float Rotation;

		BoxCollider2D(Math::vec2 offset = Math::vec2(0.f), Math::vec2 size = Math::vec2(1.f), float rotation = 0.f) :
			Size(size), Rotation(rotation), Collider(BOX2D, offset) {}

		virtual CollisionData checkCollision(Collider* collider, BodyType type = DYNAMIC) override;
		virtual CollisionData constraintCollision(Collider* collider) override;
	};

	struct CircleCollider :Collider{
		
		float Radius = 0.5f;

		CircleCollider(Math::vec2 offset = Math::vec2(0.f),float radius = 0.5f) :Radius(radius), Collider(CIRCLE, offset){}

		virtual CollisionData checkCollision(Collider* collider, BodyType type = DYNAMIC) override;
		virtual CollisionData constraintCollision(Collider* boundingObject) override;
	};
}