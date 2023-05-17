#include "Physx2D/physx2d.h"

//#define DEBUG
#ifdef DEBUG
struct AllocationViewer {
	uint32_t allocated = 0;
	uint32_t freed = 0;
	
	uint32_t usage() { return allocated - freed; }
};

static AllocationViewer tracker;

void printUsage() {
	std::cout << "Usage : " << tracker.usage() << " , allocated : " << tracker.allocated << " , freed : " << tracker.freed << std::endl;
}

void* operator new(size_t size) {
	tracker.allocated += size;
	return malloc(size);
}

void operator delete(void* mem, size_t size) {
	tracker.freed += size;
	free(mem);
}
#endif

class Gravity : public Physx2D::ScriptObject {
	public:

		virtual void setup() override {

			Physx2D::Math::vec2 rp = Physx2D::Math::vec2(Physx2D::Math::randint(-400, 400), Physx2D::Math::randint(-400, 400));
			Physx2D::Math::vec2 sc = Physx2D::Math::vec2(Physx2D::Math::random(1224) * 5.f + 3.f);

			Physx2D::Transform* tfr = self->GetComponent<Physx2D::Transform>();
			tfr->Position = rp;
			tfr->Scale = sc;

			self->AddComponent<Physx2D::RigidBody2D>(Physx2D::DYNAMIC, rp, Physx2D::Math::vec2(), sc.x, 0.f, 0.8f);
			self->AddComponent<Physx2D::CircleCollider>(Physx2D::Math::vec2(), sc.x * 0.5f);
			self->AddComponent<Physx2D::RendererComponent>(Physx2D::CIRCLE, Physx2D::Math::vec4(Physx2D::Math::random(1224), Physx2D::Math::random(1223), 0.5f, 1.f));
		}

		virtual void update() override {
			
			if (self->HasComponent<Physx2D::RigidBody2D>()) {
				Physx2D::RigidBody2D* rgb = self->GetComponent<Physx2D::RigidBody2D>();
				Physx2D::Transform* trf= self->GetComponent<Physx2D::Transform>();
				rgb->Acceleration = Physx2D::Math::vec2(0, -100.f);
			}
			if (self->HasComponent<Physx2D::RendererComponent>()) {
				Physx2D::RendererComponent* rdr1 = self->GetComponent<Physx2D::RendererComponent>();
				rdr1->color += Physx2D::Color(-0.1f ,0.f, 0.2f, 0.f);
			}
		}

		virtual void OnCollisionDetected(Physx2D::CollisionData& data, Physx2D::Entity* other) {
			Physx2D::RendererComponent* rdr1 = self->GetComponent<Physx2D::RendererComponent>();
			Physx2D::RendererComponent* rdr2 = other->GetComponent<Physx2D::RendererComponent>();

			rdr1->color += Physx2D::Color(0.05f, 0.f, -0.1f, 0.f);
			rdr2->color += Physx2D::Color(0.05f, 0.f, -0.1f, 0.f);
		}
};

int main() {

	//new scope for tracking memory
	{
		Physx2D::Window window;
		window.Init();

		Physx2D::World world(window);

#ifdef DEBUG
		printUsage();
#endif

		for (int i = 0; i < 1500; i++) {
			Physx2D::Entity* entity = world.CreateEntity();

			Gravity* grv = new Gravity();
			entity->AddComponent<Physx2D::ScriptComponent>(grv);
		}

		Physx2D::Entity* entity = world.CreateEntity();
		Physx2D::Transform* tfr = entity->GetComponent<Physx2D::Transform>();
		tfr->Scale = window.GetResolution();
		entity->AddComponent<Physx2D::AABB>(Physx2D::Math::vec2(), tfr->Scale);
		entity->AddComponent<Physx2D::RigidBody2D>();
		entity->AddComponent<Physx2D::RendererComponent>(Physx2D::QUAD, Physx2D::Color(0.3f));


#ifdef DEBUG
		printUsage();
#endif
		world.Init();

#ifdef DEBUG
		printUsage();
#endif

		Physx2D::Time::initTimer();
		while (!window.ShouldClose()) {
			window.UpdateEvents();

			world.Update(Physx2D::Time::get_delta_time());

			window.FillScreen(Physx2D::Color(0.1f, 0.1f, 0.1f, 1.0f));
			world.Render();

			
			window.Update();
			Physx2D::Time::update();
			window.SetTitle((std::to_string(Physx2D::Time::get_fps())).c_str());

#ifdef DEBUG
			printUsage();
#endif
		}
		window.Destroy();
	}

#ifdef DEBUG
	printUsage();
#endif

	return 0;
}
