#include <iostream>
#include <sstream>

#include "graphic_renderer.h"
#include "resource_manager.h"

namespace heracles {

	// ����
	GLFWwindow* graphic_renderer::window_ = nullptr;

	// ��ɫ������
	shader graphic_renderer::shader_program_;

	// ����
	world* graphic_renderer::the_world_ = nullptr;

	// ���ڴ�С��ʼ����
	GLint graphic_renderer::win_width_ = 1600;
	GLint graphic_renderer::win_height_ = 900;

	// �ӵ�������ͶӰ����
	GLfloat graphic_renderer::zoom_ = 0.0f;
	vec2 graphic_renderer::view_(0.0f, 0.0f);
	mat22 graphic_renderer::projection_(1000.0f / win_width_, 0, 0, 1000.0f / win_height_);

	text* graphic_renderer::text_ = nullptr;

	// �л�/������ɫ��
	void graphic_renderer::set_shader(const char* s, const char* v_name, const vec2 v, const char* m_name, const mat22 m)
	{
		const auto temp = resource_manager::get_shader(s);

		if (shader_program_.id != temp.id) {
			shader_program_ = temp;
			shader_program_.use();
		}

		shader_program_.use().set_vec2(v_name, v);
		shader_program_.set_mat22(m_name, m);
	}


	// ��������
	void graphic_renderer::draw_text(const bool is_screen, const std::string text, 
									 const GLfloat xpos, const GLfloat ypos, GLfloat const scale, 
									 const GLfloat r, const GLfloat g, const GLfloat b) {
		if (is_screen) {
			set_shader("text", "view", vec2(0.0f, 0.0f), "projection", mat22(2.0f / win_width_, 0.0f, 0.0f, 2.0f / win_height_));
			text_->render_text(text, xpos, ypos, scale, r, g, b);
		}
		else {
			set_shader("text", "view", view_, "projection", projection_);
			text_->render_text(text, xpos, ypos, scale / 1000.0f, r, g, b);
		}
	}

	// ���Ƹ���
	void graphic_renderer::draw_body(polygon_body& body) {
		set_shader("graphic", "translation", body.get_world_position(), "rotation", body.get_rotation());

		//resource_manager::get_texture("test1").bind();
		glBindVertexArray(*body.get_id());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}

	// ���ƽ���
	void graphic_renderer::draw_joint() {

	}

	// ��������ʾdt
	void graphic_renderer::update_title(const double dt) {
		std::stringstream ss;
		ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
		glfwSetWindowTitle(window_, ss.str().c_str());
	}

	// ʱ��ͬ��
	auto graphic_renderer::diff_time() {
		using namespace std::chrono;
		using seconds = duration<double>;
		static auto last_clock = high_resolution_clock::now();		// ÿ�ε���high_resolution_clock::now()
		const auto now = high_resolution_clock::now();				// ��һ��һ����ǰһ�δ�
		const auto dt = duration_cast<seconds>(now - last_clock);	// ��֤tick����һ���ȶ���ʱ����
		update_title(dt.count());									// ��ʾdt
		last_clock = now;
		return dt;
	}

	// �󶨸������Ķ��㣬�´����ĸ���ֻ��Ҫ����һ�θú���
	void graphic_renderer::bind_vertex_array(polygon_body& body) {
		auto vertices = body.get_vertices();

		float tex_coord[] = {
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};

		// ���ö������飬���ö����������VAO���붥�㻺�����VBO��
		const auto vao = body.get_id();
		unsigned int vbo;
		glGenVertexArrays(1, vao);
		glGenBuffers(1, &vbo);

		// ������
		glBindVertexArray(*vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, nullptr, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(8 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 8, &vertices[0]);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 8, sizeof(float) * 8, tex_coord);

		// ��������
		//resource_manager::load_texture("src/resources/container.jpg", "test1");
	
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}

	// ��Ⱦ - ���� OpenGL ���Ʒ����������
	void graphic_renderer::display() {
		glClear(GL_COLOR_BUFFER_BIT);
		
		for (auto &body : the_world_->get_bodies()) {
			draw_body(*std::dynamic_pointer_cast<polygon_body>(body).get());

			draw_text(false, std::string("   World Pos:").append
					(std::to_string(body->get_world_position().x).append
					(std::string(", ").append
					(std::to_string(body->get_world_position().y)).append
					(std::string("  Velocity: ").append(std::to_string(body->get_velocity().magnitude()))))),
					  body->get_world_position().x, body->get_world_position().y, 1.0f,
					  1.0f, 1.0f, 1.0f);
		}

		draw_text(true, "Heracles", -780.0f, 400.0f, 1.0f, 1.0f, 1.0f, 1.0f);

		// glfw˫����+�����¼�
		glfwSwapBuffers(window_);
	}

