#include "Physx2D/physx2d.h"

struct Link {
	int p1;
	int p2;

	float length;
};
class SoftBody :public Physx2D::ScriptObject {
private:
	std::vector<Physx2D::Entity*> linkRen;

public:
	std::vector<Physx2D::Entity*> particles;
	std::vector<Link> links;

	Physx2D::Math::vec2 pos;
	float radius;
	int n_particles;

	SoftBody(Physx2D::Math::vec2 pos, float radius = 100.f ,int n_particles = 30) {
		this->radius = radius;
		this->pos = pos;
		this->n_particles = n_particles;
		particles.reserve(n_particles);
		linkRen.reserve(n_particles);
		links.reserve(n_particles);
	}

	virtual void setup() override {

		float del = 2.f*Physx2D::Math::PI / n_particles;
		for (int i = 0; i < n_particles; i++) {
			Physx2D::Entity* ent = self->m_world->CreateEntity();
			ent->Add_ReplaceComponent<Physx2D::Transform>(pos + Physx2D::Math::vec2(cos(del * i), sin(del * i)) * radius, Physx2D::Math::vec2(8.f));
			ent->AddComponent<Physx2D::RendererComponent>(Physx2D::CIRCLE, Physx2D::Color(.5f, .1f, .1f, 1.0f));
			ent->AddComponent<Physx2D::CircleCollider>(Physx2D::Math::vec2(), 4.f);
			ent->AddComponent<Physx2D::RigidBody2D>(Physx2D::DYNAMIC, Physx2D::Math::vec2(), Physx2D::Math::vec2(), 10.f, 0.f, 1.f);

			particles.push_back(ent);
			
			links.push_back({i, (i+1)% n_particles, 50.f});


			Physx2D::Entity* link = self->m_world->CreateEntity();
			link->AddComponent<Physx2D::RendererComponent>(Physx2D::LINE, Physx2D::Color(0.5f));
			linkRen.push_back(link);
		}
	}

	virtual void update(float delta_time) override {
		for (int i = 0; i < n_particles; i++) {
			Physx2D::Math::vec2 pos1 = particles[links[i].p1]->GetComponent<Physx2D::Transform>()->Position;
			Physx2D::Math::vec2 del = particles[links[i].p2]->GetComponent<Physx2D::Transform>()->Position - pos1;

			

			Physx2D::RigidBody2D* rgb1 = particles[links[i].p1]->GetComponent<Physx2D::RigidBody2D>();
			Physx2D::RigidBody2D* rgb2 = particles[links[i].p2]->GetComponent<Physx2D::RigidBody2D>();

			Physx2D::Math::vec2 force = del.normalized() * (del.length() - links[i].length) * 500.f;
			
			force += (rgb2->Velocity - rgb1->Velocity) * 80.f;
			rgb1->Acceleration += force;
			rgb2->Acceleration -= force;

			Physx2D::Math::vec2 pos = particles[links[i].p1]->GetComponent<Physx2D::Transform>()->Position;
			Physx2D::Math::vec2 l_vec = particles[links[i].p2]->GetComponent<Physx2D::Transform>()->Position - pos;

			particles[i]->GetComponent<Physx2D::RigidBody2D>()->Acceleration += Physx2D::Math::vec2(50.f, -100.f);

			Physx2D::Transform* tfr = linkRen[i]->GetComponent<Physx2D::Transform>();
			tfr->Position = pos;
			tfr->Scale = l_vec.length();
			tfr->Rotation = atan2(l_vec.y, l_vec.x);
		}
	}
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
	world.CreateEntity()->AddComponent<Physx2D::ScriptComponent>(new SoftBody(Physx2D::Math::vec2()));

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
