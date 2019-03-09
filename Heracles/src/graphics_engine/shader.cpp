#include "shader.h"
#include <iostream>

namespace heracles {

	// 使用着色器
	shader &shader::use() {
		glUseProgram(this->id);
		return *this;
	}

	// 编译着色器
	void shader::compile(const GLchar* vertex_source, const GLchar* fragment_source) {
		// 顶点着色器
		const auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_source, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		// 片段着色器
		const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragment_source, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		// 链接着色器程序
		this->id = glCreateProgram();
		glAttachShader(this->id, vertex);
		glAttachShader(this->id, fragment);
		glLinkProgram(this->id);
		check_compile_errors(this->id, "PROGRAM");

		// 程序链接完成，删除着色器
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	// 设置Uniform
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

	// 检测着色器编译、链接是否出错
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