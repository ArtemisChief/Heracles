#pragma once

#include <glad/glad.h>
#include <string>

#include "../util/math.h"

namespace heracles {

	class shader {

	public:

		GLint id;

		shader() {}

		// ʹ����ɫ��
		shader& use();

		void compile(const GLchar *vertex_source, const GLchar *fragment_source);

		// ����Uniform
		void set_int(const GLchar *name, const GLint value) const;
		void set_float(const GLchar *name, const GLfloat value) const;
		void set_vec2(const GLchar *name, const vec2& value) const;
		void set_mat22(const GLchar *name, const mat22& mat) const;

	private:
		// �����ɫ�����롢�����Ƿ����
		static void check_compile_errors(const GLuint shader, const std::string type);

	};
}