#pragma once

#include "../utils/defaults.h"
#include <typeindex>

namespace Physx2D {
	class Entity;
	enum BodyType { STATIC = 0, DYNAMIC };
	enum RenderType {LINE = 0, TRIANGLE, QUAD, CIRCLE};

	class ScriptObject {
	public:
		Entity* self = nullptr;

		ScriptObject() {}

		virtual void setup() = 0;
		virtual void update(float delta_time) = 0;
		virtual void OnCollisionDetected(CollisionData& data, Entity* other) {}
	};
	
	struct Tag {
		std::string tag;

		Tag(std::string _tag = std::string()) :tag(_tag) {}
	};

	struct Transform {

		Math::vec2 Position;
		Math::vec2 Scale;
		float Rotation;

		Transform(Math::vec2 position = Math::vec2(0.0f), Math::vec2 scale = Math::vec2(1.0f), float rotation = 0.f) {
			Position = position;
			Scale = scale;
			Rotation = rotation;
		}
	};

	struct RendererComponent {

		RenderType type;
		Math::vec4 color;

		RendererComponent(RenderType _type = QUAD, Math::vec4 _color = Math::vec4(1.0f)) :type(_type), color(_color) {}
	};

	struct RenderData {
		Transform transform;
		Color color;
	};

	struct RigidBody2D {
		BodyType Type;
		Math::vec2 Velocity;
		Math::vec2 Acceleration;

		float mass ;
		float coef_drag;
		float coef_restitution;

		RigidBody2D(BodyType type = STATIC,Math::vec2 v = Math::vec2(), Math::vec2 a = Math::vec2(), float m = 1.0f, float cd = 0.0f, float cr = 1.0f) 
			: Velocity(v), Acceleration(a), mass(m), coef_drag(cd), coef_restitution(cr), Type(type) {}
	};

	struct Collider {

		std::type_index typeIndex;
		Math::vec2 Offset;
		
		Collider(Math::vec2 off, std::type_index ti) : Offset(off), typeIndex(ti) {}

		virtual Math::vec2 getSize() = 0;
	};

	struct BoundingCircle : Collider{

		float Radius;

		BoundingCircle(Math::vec2 offset = Math::vec2(0.f), float radius = 0.5f)
			:Radius(radius), Collider(offset, typeid(BoundingCircle)) {}

		virtual Math::vec2 getSize() override {
			return Math::vec2(Radius*2.f);
		}
	};

	struct AABB : Collider{

		Math::vec2 Size;

		AABB(Math::vec2 offset = Math::vec2(0.f), Math::vec2 size = Math::vec2(1.f))
			:Size(size), Collider(offset, typeid(AABB)) {}

		virtual Math::vec2 getSize() override {
			return Size;
		}
	};

	struct BoxCollider2D : Collider {

		Math::vec2 Size;
		float Rotation;

		BoxCollider2D(Math::vec2 offset = Math::vec2(0.f), Math::vec2 size = Math::vec2(1.f), float rotation = 0.f) 
			:Size(size), Rotation(rotation), Collider(offset, typeid(BoxCollider2D)) {}

		virtual Math::vec2 getSize() override {
			return Size;
		}
	};

	struct CircleCollider : Collider{

		float Radius;

		CircleCollider(Math::vec2 offset = Math::vec2(0.f), float radius = 0.5f) 
			:Radius(radius), Collider(offset, typeid(CircleCollider)) {}

		virtual Math::vec2 getSize() override {
			return Math::vec2(Radius*2.f);
		}
	};

	struct ScriptComponent {
		ScriptObject* script;

		ScriptComponent(ScriptObject* _script = nullptr) : script(_script) {}

		~ScriptComponent() {
			delete script;
		}
	};
}