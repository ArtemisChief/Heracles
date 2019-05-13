#pragma once
#include <glad/glad.h>
#include <map>
#include "shader.h"

namespace heracles {

	struct character {
		GLuint texture_id;
		GLuint size_x;
		GLuint size_y;
		GLint bearing_x;
		GLint bearing_y;
		GLuint advance;
	};

	class text {
	public:
		// 存储编译完成的字体
		std::map<GLchar, character> characters;

		// 字体着色器
		shader text_shader;

		// 构造函数
		text(const GLuint width, const GLuint height);

		// 读取ttf字体文件并编译
		void load(std::string font, const GLuint font_size);

		// 渲染文字
		void render_text(std::string text, GLfloat x, const GLfloat y, const GLfloat scale, 
						 const GLfloat r, const GLfloat g, const GLfloat b);

	private:
		GLuint vao_, vbo_;
	};

}