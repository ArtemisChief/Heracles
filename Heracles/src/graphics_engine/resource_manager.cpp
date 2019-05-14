#include <iostream>
#include <sstream>
#include <fstream>

#include "resource_manager.h"

namespace heracles {

	std::map<std::string, shader> resource_manager::shaders_;

	shader resource_manager::load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const std::string name) {
		shaders_[name] = load_shader_from_file(v_shader_file, f_shader_file);
		return shaders_[name];
	}

	shader resource_manager::get_shader(const std::string name) {
		return shaders_[name];
	}

	void resource_manager::clear() {
		for (const auto iter : shaders_)
			glDeleteProgram(iter.second.id);
	}

	shader resource_manager::load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file) {
		// 1. 从文件路径得到顶点、片段着色器源码
		std::string vertex_code;
		std::string fragment_code;
		try {
			// 打开文件
			std::ifstream vertex_shader_file(v_shader_file);
			std::ifstream fragment_shader_file(f_shader_file);
			std::stringstream v_shader_stream, f_shader_stream;
			// 读取文件的缓冲内容到数据流中
			v_shader_stream << vertex_shader_file.rdbuf();
			f_shader_stream << fragment_shader_file.rdbuf();
			// 关闭文件处理器
			vertex_shader_file.close();
			fragment_shader_file.close();
			// 转换数据流到string
			vertex_code = v_shader_stream.str();
			fragment_code = f_shader_stream.str();
		}
		catch (std::exception e) {
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const auto v_shader_code = vertex_code.c_str();
		const auto f_shader_code = fragment_code.c_str();
		// 2. 从源码中编译着色器程序
		shader shader;
		shader.compile(v_shader_code, f_shader_code);
		return shader;
	}
}