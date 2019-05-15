#pragma once
#include <GLFW/glfw3.h>

namespace heracles {

	class user_interface {

	public:
		// ��굱ǰ����
		static double mouse_x, mouse_y;

		// ��ǰѡ�������
		static int curr_type;

		// ��ǰָ�������
		static rigid_body::ptr curr_body;

		// ������������
		static rigid_body::ptr target_body;

		// �Ƿ�����
		static bool is_locked;

		// �Ƿ���ʾ��Ϣ
		static bool show_info;

		// ����GUI
		static void update_gui();

		// �������ص�����
		static void cursor_callback(GLFWwindow* window, double x, double y);

		// ��갴���ص�����
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// �����ֻص�����
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// ���̰����ص�����
		static void keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);

		// �����������
		static void process_input();

	};

}
