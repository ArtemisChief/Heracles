#include <iostream>
#include <sstream>

#include "graphic_renderer.h"
#include "resource_manager.h"
#include <cstdarg>
#include <atomic>
#include <random>
#include "user_interface.h"

namespace heracles {

	unsigned int graphic_renderer::point_vao_;

	// 窗口
	GLFWwindow* graphic_renderer::window_ = nullptr;

	// 着色器程序
	shader graphic_renderer::shader_program_;

	// 窗口大小初始设置
	GLint graphic_renderer::win_width_ = 1600;
	GLint graphic_renderer::win_height_ = 900;

	// 视点向量与投影矩阵
	GLfloat graphic_renderer::zoom_ = 0.0f;
	vec2 graphic_renderer::view_(0.0f, 0.0f);
	mat22 graphic_renderer::projection_(400.0f / win_width_, 0, 0, 400.0f / win_height_);

	text* graphic_renderer::text_ = nullptr;

	// 切换/设置着色器
	void graphic_renderer::set_shader(const char* shader_name, const char* name, const vec2 v) {
		const auto temp = resource_manager::get_shader(shader_name);

		if (shader_program_.id != temp.id) {
			shader_program_ = temp;
			shader_program_.use();
		}

		shader_program_.set_vec2(name, v);
	}

	void graphic_renderer::set_shader(const char* shader_name, const char* name, const float x, const float y, const float z) {
		const auto temp = resource_manager::get_shader(shader_name);

		if (shader_program_.id != temp.id) {
			shader_program_ = temp;
			shader_program_.use();
		}

		shader_program_.set_vec3(name, x, y, z);
	}

	void graphic_renderer::set_shader(const char* shader_name, const char* name, const mat22 m) {
		const auto temp = resource_manager::get_shader(shader_name);

		if (shader_program_.id != temp.id) {
			shader_program_ = temp;
			shader_program_.use();
		}

		shader_program_.set_mat22(name, m);
	}


	// 绘制文字
	void graphic_renderer::draw_text(const bool is_screen, const std::string text, 
									 const GLfloat xpos, const GLfloat ypos, GLfloat const scale, 
									 const GLfloat r, const GLfloat g, const GLfloat b) {
		if (is_screen) {
			set_shader("text", "view", vec2(0.0f, 0.0f));
			set_shader("text", "projection", mat22(2.0f / win_width_, 0.0f, 0.0f, 2.0f / win_height_));
			text_->render_text(text, xpos, ypos, scale, r, g, b);
		}
		else {
			set_shader("text", "view", view_);
			set_shader("text", "projection", projection_);
			text_->render_text(text, xpos, ypos, scale / 1000.0f, r, g, b);
		}
	}

	// 绘制刚体
	void graphic_renderer::draw_body(rigid_body& body) {
		set_shader("graphic", "translation", body.get_world_position());
		set_shader("graphic", "rotation", body.get_rotation());
		set_shader("graphic", "anchor", body.get_centroid());

		glBindVertexArray(*body.get_id());

		set_shader("graphic", "color", 0.41f, 0.41f, 0.41f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, body.count());

		glLineWidth(2.0);
		set_shader("graphic", "color", 0.9f, 0.9f, 0.9f);
		glDrawArrays(GL_LINE_LOOP, 0, body.count());

		if (user_interface::show_info && body.get_id() == user_interface::target_body->get_id()) {
			glLineWidth(3.0);
			if (user_interface::is_locked)
				set_shader("graphic", "color", 0.75f, 0.40f, 0.2f);
			else
				set_shader("graphic", "color", 0.0f, 0.47f, 0.84f);
			glDrawArrays(GL_LINE_LOOP, 0, body.count());
		}
	}

	void graphic_renderer::draw_joint(revolute_joint& joint)
	{
		if (joint.get_a()->get_mass() != inf) {
			glBindVertexArray(*joint.get_id_a());
			set_shader("graphic", "translation", joint.get_a()->get_world_position());
			set_shader("graphic", "rotation", joint.get_a()->get_rotation());
			set_shader("graphic", "anchor", vec2(0, 0));
			set_shader("graphic", "color", 0.35f, 0.49f, 0.56f);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}

		if (joint.get_b()->get_mass() != inf) {
			glBindVertexArray(*joint.get_id_b());
			set_shader("graphic", "translation", joint.get_b()->get_world_position());
			set_shader("graphic", "rotation", joint.get_b()->get_rotation());
			set_shader("graphic", "anchor", vec2(0, 0));
			set_shader("graphic", "color", 0.35f, 0.49f, 0.56f);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}
	}


