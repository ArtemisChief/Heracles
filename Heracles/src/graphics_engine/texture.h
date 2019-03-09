#pragma once
#include <glad/glad.h>

namespace heracles {

	class texture {
	public:

		GLuint id;

		GLuint width, height;

		texture();

		// ��ͼƬ�����������
		void generate(GLuint width, GLuint height, unsigned char* data);

		// �뵱ǰ����Ķ��������
		void bind() const;
	};

}