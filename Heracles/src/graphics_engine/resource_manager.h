#pragma once
#include <glad/glad.h>
#include <string>
#include <map>
#include "shader.h"
#include "texture.h"

namespace heracles {

	class resource_manager {

	public:

		static std::map<std::string, shader> shaders_;

		static std::map<std::string, texture> textures_;

		static shader load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const std::string name);

		static shader get_shader(const std::string name);

		static texture load_texture(const GLchar *file, const std::string name);

		static texture get_texture(const std::string name);

		static void clear();

	private:

		resource_manager() {}

		static shader load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file);

		static texture load_texture_from_file(const GLchar *file);
	};

}
