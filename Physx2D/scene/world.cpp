#include "world.h"
#include "entity.h"

namespace Physx2D {
	World::World(Window& targetWindow) {
		window = &targetWindow;
	}

	World::~World()	{
		
		for (auto& iter : renderers) {
			iter.second.del();
		}
		renderers.clear();
		for (auto& iter : shaders) {
			if(iter.second)
				iter.second.reset();;
		}
		shaders.clear();
		for (auto& entity : entities) {
			if (entity->HasComponent<Transform>())
				entity->RemoveComponent<Transform>();
			if (entity->HasComponent<Tag>())
				entity->RemoveComponent<Tag>();
			if (entity->HasComponent<RigidBody2D>())
				entity->RemoveComponent<RigidBody2D>();
			if (entity->HasComponent<RendererComponent>())
				entity->RemoveComponent<RendererComponent>();
			if (entity->HasComponent<ScriptComponent>()) 
				entity->RemoveComponent<ScriptComponent>();
			if (entity->HasComponent<Collider>()) {
				Collider* cldr = entity->GetComponent<Collider>();

				if (cldr->typeIndex == std::type_index(typeid(CircleCollider)))
					delete ((CircleCollider*)cldr);
				else if (cldr->typeIndex == std::type_index(typeid(BoxCollider2D)))
					delete ((BoxCollider2D*)cldr);
				else if (cldr->typeIndex == std::type_index(typeid(BoundingCircle)))
					delete ((BoundingCircle*)cldr);
				else
					delete ((AABB*)cldr);
			}
			delete entity;
		}
		renderData.clear();
	}

	void World::Init() {
		addInstancedRenderer(LINE, initVectorFromArray(LINE_VERTICES, Math::vec2), GL_LINES);
		addInstancedRenderer(TRIANGLE, initVectorFromArray(TRIANGLE_VERTICES, Math::vec2), GL_TRIANGLES);
		addInstancedRenderer(QUAD, initVectorFromArray(QUAD_VERTICES, Math::vec2), GL_TRIANGLES);
		addInstancedRenderer(CIRCLE, initVectorFromArray(QUAD_VERTICES, Math::vec2), GL_TRIANGLES);

		std::shared_ptr<Shader> s1 = std::make_shared<Shader>(Shader("Physx2D/utils/res/vert.glsl", "Physx2D/utils/res/quad.glsl"));
		std::shared_ptr<Shader> s2 = std::make_shared<Shader>(Shader("Physx2D/utils/res/vert.glsl", "Physx2D/utils/res/circle.glsl"));

		shaders[QUAD] = s1;
		shaders[CIRCLE] = s2;
		shaders[TRIANGLE] = s1;
		shaders[LINE] = s1;

		for (auto& entity : entities) {
			if (entity->HasComponent<ScriptComponent>()) {
				entity->GetComponent<ScriptComponent>()->script->setup();
			}
		}
	}

	void World::Update(double delta_time) {
		float time = TIME;
		handleCollisions();
		std::cout << (TIME - time) * 1000<< ", "; time = TIME;
		handleScriptUpdate();
		std::cout << (TIME - time)*1000 << ", "; time = TIME;
		handlePhysics(delta_time);
		std::cout << (TIME - time) *1000<< ", "; time = TIME;
		updateRenderData();
		std::cout << (TIME - time)*1000 << "\n"; time = TIME;
	}

	void World::Render() {
		for (auto& iter : renderers) {
			iter.second.Draw(shaders[iter.first].get());
		}
	}

	Entity* World::CreateEntity(std::string name) {
		EntityID id = manager.CreateEntity();
		Entity* ent = new Entity(id, this);

		ent->AddComponent<Transform>();
		ent->AddComponent<Tag>(name.empty()?"Entity":name);

		entities.push_back(ent);
		return ent;
	}

	void World::loadShader(const char* vert, const char* frag, uint32_t ID) {
		shaders[ID] = std::make_shared<Shader>(Shader(vert, frag));
	}

	InstancedRenderer* World::addInstancedRenderer(uint32_t type, std::vector<Math::vec2> vertices, GLenum draw_mode) {
		bool createNew = true;
		for (auto& iter : renderers) {
			if (iter.first == type) {
				createNew = false;
				break;
			}
		}
		assert(createNew && "Renderer of this type already exists");

		renderers[type] = InstancedRenderer(vertices, draw_mode);
		renderers[type].ArrayDataLayout(0, 2, GL_FLOAT, sizeof(vec2), 0);

		renderers[type].InstanceLayout(1, 2, GL_FLOAT, sizeof(RenderData), 0); //vec2 position
		renderers[type].InstanceLayout(2, 2, GL_FLOAT, sizeof(RenderData), 2 * sizeof(float)); //vec2 scale
		renderers[type].InstanceLayout(3, 1, GL_FLOAT, sizeof(RenderData), 4 * sizeof(float)); // float rotation
		renderers[type].InstanceLayout(4, 4, GL_FLOAT, sizeof(RenderData), 5 * sizeof(float)); //vec4 color

		return &renderers[type];
	}

