#pragma once
#include <glad/glad.h>

namespace heracles {

	class texture {
	public:

		GLuint id;

		GLuint width, height;

		texture();

		// 从图片生成纹理对象
		void generate(GLuint width, GLuint height, unsigned char* data);

		// 与当前激活的对象绑定纹理
		void bind() const;
	};

}