#pragma once
#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "shader.h"
#include "../util/stb_image.h"
#include "../physics_engine/body.h"
#include "../physics_engine/world.h"


namespace heracles {
	class game_renderer {

		// ����
		static GLFWwindow* window_;

		// ��ɫ������
		static shader* shader_program_;

		// ����
		static world* the_world_;

		// ���ڴ�С��ʼ����
		static int win_width_;
		static int win_height_;

		// �ӵ�������ͶӰ����
		static float zoom_;
		static vec2 view_;
		static mat22 projection_;

	public:
		// ���Ƹ���
		static void draw_body(polygon_body& body);

		// ���ƽ���
		static void draw_joint();

		// ��������ʾdt
		static void update_title(const double dt);

		// ʱ��ͬ��
		static auto diff_time();

		// �󶨸������Ķ��㣬�´����ĸ���ֻ��Ҫ����һ�θú���
		static void bind_vertex_array(polygon_body& body);

		// ��Ⱦ
		static void display();

		// ������ƶ�
		static void move_camera(const vec2 translation);

		// ��갴���ص�����
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// �����ֻص�����
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// ��������
		static void process_input(GLFWwindow *window);

		// ���ڴ�С����ʱ���ӿڣ�Viewport�����е����Ļص�����
		static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

		// �������沿��
		static void heracles_run();

		// ��ʼ��
		static int init();

		// ��Ⱦ��ѭ��
		static void loop();
	};
}