	InstancedRenderer* World::addInstancedRenderer(uint32_t type, std::vector<Math::vec2> vertices, std::vector<uint32_t> indices, GLenum draw_mode) {
		bool createNew = true;
		for (auto& iter : renderers) {
			if (iter.first == type) {
				createNew = false;
				break;
			}
		}
		assert(createNew && "Renderer of this type already exists");

		renderers[type] = InstancedRenderer(vertices, indices, draw_mode);

		renderers[type].ArrayDataLayout(0, 2, GL_FLOAT, sizeof(vec2), 0);

		renderers[type].InstanceLayout(1, 2, GL_FLOAT, sizeof(RenderData), 0); //vec2 position
		renderers[type].InstanceLayout(2, 2, GL_FLOAT, sizeof(RenderData), 2 * sizeof(float)); //vec2 scale
		renderers[type].InstanceLayout(3, 1, GL_FLOAT, sizeof(RenderData), 4 * sizeof(float)); // float rotation
		renderers[type].InstanceLayout(4, 4, GL_FLOAT, sizeof(RenderData), 5 * sizeof(float)); //vec4 color


		return &renderers[type];
	}

	void World::handleScriptUpdate() {
		for (auto& entity : entities) {
			if (entity->HasComponent<ScriptComponent>()) {
				entity->GetComponent<ScriptComponent>()->script->update();
			}
		}
	}

	void World::handlePhysics(double delta_time) {
		for (auto& entity : entities) {
			if (entity->HasComponent<RigidBody2D>()) {
				PhysicsHandler::updateRigidBody(entity->GetComponent<RigidBody2D>(), entity->GetComponent<Transform>(), delta_time);
			}
		}
	}

	void World::handleCollisions() {
		Math::centerRect treesize = Math::centerRect(Math::vec2(0.f), window->GetResolution());
		Math::vec2 range(10.f, 10.f);
		
		QuadTree<Entity*> qtree(treesize);
		
		//populate quadtree
		for (auto& entity : entities) {
			if (entity->HasComponent<Collider>()) {
				Transform* tfr = entity->GetComponent<Transform>();
				Collider* cld = entity->GetComponent<Collider>();

				if (cld->typeIndex == std::type_index(typeid(AABB)) || cld->typeIndex == std::type_index(typeid(BoundingCircle)))
					continue;
				
				range.x = max(range.x, cld->getSize().x);
				range.y = max(range.y, cld->getSize().y);
				qtree.insert(tfr->Position + cld->Offset, entity);
			}
		}
		range *= 2.f;

		//collision check 
		
		for (auto& entity : entities) {
			if (entity->HasComponent<Collider>()) {
				Transform* tfr = entity->GetComponent<Transform>();
				Collider* cld = entity->GetComponent<Collider>();
				RigidBody2D* rgb = entity->GetComponent<RigidBody2D>();

				std::vector<Entity*> neighbours;

				bool isBound = (cld->typeIndex == std::type_index(typeid(AABB))) || (cld->typeIndex == std::type_index(typeid(BoundingCircle)));

				qtree.query(Math::centerRect(tfr->Position + cld->Offset, isBound?cld->getSize() : range), neighbours);

				for (auto& nent : neighbours) {
					if (nent == entity) continue;
					
					Transform* ntfr = nent->GetComponent<Transform>();
					Collider* ncld = nent->GetComponent<Collider>();
					RigidBody2D* nrgb = nent->GetComponent<RigidBody2D>();

					CollisionData data = PhysicsHandler::checkCollision(cld, tfr, rgb->Type, ncld, ntfr, nrgb->Type);
					if (data.status) {
						if (rgb->Type == DYNAMIC)
							PhysicsHandler::collisionPhysics(data, rgb, nrgb);
						else if (nrgb->Type == DYNAMIC)
							PhysicsHandler::collisionPhysics(data, nrgb, rgb);

						if (entity->HasComponent<ScriptComponent>()) {
							entity->GetComponent<ScriptComponent>()->script->OnCollisionDetected(data, nent);
						}
					}
				}
			}
		}
	}

	void World::updateRenderData() {
		for (auto& vec : renderData)
			vec.second.clear();

		for (auto& entity : entities) {
			if (entity->HasComponent<RendererComponent>()) {
				Transform* trnf = entity->GetComponent<Transform>();
				RendererComponent* rnc = entity->GetComponent<RendererComponent>();
				Collider* cld = entity->GetComponent<Collider>();
				renderData[rnc->type].push_back(RenderData(*trnf, rnc->color)); // TODO : can be optimized
			}
		}

		for (auto& iter : shaders) {
			iter.second->use();
			iter.second->setVec2("u_resolution", window->GetResolution());
			iter.second->setMat3("u_ortho", Math::get_ortho2d(vec2(0.f, 0.f), window->GetResolution()));
			iter.second->setFloat("u_time", Time::get_time());
		}
		
		for (auto& renderer : renderers) {
			auto& rd = renderData[renderer.first];
			renderer.second.InstanceData(rd.data(), rd.size(), sizeof(RenderData));
		}
	}
}
