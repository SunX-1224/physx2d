#pragma once

#include<glad/glad.h>
#include "../math/math.h"
#include <vector>

namespace Physx2D {

	template<class T>
	struct Data {
		Math::vec2 key;
		T data;

		Data() {}
		Data(Math::vec2 key_, T data_) :key(key_), data(data_) {}
		
		void operator =(Data<T> data_) {
			key = data_.key;
			data = data_.data;
		}
	};

	template<class T>
	class QuadTree {
		public:
			static const int MAX = 8;
			Data<T> data[MAX];
			int size = 0;
			Math::centerRect bounds;

			QuadTree* NW = NULL;
			QuadTree* NE = NULL;
			QuadTree* SW = NULL;
			QuadTree* SE = NULL;

			QuadTree(Math::centerRect bounds_) :bounds(bounds_) {
			}

			bool insert(Math::vec2 point, T data_) {

				if (!bounds.contains(point)) 
					return false;

				if (size < MAX && NW == NULL) {
					data[size++] = Data<T>(point, data_);
					return true;
				}
				if(NW == NULL)
					this->subdivide();

				if (SW->insert(point, data_)) return true;
				if (SE->insert(point, data_)) return true;
				if (NE->insert(point, data_)) return true;
				if (NW->insert(point, data_)) return true;

				return false;
			}

			void subdivide() {
				NW = new QuadTree(bounds.getPart(-0.5f,  0.5f));
				NE = new QuadTree(bounds.getPart( 0.5f,  0.5f));
				SW = new QuadTree(bounds.getPart(-0.5f, -0.5f));
				SE = new QuadTree(bounds.getPart( 0.5f, -0.5f));
			}

			void query(Math::centerRect range, std::vector<T>& result) {

				if (!bounds.intersects(range))
					return;

				for (int i = 0; i < size; i++) {
					if (range.contains(data[i].key))
						result.push_back(data[i].data);
				}

				if (NW == NULL)
					return;

				SW->query(range, result);
				SE->query(range, result);
				NE->query(range, result);
				NW->query(range, result);
			}

			~QuadTree() {
				delete NW;
				delete NE;
				delete SE;
				delete SW;
			}
	};
}