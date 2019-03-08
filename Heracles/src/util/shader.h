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
		// 1. ���ļ�·���õ����㡢Ƭ����ɫ��Դ��
		std::string vertex_code;
		std::string fragment_code;
		std::ifstream v_shader_file;
		std::ifstream f_shader_file;

		// ��֤ifstream��������׳��쳣
		v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			// ���ļ�
			v_shader_file.open(vertex_path);
			f_shader_file.open(fragment_path);
			std::stringstream v_shader_stream, f_shader_stream;
			// ��ȡ�ļ��Ļ������ݵ���������
			v_shader_stream << v_shader_file.rdbuf();
			f_shader_stream << f_shader_file.rdbuf();
			// �ر��ļ�������
			v_shader_file.close();
			f_shader_file.close();
			// ת����������string
			vertex_code = v_shader_stream.str();
			fragment_code = f_shader_stream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		auto v_shader_code = vertex_code.c_str();
		auto f_shader_code = fragment_code.c_str();

		// ������ɫ��
		const auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &v_shader_code, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		// Ƭ����ɫ��
		const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &f_shader_code, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		// ��ɫ������
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		check_compile_errors(id, "PROGRAM");

		// ɾ����ɫ���������Ѿ����ӵ���������
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	// ʹ����ɫ��
	void use() const {
		glUseProgram(id);
	}

	// ����Uniform
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
	// �����ɫ�����롢�����Ƿ����
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