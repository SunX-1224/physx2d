#pragma once

#include "renderer2D.h"

namespace Physx2D {
	class InstancedRenderer : public Renderer2D {
	public:
		uint32_t m_count;

		InstancedRenderer();
		InstancedRenderer(
			std::vector<Math::vec2> vertices,
			std::vector<uint32_t> indices,
			GLenum mode = GL_TRIANGLES
		);

		void InstanceLayout(
			uint32_t location,
			uint32_t num_data,
			GLenum type,
			GLsizei stride,
			uint32_t offset,
			uint32_t divisor = 1
		);

		void InstanceData(void* data, uint32_t count, size_t size_i);
		void Draw(Shader& shader);
		void del();

	protected:
		uint32_t m_ivbo;
	};
}