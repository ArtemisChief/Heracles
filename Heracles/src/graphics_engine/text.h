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
		// �洢������ɵ�����
		std::map<GLchar, character> characters;

		// ������ɫ��
		shader text_shader;

		// ���캯��
		text(const GLuint width, const GLuint height);

		// ��ȡttf�����ļ�������
		void load(std::string font, const GLuint font_size);

		// ��Ⱦ����
		void render_text(std::string text, GLfloat x, const GLfloat y, const GLfloat scale, 
						 const GLfloat r, const GLfloat g, const GLfloat b);

	private:
		GLuint vao_, vbo_;
	};

}