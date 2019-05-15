#pragma once
#include <glad/glad.h>
#include <string>
#include <map>
#include "shader.h"

namespace heracles {

	class resource_manager {

	public:

		static shader load_shader(const GLchar *v_shader_file, const GLchar *f_shader_file, const std::string name);

		static shader get_shader(const std::string name);

		static void clear();

	private:

		static std::map<std::string, shader> shaders_;

		static shader load_shader_from_file(const GLchar *v_shader_file, const GLchar *f_shader_file);
	};

}
