#include "math.h"

namespace Physx2D{
	namespace Math {
		float dot(vec2 u, vec2 v){
			return (u.x * v.x + u.y * v.y);
		}

		mat3 get_ortho2d(vec2 btm_left, vec2 top_right) {
			mat3 ortho = mat3(1.0f);
			ortho.value[0][0] = 2.0f / (top_right.x - btm_left.x);
			ortho.value[1][1] = 2.0f / (top_right.y - btm_left.y);

			ortho.value[0][2] = -1.0f;
			ortho.value[1][2] = -1.0f;
			return ortho;
		}

		float random(uint32_t seed) {
			uint32_t r = seed *seed* 637432847 + 748357345;
			r *= r + 78457834;
			r *= (Time::get_time() * 1000000000);
			float rn = (float)r / 0xffffffff;
			return rn;
		}
		int randint(int a, int b){
			float x = (random(a) + random(b) + random(Time::get_time() * 1000000))/3.f;
			return a + (int)(x * (b - a));
		}
	}

}