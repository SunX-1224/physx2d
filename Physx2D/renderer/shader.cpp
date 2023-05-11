#include "shader.h"

namespace Physx2D {

	std::string get_file_content(const char* filename)
	{
		std::ifstream in(filename, std::ios::binary);

		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		throw(errno);
	}

	unsigned int Shader::compile_shader(const char* source, GLenum type) {
		unsigned int  id;
		int success;
		char infoLog[512];

		id = glCreateShader(type);
		glShaderSource(id, 1, &source, NULL);
		glCompileShader(id);

		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (not success) {
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			std::cout << "ERROR while compiling " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader :: LOG : " << infoLog << std::endl;
		}
		return id;
	}

	Shader::Shader() : m_ID(0) {
	}

	Shader::Shader(const char* vertexPath, const char* fragmentPath) {
		unsigned int vertShader, fragShader;

		std::string vertString = get_file_content(vertexPath);
		std::string fragString = get_file_content(fragmentPath);

		const char* vertexCode = vertString.c_str();
		const char* fragmentCode = fragString.c_str();

		vertShader = compile_shader(vertexCode, GL_VERTEX_SHADER);
		fragShader = compile_shader(fragmentCode, GL_FRAGMENT_SHADER);

		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertShader);
		glAttachShader(m_ID, fragShader);
		glLinkProgram(m_ID);


		{// Link status check
			int success;
			char infoLog[512];

			glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
			if (not success) {
				glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
				std::cout << "ERROR while linking program : " << vertexPath << " : " << fragmentPath << " : " << infoLog << std::endl;
			}
		}

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}

	void Shader::use() {
		glUseProgram(m_ID);
	}

	void Shader::del() {
		glDeleteProgram(m_ID);
	}

	void Shader::setBool(const char* name, bool value) {
		glUniform1i(glGetUniformLocation(m_ID, name), (int)value);
	}

	void Shader::setInt(const char* name, int value) {
		glUniform1i(glGetUniformLocation(m_ID, name), value);
	}

	void Shader::setFloat(const char* name, float value) {
		glUniform1f(glGetUniformLocation(m_ID, name), value);
	}

	void Shader::setVec2(const char* name, Math::vec2 vec) {
		glUniform2f(glGetUniformLocation(m_ID, name), vec.x, vec.y);
	}

	void Shader::setMat3(const char* name, Math::mat3 mat) {
		glUniformMatrix3fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, &mat.value[0][0]);
	}
}