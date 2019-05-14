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

		// ��ɫ������
		static shader shader_program_;

		// ���ڴ�С��ʼ����
		static GLint win_width_;
		static GLint win_height_;

		// ����
		static world* the_world_;

		// �ӵ�������ͶӰ����
		static GLfloat zoom_;
		static vec2 view_;
		static mat22 projection_;

		// ������Ⱦ��
		static text* text_;

	public:

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

		// ���ƽӴ���
		static void draw_contact(const arbiter::ptr& arbiter);

		// ��ʾUI
		static void show_ui();

		// ��������ʾdt
		static void update_title(const float dt);

		// ʱ��ͬ��
		static auto diff_time();

		// �󶨸������Ķ��㣬�´����ĸ���ֻ��Ҫ����һ�θú���
		static void bind_vertex_array(rigid_body::ptr body);

		// ��Ⱦ
		static void display();

		// �������沿��
		static void heracles_run();

		// ��ȡ���ָ�������
		static void get_body_info();

		// ������ƶ�
		static void move_camera(const vec2 translation);

		// ��갴���ص�����
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// �����ֻص�����
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// ���̰����ص�����
		static void keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);

		// ��������
		static void process_input();

		// ���ڴ�С����ʱ���ӿڣ�Viewport�����е����Ļص�����
		static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

		// ��ʼ��
		static int init();

		// ��Ⱦ��ѭ��
		static void loop();
	};
}