	// 绘制接触点
	void graphic_renderer::draw_contact(const arbiter::ptr& arbiter) {
		glBindVertexArray(point_vao_);

		auto &contacts = arbiter->get_contacts();
		for(auto contact:contacts)
		{
			set_shader("graphic", "translation", contact.position);
			set_shader("graphic", "rotation", mat22::i);
			set_shader("graphic", "anchor", vec2(0,0));
			set_shader("graphic", "color", 1.0f, 0.2f, 0.2f);
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}

	// 标题栏显示dt
	void graphic_renderer::update_title(const float dt) {
		std::stringstream ss;
		ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
		glfwSetWindowTitle(window_, ss.str().c_str());
	}

	// 时钟同步
	auto graphic_renderer::diff_time() {
		using namespace std::chrono;
		using seconds = duration<double>;
		static auto last_clock = high_resolution_clock::now();		// 每次调用high_resolution_clock::now()
		const auto now = high_resolution_clock::now();				// 后一次一定比前一次大
		const auto dt = duration_cast<seconds>(now - last_clock);	// 保证tick经过一个稳定的时间间隔
		update_title(dt.count());									// 显示dt
		last_clock = now;
		return dt;
	}

	// 绑定刚体顶点
	void graphic_renderer::bind_vertex_array(rigid_body::ptr body) {
		auto vertices = body->get_vertices();
		const auto size = sizeof(float) * vertices.size() * 2;

		// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
		auto& vao = *body->get_id();
		unsigned int vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		// 处理顶点
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size * 2, &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
	}

	// 绑定铰链
	void graphic_renderer::bind_vertex_array(revolute_joint::ptr joint) {
		const auto centroid_a = joint->get_a()->get_centroid();
		const auto anchor_a = joint->get_local_anchor_a();

		std::vector<vec2> vertices1 = { centroid_a, anchor_a };
		const auto size = sizeof(float) * vertices1.size() * 2;

		// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
		auto& vao1 = *joint->get_id_a();
		unsigned int vbo1;
		glGenVertexArrays(1, &vao1);
		glGenBuffers(1, &vbo1);

		// 处理顶点
		glBindVertexArray(vao1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		glBufferData(GL_ARRAY_BUFFER, size * 2, &vertices1[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		const auto centroid_b = joint->get_b()->get_centroid();
		const auto anchor_b = joint->get_local_anchor_b();

		std::vector<vec2> vertices2 = { centroid_b, anchor_b };

		// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
		auto& vao2 = *joint->get_id_b();
		unsigned int vbo2;
		glGenVertexArrays(1, &vao2);
		glGenBuffers(1, &vbo2);

		// 处理顶点
		glBindVertexArray(vao2);

		glBindBuffer(GL_ARRAY_BUFFER, vbo2);
		glBufferData(GL_ARRAY_BUFFER, size * 2, &vertices2[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
	}

	// 渲染 - 所有 OpenGL 绘制放在这里进行
	void graphic_renderer::display() {
		glClearColor(0.117f, 0.117f, 0.117f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		world::lock();
		for (auto &body : world::bodies) {
			draw_body(*std::dynamic_pointer_cast<rigid_body>(body));
		}

		for (auto &joint : world::joints) {
			draw_joint(*std::dynamic_pointer_cast<revolute_joint>(joint));
		}

		if (user_interface::show_info) {
			for (auto &arbiter : world::arbiters) {
				draw_contact(arbiter.second);
			}
		}
		world::unlock();

		user_interface::update_gui();

		// glfw双缓冲+处理事件
		glfwSwapBuffers(window_);
	}

	// 物理引擎运行线程
	static std::atomic<bool> should_stop{ false };
	void graphic_renderer::heracles_run() {
		using namespace std::chrono_literals;
		while (!should_stop) {
			std::this_thread::sleep_for(10ms);
			auto dt = diff_time().count();

			if (!world::is_paused) {
				// 世界运行一个步长（Step）的运算
				world::lock();
				world::step(std::min(dt, 0.01));
				world::unlock();
			}
		}
	}

	// 摄像机移动
	void graphic_renderer::move_camera(const vec2 translation) {
		view_ += translation;
		set_shader("graphic", "view", view_);
	}

	// 窗口大小调整时对视口（Viewport）进行调整的回调函数
	void graphic_renderer::framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
		glViewport(0, 0, width, height);
		win_width_ = width;
		win_height_ = height;
		projection_[0].x = 400.0f / win_width_ + zoom_ * 10.0f / win_width_;
		projection_[1].y = 400.0f / win_height_ + zoom_ * 10.0f / win_height_;
		set_shader("graphic", "projection", projection_);
	}

	// 初始化
	int graphic_renderer::init() {
		// glfw初始化
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// 多重采样抗锯齿倍数
		glfwWindowHint(GLFW_SAMPLES, 4);
		
		// glfw窗口创建
		window_ = glfwCreateWindow(win_width_, win_height_, "Heracles", nullptr, nullptr);
		if (window_ == nullptr) {
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window_);
		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

		// 设置回调函数
		glfwSetMouseButtonCallback(window_, user_interface::mouse_callback);
		glfwSetScrollCallback(window_, user_interface::scroll_callback);
		glfwSetKeyCallback(window_, user_interface::keyboard_callback);
		glfwSetCursorPosCallback(window_, user_interface::cursor_callback);

		// glad加载OpenGL函数指针
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			return -2;
		}

		// 开启多重采样抗锯齿
		glEnable(GL_MULTISAMPLE);

		// 开启混合
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 加载字体
		text_ = new text(win_width_, win_height_);
		text_->load("C:/Windows/Fonts/consola.ttf", 48);

		// 构造并使用点着色器和片段着色器
		resource_manager::load_shader("src/graphics_engine/shader/graphic.v", 
									  "src/graphics_engine/shader/graphic.f",
									  "graphic");
		
		set_shader("graphic", "view", view_);
		set_shader("graphic", "projection", projection_);

		// 线框模式
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// 设置点的顶点数组
		auto p = vec2(0, 0);
		unsigned int vbo;
		glGenVertexArrays(1, &point_vao_);
		glGenBuffers(1, &vbo);
		glBindVertexArray(point_vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof p, &p, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		// 设置点大小
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);

		// 设置世界参数
		world::g = vec2(0.0f, -9.8f);
		arbiter::k_bias_factor = 0.2f;

		return 0;
	}

	// 渲染主循环
	void graphic_renderer::loop() {
		std::thread heracles_thread(heracles_run);

		while (!glfwWindowShouldClose(window_)) {
			user_interface::process_input();		// 处理输入
			display();								// 显示图像
			glfwPollEvents();						// 处理事件
		}

		glfwTerminate();
		should_stop = true;
		heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	}
}
