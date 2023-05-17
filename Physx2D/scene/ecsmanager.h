#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <cassert>
#include "components.h"

namespace Physx2D {
	using EntityID = uint32_t;

	class ECSManager {

	public:

		const EntityID  CreateEntity() {
			const EntityID entity = nextEntity++;
			return entity;
		}

		template<typename T>
		T* addComponent(EntityID entity, T* component) {
			assert(entity >= 0 && entity < nextEntity && "Invalid entity ID");

			std::vector<void*>& componentVector = componentStorage[std::type_index(typeid(T))];
			if (entity >= componentVector.size()) {
				componentVector.resize(entity + 16, nullptr);
			}

			if (componentVector[entity] != nullptr) delete ((T*)componentVector[entity]);

			componentVector[entity] = component;
			return static_cast<T*>(componentVector[entity]);
		}

		template<typename T>
		void removeComponent(EntityID entity) {
			assert(entity >= 0 && entity < nextEntity && "Invalid entity ID");

			std::vector<void*>& componentVector = componentStorage[std::type_index(typeid(T))];
			delete ((T*)componentVector[entity]);
		}

		template<typename T>
		bool hasComponent(EntityID entity) {

			std::vector<void*>& componentVector = componentStorage[std::type_index(typeid(T))];
			return entity < componentVector.size() && componentVector[entity] ;
		}

		template<typename T>
		T* getComponent(EntityID entity) {
			
			assert(entity >= 0 && entity < nextEntity && "Invalid entity ID");

			std::vector<void*>& componentVector = componentStorage[std::type_index(typeid(T))];

			if (entity < componentVector.size()) {
				return static_cast<T*>(componentVector[entity]);
			}
			T* cptr = nullptr;
			return cptr;
		}

		template<typename T>
		const std::vector<void*>& getAllComponents() {
			return componentStorage[std::type_index(typeid(T))];
		}

		~ECSManager() {
			componentStorage.clear();
		}

	private:
		EntityID nextEntity = 0;
		
		std::unordered_map<std::type_index, std::vector<void*>> componentStorage;

	};
}