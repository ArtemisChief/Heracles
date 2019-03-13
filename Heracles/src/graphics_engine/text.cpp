#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

#include "text.h"
#include "resource_manager.h"

namespace heracles {

	text::text(const GLuint width, const GLuint height) {
		// 读取并配置着色器
		this->text_shader = resource_manager::load_shader("src/graphics_engine/shader/text.v", "src/graphics_engine/shader/text.f", "text");
		this->text_shader.use().set_vec2("view", vec2(0.0f, 0.0f));
		this->text_shader.set_mat22("projection", mat22(2.0f / width, 0.0f, 0.0f, 2.0f / height));
		this->text_shader.set_vec3("textColor", 1.0f, 1.0f, 1.0f);
		this->text_shader.set_int("text", 0);

		// 处理VAO与VBO
		glGenVertexArrays(1, &this->vao_);
		glGenBuffers(1, &this->vbo_);

		glBindVertexArray(this->vao_);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}

	void text::load(std::string font, const GLuint font_size) {
		// 清理之前编译的字体
		this->characters.clear();

		// 初始化 Freetype 库
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		// 加载字体为一个面
		FT_Face face;
		if (FT_New_Face(ft, font.c_str(), 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

		// 设置字体大小
		FT_Set_Pixel_Sizes(face, 0, font_size);

		// 将纹理解压对齐参数设为1
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		GLuint texture;

		// 取 ASCII 32~127位进行编译
		for (GLubyte c = 32; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			// 生成纹理
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			// 设置纹理
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// 存储字符以供调用
			character ch = {
				texture,
				face->glyph->bitmap.width, 
				face->glyph->bitmap.rows,
				face->glyph->bitmap_left, 
				face->glyph->bitmap_top,
				face->glyph->advance.x
			};
			characters.insert(std::pair<GLchar, character>(c, ch));
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// 释放 Freetpye 资源
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void text::render_text(std::string text, GLfloat x, const GLfloat y, const GLfloat scale, 
						   const GLfloat r, const GLfloat g, const GLfloat b) {

		this->text_shader.use().set_vec3("textColor", r, g, b);
		//glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(this->vao_);

		for (std::string::const_iterator c = text.begin(); c != text.end(); ++c) {
			const auto ch = characters[*c];

			const auto xpos = x + ch.bearing_x * scale;
			const auto ypos = y - (ch.size_y - ch.bearing_y) * scale;

			const auto w = ch.size_x * scale;
			const auto h = ch.size_y * scale;

			// 对每个字符更新VBO
			GLfloat vertices[16] = {
				 xpos,     ypos,       0.0, 1.0 ,
				 xpos,     ypos + h,   0.0, 0.0 ,
				 xpos + w, ypos,       1.0, 1.0 ,
				 xpos + w, ypos + h,   1.0, 0.0
			};

			glBindTexture(GL_TEXTURE_2D, ch.texture_id);

			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 16, vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			x += (ch.advance >> 6) * scale;

		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}