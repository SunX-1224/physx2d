#pragma once

#include "world.h"
#include "components.h"

namespace Physx2D {
	class World;

	class Entity {
		public:
			Entity() = default;
			Entity(EntityID id, World* world_);

			template<typename T, typename... Args>
			T* AddComponent(Args... args) {

				if constexpr(std::is_base_of_v<Collider, T>) {
					assert(!m_world->manager.hasComponent<Collider>(m_ID) && "Component already exists");
					return  static_cast<T*>(m_world->manager.addComponent<Collider>(m_ID, new T(args...)));
				}
				else {
					assert(!m_world->manager.hasComponent<T>(m_ID) && "Component already exists");
					T* comp = new T(args...);
					if constexpr (std::is_same_v<T, ScriptComponent>) {
						comp->script->self = this;
					}
					
					return  m_world->manager.addComponent<T>(m_ID, comp);
				}
			}

			template<typename T, typename... Args>
			T* Add_ReplaceComponent(Args... args) {
				if constexpr(std::is_base_of_v<Collider, T>) 
					return m_world->manager.addComponent<Collider>(m_ID, new T(args...));
				else {
					T* comp = new T(args...);
					if constexpr (std::is_same_v<T, ScriptComponent>)
						comp->script->self = this;
					return m_world->manager.addComponent<T>(m_ID, comp);
				}
			}

			template<typename T>
			void RemoveComponent() {
				
				if constexpr(std::is_base_of_v<Collider, T>) {
					assert(m_world->manager.hasComponent<Collider>(m_ID) && "Component doesnot exist");
					m_world->manager.removeComponent<Collider>(m_ID);
				}
				else {
					assert(m_world->manager.hasComponent<T>(m_ID) && "Component doesnot exist");
					m_world->manager.removeComponent<T>(m_ID);
				}
			}

			template<typename T>
			bool HasComponent() {
				if constexpr(std::is_base_of_v<Collider, T>)
					return m_world->manager.hasComponent<Collider>(m_ID);
				return m_world->manager.hasComponent<T>(m_ID);
			}

			template<typename T>
			T* GetComponent() {
				T* cptr;
				if constexpr(std::is_base_of_v<Collider, T>) {
					cptr = m_world->manager.getComponent<Collider>(m_ID);
					assert(cptr != nullptr && "Invalid Component");
				}
				else {
					cptr = m_world->manager.getComponent<T>(m_ID);
					assert(cptr != nullptr && "Invalid Component");
				}
				return cptr;
			}

			bool operator == (Entity nen) {
				return m_ID == nen.m_ID;
			}

		private:
			EntityID m_ID = 0;
			World* m_world = nullptr;
	};
}