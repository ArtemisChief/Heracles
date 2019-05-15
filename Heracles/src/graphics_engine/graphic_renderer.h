#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "shader.h"
#include "../physics_engine/body.h"
#include "../physics_engine/world.h"
#include "text.h"


namespace heracles {

	class graphic_renderer {

		// ����
		static GLFWwindow* window_;

		// ��Ķ����������
		static unsigned int point_vao_;

		// ��ɫ������
		static shader shader_program_;

		// ���ڴ�С��ʼ����
		static GLint win_width_;
		static GLint win_height_;

		// �ӵ�������ͶӰ����
		static GLfloat zoom_;
		static vec2 view_;
		static mat22 projection_;

		// ������Ⱦ��
		static text* text_;

	public:

		friend class user_interface;

		// �л�/������ɫ��
		static void set_shader(const char* shader_name, const char* name, const vec2 v);
		static void set_shader(const char* shader_name, const char* name, const float x, const float y, const float z);
		static void set_shader(const char* shader_name, const char* name, const mat22 m);

		// ��������
		static void draw_text(const bool is_screen, const std::string text, 
							  const GLfloat xpos, const GLfloat ypos, const GLfloat scale, 
							  const GLfloat r, const GLfloat g, const GLfloat b);

		// ���Ƹ���
		static void draw_body(rigid_body& body);

		// ���ƽ���
		static void draw_joint(revolute_joint& joint);

		// ���ƽӴ���
		static void draw_contact(const arbiter::ptr& arbiter);

		// ��������ʾdt
		static void update_title(const float dt);

		// ʱ��ͬ��
		static auto diff_time();

		// �󶨸������Ķ��㣬�´����ĸ���ֻ��Ҫ����һ�θú���
		static void bind_vertex_array(rigid_body::ptr body);

		// �󶨽�������Ķ��㣬�´����Ľ���ֻ��Ҫ����һ�θú���
		static void bind_vertex_array(revolute_joint::ptr joint);

		// ��Ⱦ
		static void display();

		// ���������߳�
		static void heracles_run();

		// ������ƶ�
		static void move_camera(const vec2 translation);

		// ���ڴ�С����ʱ���ӿڣ�Viewport�����е����Ļص�����
		static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

		// ��ʼ��
		static int init();

		// ��Ⱦ��ѭ��
		static void loop();
	};
}
