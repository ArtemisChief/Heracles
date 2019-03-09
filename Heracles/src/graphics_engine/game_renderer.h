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

		// 窗口
		static GLFWwindow* window_;

		// 着色器程序
		static shader* shader_program_;

		// 世界
		static world* the_world_;

		// 窗口大小初始设置
		static int win_width_;
		static int win_height_;

		// 视点向量与投影矩阵
		static float zoom_;
		static vec2 view_;
		static mat22 projection_;

	public:
		// 绘制刚体
		static void draw_body(polygon_body& body);

		// 绘制铰链
		static void draw_joint();

		// 标题栏显示dt
		static void update_title(const double dt);

		// 时钟同步
		static auto diff_time();

		// 绑定刚体对象的顶点，新创建的刚体只需要调用一次该函数
		static void bind_vertex_array(polygon_body& body);

		// 渲染
		static void display();

		// 摄像机移动
		static void move_camera(const vec2 translation);

		// 鼠标按键回调函数
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// 鼠标滚轮回调函数
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// 处理输入
		static void process_input(GLFWwindow *window);

		// 窗口大小调整时对视口（Viewport）进行调整的回调函数
		static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

		// 物理引擎部分
		static void heracles_run();

		// 初始化
		static int init();

		// 渲染主循环
		static void loop();
	};
}
