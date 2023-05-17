#pragma once

#include <GLFW/glfw3.h>
#include "../math/math.h"
#include <vector>

#define SCREEN_WIDTH 1536
#define SCREEN_HEIGHT 800
#define TIME glfwGetTime()
#define arrayLength(a) (sizeof(a)/sizeof(a[0]))
#define initVectorFromArray(arr, type) std::vector<type>(arr, arr + arrayLength(arr))
#define max(a, b) (a>b?a:b)
#define min(a, b) (a<b?a:b)

namespace Physx2D {
	typedef Math::vec4 Color;

	const Math::vec2 QUAD_VERTICES[6] {
			Math::vec2(-.5f, -.5f),
			Math::vec2( .5f, -.5f),
			Math::vec2(-.5f,  .5f),

			Math::vec2( .5f, -.5f),
			Math::vec2(-.5f,  .5f),
			Math::vec2( .5f,  .5f)
	};

	const Math::vec2 UNIT_RECT[4]{
			Math::vec2(-.5f, -.5f),
			Math::vec2( .5f, -.5f),
			Math::vec2( .5f,  .5f),
			Math::vec2(-.5f,  .5f),
	};

	const Math::vec2 TRIANGLE_VERTICES[3] {
		Math::vec2(-0.866f, -0.5f),
		Math::vec2( 0.866f, -0.5f),
		Math::vec2( 0.000f,  1.0f)
	};

	const Math::vec2 LINE_VERTICES[2]{
		Math::vec2(0.f),
		Math::vec2(1.0f, 0.f)
	};

	struct WindowProps {
		uint32_t WIDTH, HEIGHT;
		bool RESIZABLE, FULLSCREEN_MODE;
		const char* TITLE;

		WindowProps(
			uint32_t width = SCREEN_WIDTH,
			uint32_t height = SCREEN_HEIGHT,
			const char* title = "Physx2D Window",
			bool resizable = true,
			bool fullscreen = false
		) {
			WIDTH = width;
			HEIGHT = height;
			TITLE = title;
			RESIZABLE = resizable;
			FULLSCREEN_MODE = fullscreen;
		}
	};

	struct CollisionData {
		bool status;
		Math::vec2 axis;

		CollisionData(bool status = false, Math::vec2 axis = Math::vec2(0.f)) {
			this->status = status;
			this->axis = axis;
		}
	};
}