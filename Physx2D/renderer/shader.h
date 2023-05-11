#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>

#include "../math/math.h"

namespace Physx2D {

	std::string get_file_content(const char*);

	class Shader {
	public:
		unsigned int m_ID;

		Shader();
		Shader(const char* vertexPath, const char* fragmentPath);

		void use();
		void del();
		void setBool(const char*, bool);
		void setInt(const char*, int);
		void setFloat(const char*, float);
		void setVec2(const char*, Math::vec2);
		void setMat3(const char*, Math::mat3);
		static unsigned int compile_shader(const char*, GLenum);
	};
}