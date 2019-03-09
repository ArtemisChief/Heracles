#include "shader.h"
#include <iostream>

namespace heracles {

	// ʹ����ɫ��
	shader &shader::use() {
		glUseProgram(this->id);
		return *this;
	}

	// ������ɫ��
	void shader::compile(const GLchar* vertex_source, const GLchar* fragment_source) {
		// ������ɫ��
		const auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_source, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		// Ƭ����ɫ��
		const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragment_source, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		// ������ɫ������
		this->id = glCreateProgram();
		glAttachShader(this->id, vertex);
		glAttachShader(this->id, fragment);
		glLinkProgram(this->id);
		check_compile_errors(this->id, "PROGRAM");

		// ����������ɣ�ɾ����ɫ��
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	// ����Uniform
	void shader::set_int(const GLchar *name, const GLint value) const {
		glUniform1i(glGetUniformLocation(this->id, name), value);
	}
	void shader::set_float(const GLchar *name, const GLfloat value) const {
		glUniform1f(glGetUniformLocation(this->id, name), value);
	}
	void shader::set_vec2(const GLchar *name, const vec2& value) const {
		glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]);
	}
	void shader::set_mat22(const GLchar *name, const mat22& mat) const {
		glUniformMatrix2fv(glGetUniformLocation(this->id, name), 1, GL_TRUE, &mat[0][0]);
	}

	// �����ɫ�����롢�����Ƿ����
	void shader::check_compile_errors(const GLuint shader, const std::string type) {
		GLint success;
		GLchar info_log[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
					<< info_log << "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
					<< info_log << "\n -- --------------------------------------------------- -- "
					<< std::endl;
			}
		}
	}
}