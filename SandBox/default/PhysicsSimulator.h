#pragma once

//#define DEBUG
#include "../../Physx2D/physx2d.h"

#include <map>
#include <vector>

using namespace Physx2D;
using namespace Math;

enum Type{CIRCLE = 0, QUAD, BoxBound, CircleBound};

struct PhysicsBody {
	Transform* transform;
	RigidBody2D* rigidbody2d;
	Collider* collider;
	SpriteRenderer* spriteRenderer;

	PhysicsBody(Transform* _transform = NULL, RigidBody2D* _rigidbody2d = NULL, Collider* _collider = NULL, SpriteRenderer* _spriteRenderer = NULL)
		: transform(_transform), rigidbody2d(_rigidbody2d), collider(_collider), spriteRenderer(_spriteRenderer)
	{
		rigidbody2d->transform = transform;
		collider->transform = transform;
		spriteRenderer->transform = *transform;
	}

	void del() {
		delete transform;
		delete rigidbody2d;
		delete collider;
		delete spriteRenderer;
	}
};

class PhysicsSimulator {
	public:
		static std::map<int, Shader> Shaders;
		static std::map<int, std::vector<PhysicsBody>> Objects;

		//static FrameBuffer frame1;

		static Window AppWindow;

		static void Init();
		static void Run();
		static void Draw();

		static void Clear();

	private:
		static std::map<int, std::vector<SpriteRenderer>> spriteRenderers;
		static std::map<int, InstancedRenderer> renderers;
		static double last_time;

		static void LoadShader(const char* vert, const char* frag, int name);
		static PhysicsBody* AddObject( int type);
		static void Update(float delta_time);
		static void UpdateRenderData();
		static void ProcessInput();
};
