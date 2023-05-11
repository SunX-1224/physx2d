#pragma once

#include <glad/glad.h>

namespace Physx2D {
	class VAO {
	public:
		GLuint m_ID;

		VAO();

		void layout(GLuint location, GLuint size, GLenum type, GLuint stride, unsigned int offset);
		void bind();
		void unbind();
		void del();
	};
}