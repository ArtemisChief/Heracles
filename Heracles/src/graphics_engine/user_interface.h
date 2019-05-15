#pragma once
#include <GLFW/glfw3.h>

namespace heracles {

	class user_interface {

	public:
		// 鼠标当前坐标
		static double mouse_x, mouse_y;

		// 当前选择的类型
		static int curr_type;

		// 当前指向的物体
		static rigid_body::ptr curr_body;

		// 需锁定的物体
		static rigid_body::ptr target_body;

		// 是否锁定
		static bool is_locked;

		// 是否显示信息
		static bool show_info;

		// 更新GUI
		static void update_gui();

		// 鼠标坐标回调函数
		static void cursor_callback(GLFWwindow* window, double x, double y);

		// 鼠标按键回调函数
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// 鼠标滚轮回调函数
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// 键盘按键回调函数
		static void keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);

		// 处理高速输入
		static void process_input();

	};

}
