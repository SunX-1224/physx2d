#pragma once
#include <GLFW/glfw3.h>

namespace Physx2D {
	class Time {
		public:
			static void initTimer();
			static double get_time();
			static int get_fps();
			static double get_delta_time();
			static void update();
		private:
			static double current_time, last_time;
	};
}
