#include "instancedRenderer.h"

namespace Physx2D {

	InstancedRenderer::InstancedRenderer() {
		m_count = 0;
		glGenBuffers(1, &m_ivbo);
	}

	InstancedRenderer::InstancedRenderer(std::vector<Math::vec2> vertices, std::vector<uint32_t> indices, GLenum mode) :
		Renderer2D(vertices, indices, mode)
	{
		m_count = 1;
		glGenBuffers(1, &m_ivbo);
	}

	void InstancedRenderer::InstanceLayout(uint32_t location, uint32_t num_data, GLenum type, GLsizei stride, uint32_t offset, uint32_t divisor) {
		m_vao.bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);

		m_vao.layout(location, num_data, type, stride, offset);
		glVertexAttribDivisor(location, divisor);

		m_vao.unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void InstancedRenderer::InstanceData(void* data, uint32_t count, size_t size_i) {
		m_count = count;
		m_vao.bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_ivbo);
		glBufferData(GL_ARRAY_BUFFER, size_i * count, data, GL_DYNAMIC_DRAW);
		m_vao.unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void InstancedRenderer::Draw(Shader& shader) {
		shader.use();
		m_vao.bind();
		glDrawElementsInstanced(m_renderMode, p_indicesCount, GL_UNSIGNED_INT, 0, m_count);
		m_vao.unbind();
	}
	void InstancedRenderer::del(){
		glDeleteBuffers(1, &m_ivbo);
	}
}