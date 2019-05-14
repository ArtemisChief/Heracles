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

		// 窗口
		static GLFWwindow* window_;

		// 着色器程序
		static shader shader_program_;

		// 窗口大小初始设置
		static GLint win_width_;
		static GLint win_height_;

		// 世界
		static world* the_world_;

		// 视点向量与投影矩阵
		static GLfloat zoom_;
		static vec2 view_;
		static mat22 projection_;

		// 文字渲染器
		static text* text_;

	public:

		// 切换/设置着色器
		static void set_shader(const char* shader_name, const char* name, const vec2 v);
		static void set_shader(const char* shader_name, const char* name, const float x, const float y, const float z);
		static void set_shader(const char* shader_name, const char* name, const mat22 m);

		// 绘制文字
		static void draw_text(const bool is_screen, const std::string text, 
							  const GLfloat xpos, const GLfloat ypos, const GLfloat scale, 
							  const GLfloat r, const GLfloat g, const GLfloat b);

		// 绘制刚体
		static void draw_body(rigid_body& body);

		// 绘制接触点
		static void draw_contact(const arbiter::ptr& arbiter);

		// 显示UI
		static void show_ui();

		// 标题栏显示dt
		static void update_title(const float dt);

		// 时钟同步
		static auto diff_time();

		// 绑定刚体对象的顶点，新创建的刚体只需要调用一次该函数
		static void bind_vertex_array(rigid_body::ptr body);

		// 渲染
		static void display();

		// 物理引擎部分
		static void heracles_run();

		// 获取鼠标指向的物体
		static void get_body_info();

		// 摄像机移动
		static void move_camera(const vec2 translation);

		// 鼠标按键回调函数
		static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods);

		// 鼠标滚轮回调函数
		static void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);

		// 键盘按键回调函数
		static void keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);

		// 处理输入
		static void process_input();

		// 窗口大小调整时对视口（Viewport）进行调整的回调函数
		static void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

		// 初始化
		static int init();

		// 渲染主循环
		static void loop();
	};
}
