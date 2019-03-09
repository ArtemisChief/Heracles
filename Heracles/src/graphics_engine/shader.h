#pragma once

#include <glad/glad.h>
#include <string>

#include "../util/math.h"

namespace heracles {

	class shader {

	public:

		GLint id;

		shader() {}

		// 使用着色器
		shader& use();

		void compile(const GLchar *vertex_source, const GLchar *fragment_source);

		// 设置Uniform
		void set_int(const GLchar *name, const GLint value) const;
		void set_float(const GLchar *name, const GLfloat value) const;
		void set_vec2(const GLchar *name, const vec2& value) const;
		void set_mat22(const GLchar *name, const mat22& mat) const;

	private:
		// 检测着色器编译、链接是否出错
		static void check_compile_errors(const GLuint shader, const std::string type);

	};
}