	// �����������в���
	static std::atomic<bool> is_paused{ false };
	static std::atomic<bool> should_stop{ false };
	void graphic_renderer::heracles_run() {
		using namespace std::chrono_literals;
		while (!should_stop) {
			std::this_thread::sleep_for(10ms);
			auto dt = diff_time().count();

			if (!is_paused) {
				// ��������һ��������Step��������
				the_world_->step(dt);
			}
		}
	}

	// ������ƶ�
	void graphic_renderer::move_camera(const vec2 translation) {
		view_ += translation;
		resource_manager::get_shader("graphic").use().set_vec2("view", view_);
	}

	// ��갴���ص�����
	void graphic_renderer::mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {
		if (action == GLFW_PRESS) switch (button) {
			//���������ø���
		case GLFW_MOUSE_BUTTON_LEFT: {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			const auto half_width = win_width_ / 2;
			const auto half_height = win_height_ / 2;
			vec2 pos((x - half_width) / half_width, (-y + half_height) / half_height);
			pos = projection_.inv() * pos + view_;

			// ���紴�����
			auto body = the_world_->create_box(1, 0.1, 0.1, pos);
			the_world_->add(body);

			//�󶨸���Ķ�������
			bind_vertex_array(*std::dynamic_pointer_cast<polygon_body>(body).get());

			std::cout << "Create Box (" << body->get_id() << ") : [" << pos.x << " " << pos.y << "]" << std::endl;
		}

		// ����Ҽ���ĳ������ʩ����
		case GLFW_MOUSE_BUTTON_RIGHT: {
			break;
		}
		default: ;
		}
	}

	// �����ֻص�����
	void graphic_renderer::scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
		if (zoom_ >= -20.0f && zoom_ <= 25.0f) {
			zoom_ += yoffset;
			projection_[0].x = 1000.0f / win_width_ + zoom_ * 40.0f / win_width_;
			projection_[1].y = 1000.0f / win_height_ + zoom_ * 40.0f / win_height_;
			resource_manager::get_shader("graphic").use().set_mat22("projection", projection_);
		}
		if (zoom_ <= -20.0f)
			zoom_ = -20.0f;
		if (zoom_ >= 25.0f)
			zoom_ = 25.0f;
	}

	// ���̰����ص�����
	void graphic_renderer::keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			is_paused = !is_paused;
	}

	// ��������
	void graphic_renderer::process_input() {
		if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window_, true);

		auto camera_speed = 0.02f;
		if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera_speed *= 8;
		if (glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera_speed /= 4;
		if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
			move_camera(vec2(0, 1 * camera_speed));
		if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
			move_camera(vec2(-1 * camera_speed, 0));
		if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
			move_camera(vec2(0, -1 * camera_speed));
		if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
			move_camera(vec2(1 * camera_speed, 0));
	}

	// ���ڴ�С����ʱ���ӿڣ�Viewport�����е����Ļص�����
	void graphic_renderer::framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
		glViewport(0, 0, width, height);
		win_width_ = width;
		win_height_ = height;
		projection_[0].x = 1000.0f / win_width_ + zoom_ * 40.0f / win_width_;
		projection_[1].y = 1000.0f / win_height_ + zoom_ * 40.0f / win_height_;
		resource_manager::get_shader("graphic").use().set_mat22("projection", projection_);
	}



	// ��ʼ��
	int graphic_renderer::init() {
		// glfw��ʼ��
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// glfw���ڴ���
		window_ = glfwCreateWindow(win_width_, win_height_, "Heracles", nullptr, nullptr);
		if (window_ == nullptr) {
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window_);
		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

		// ���ûص�����
		glfwSetMouseButtonCallback(window_, mouse_callback);
		glfwSetScrollCallback(window_, scroll_callback);
		glfwSetKeyCallback(window_, keyboard_callback);

		// glad����OpenGL����ָ��
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			return -2;
		}

		// �������
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// ��������
		text_ = new text(win_width_, win_height_);
		text_->load("src/resources/consola.ttf", 48);

		// ���첢ʹ�õ���ɫ����Ƭ����ɫ��
		resource_manager::load_shader("src/graphics_engine/shader/graphic.v", "src/graphics_engine/shader/graphic.f", "graphic");
		resource_manager::get_shader("graphic").use().set_vec2("view", view_);
		resource_manager::get_shader("graphic").set_mat22("projection", projection_);

		// �߿�ģʽ
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		the_world_ = new world({ 0.0f, -9.8f });

		return 0;
	}

	// ��Ⱦ��ѭ��
	void graphic_renderer::loop() {
		std::thread heracles_thread(heracles_run);

		while (!glfwWindowShouldClose(window_)) {
			process_input();		// ��������
			display();				// ��ʾͼ��
			glfwPollEvents();		// �����¼�
		}

		glfwTerminate();
		should_stop = true;
		heracles_thread.join();	// ��ֹreturnʱ���������̻߳�û������
	}
}
