#include "Physx2D/physx2d.h"

class Pendulum : public Physx2D::ScriptObject {
	public:
		Physx2D::Math::vec2 point;
		float length;
		float angle;

		Pendulum(float length, float angle, Physx2D::Math::vec2 point = Physx2D::Math::vec2(0.f, 400.f)) {
			this->length = length;
			this->point = point;
			this->angle = angle;
		}

		virtual void setup() override {
			Physx2D::Math::vec2 bpos = point + Physx2D::Math::vec2(cos(angle), sin(angle)) * length;

			bob = self->m_world->CreateEntity();
			Physx2D::Transform *btrf = bob->GetComponent < Physx2D::Transform>();
			btrf->Position = bpos;
			btrf->Scale = 30.f;
			bob->AddComponent<Physx2D::RigidBody2D>(Physx2D::DYNAMIC, Physx2D::Math::vec2(0.f))->coef_drag = .1f;
			bob->AddComponent<Physx2D::RendererComponent>(Physx2D::CIRCLE, Physx2D::Color(0.f, 0.3f, 0.3f, 1.0f));
			bob->AddComponent<Physx2D::CircleCollider>(Physx2D::Math::vec2(), btrf->Scale.x*0.5f);

			wire = self->m_world->CreateEntity();
			Physx2D::Transform* wtrf = wire->GetComponent<Physx2D::Transform>();
			wtrf->Position = point;
			wtrf->Scale = length;
			wtrf->Rotation = angle;
			wire->AddComponent<Physx2D::RendererComponent>(Physx2D::LINE, Physx2D::Color(1.f, 0.f, 0.1f, 1.0f));
		}

		virtual void update(float delta_time) override {

			Physx2D::RigidBody2D* rgb = bob->GetComponent<Physx2D::RigidBody2D>();

			Physx2D::Transform* btfr = bob->GetComponent<Physx2D::Transform>();
			Physx2D::Math::vec2 del = point - btfr->Position;
			Physx2D::Math::vec2 tension = del.normalized() * (del.length() - length) * 50.f;

			rgb->Acceleration = Physx2D::Math::vec2(0.f, -100.f) + tension;

			Physx2D::Transform* wtfr = wire->GetComponent<Physx2D::Transform>();
			wtfr->Rotation = atan2(-del.y, -del.x);
			wtfr->Scale = del.length();
		}

		virtual void OnCollisionDetected(Physx2D::CollisionData& data, Physx2D::Entity* other) {
			
		}
	private :
		Physx2D::Entity* bob;
		Physx2D::Entity* wire;

};

int main() {
	
	Physx2D::Window window;
	window.Init();

	Physx2D::World world(window);

	//Bounds
	{
		Physx2D::Entity* entity = world.CreateEntity();
		Physx2D::Transform* tfr = entity->GetComponent<Physx2D::Transform>();
		tfr->Scale = window.GetResolution();
		entity->AddComponent<Physx2D::AABB>(Physx2D::Math::vec2(), tfr->Scale);
		entity->AddComponent<Physx2D::RigidBody2D>();
		entity->AddComponent<Physx2D::RendererComponent>(Physx2D::QUAD, Physx2D::Color(0.3f));
	}

	Physx2D::Entity* pendulum = world.CreateEntity();
	pendulum->AddComponent<Physx2D::ScriptComponent>(new Pendulum(400.f, -0.6f));

	world.Init();

	Physx2D::Time::initTimer();
	while (!window.ShouldClose()) {
		window.UpdateEvents();

		world.Update(Physx2D::Time::get_delta_time());

		window.FillScreen(Physx2D::Color(0.1f, 0.1f, 0.1f, 1.0f));
		world.Render();

			
		window.Update();
		Physx2D::Time::update();
		window.SetTitle((std::to_string(Physx2D::Time::get_fps())).c_str());
	}
	window.Destroy();
	

	return 0;
}
