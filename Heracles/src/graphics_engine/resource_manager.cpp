#include <iostream>
#include <sstream>
#include <fstream>

#include "resource_manager.h"
#include "../util/stb_image.h"

namespace heracles {

	std::map<std::string, texture> resource_manager::textures;
	std::map<std::string, shader> resource_manager::shaders;

	shader resource_manager::load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const std::string name) {
		shaders[name] = load_shader_from_file(v_shader_file, f_shader_file);
		return shaders[name];
	}

	shader resource_manager::get_shader(const std::string name) {
		return shaders[name];
	}

	texture resource_manager::load_texture(const GLchar *file, const std::string name) {
		textures[name] = load_texture_from_file(file);
		return textures[name];
	}

	texture resource_manager::get_texture(const std::string name) {
		return textures[name];
	}

	void resource_manager::clear() {
		for (const auto iter : shaders)
			glDeleteProgram(iter.second.id);

		for (auto iter : textures)
			glDeleteTextures(1, &iter.second.id);
	}

	shader resource_manager::load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file) {
		// 1. 从文件路径得到顶点、片段着色器源码
		std::string vertex_code;
		std::string fragment_code;
		try {
			// 打开文件
			std::ifstream vertex_shader_file(v_shader_file);
			std::ifstream fragment_shader_file(f_shader_file);
			std::stringstream vShaderStream, fShaderStream;
			// 读取文件的缓冲内容到数据流中
			vShaderStream << vertex_shader_file.rdbuf();
			fShaderStream << fragment_shader_file.rdbuf();
			// 关闭文件处理器
			vertex_shader_file.close();
			fragment_shader_file.close();
			// 转换数据流到string
			vertex_code = vShaderStream.str();
			fragment_code = fShaderStream.str();
		}
		catch (std::exception e) {
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const GLchar *vShaderCode = vertex_code.c_str();
		const GLchar *fShaderCode = fragment_code.c_str();
		// 2. 从源码中编译着色器程序
		shader shader;
		shader.compile(vShaderCode, fShaderCode);
		return shader;
	}

	texture resource_manager::load_texture_from_file(const GLchar *file) {
		// 创建纹理对象
		texture texture;

		// 读取图片时上下翻转
		stbi_set_flip_vertically_on_load(true);

		int width, height, nr_channels;

		const auto data = stbi_load(file, &width, &height, &nr_channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}

		// 生成纹理
		texture.generate(width, height, data);

		// 释放图片数据
		stbi_image_free(data);

		return texture;
	}
}