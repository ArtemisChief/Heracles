#include <iostream>

#include "texture.h"

namespace heracles {

	texture::texture() : width(0), height(0) {
		glGenTextures(1, &this->id);
	}

	void texture::generate(const GLuint width, const GLuint height, unsigned char* data) {
		this->width = width;
		this->height = height;

		// �󶨲���
		glBindTexture(GL_TEXTURE_2D, this->id);

		// ���������Ʒ�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// ����������˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// ���
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void texture::bind() const {
		glBindTexture(GL_TEXTURE_2D, this->id);
	}

}