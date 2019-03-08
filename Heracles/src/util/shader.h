#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "math.h"

class shader {
public:
	unsigned int id;

	shader(const char* vertex_path, const char* fragment_path) {
		// 1. 从文件路径得到顶点、片段着色器源码
		std::string vertex_code;
		std::string fragment_code;
		std::ifstream v_shader_file;
		std::ifstream f_shader_file;

		// 保证ifstream对象可以抛出异常
		v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			// 打开文件
			v_shader_file.open(vertex_path);
			f_shader_file.open(fragment_path);
			std::stringstream v_shader_stream, f_shader_stream;
			// 读取文件的缓冲内容到数据流中
			v_shader_stream << v_shader_file.rdbuf();
			f_shader_stream << f_shader_file.rdbuf();
			// 关闭文件处理器
			v_shader_file.close();
			f_shader_file.close();
			// 转换数据流到string
			vertex_code = v_shader_stream.str();
			fragment_code = f_shader_stream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		auto v_shader_code = vertex_code.c_str();
		auto f_shader_code = fragment_code.c_str();

		// 顶点着色器
		const auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &v_shader_code, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		// 片段着色器
		const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &f_shader_code, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		// 着色器程序
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		check_compile_errors(id, "PROGRAM");

		// 删除着色器，它们已经链接到程序中了
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	// 使用着色器
	void use() const {
		glUseProgram(id);
	}

	// 设置Uniform
	void set_bool(const std::string &name, const bool value) const {
		glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<int>(value));
	}

	void set_int(const std::string &name, const int value) const {
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void set_float(const std::string &name, const float value) const {
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void set_vec2(const std::string &name, const heracles::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void set_vec2(const std::string &name, const float x, const float y) const {
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}

	void set_mat22(const std::string &name, const heracles::mat22 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_TRUE, &mat[0][0]);
	}

private:
	// 检测着色器编译、链接是否出错
	static void check_compile_errors(const unsigned int shader, const std::string type) {
		int success;
		char info_log[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};

#endif