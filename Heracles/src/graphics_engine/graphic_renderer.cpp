#include <iostream>
#include <sstream>

#include "graphic_renderer.h"
#include "resource_manager.h"
#include <cstdarg>
#include <atomic>
#include <random>

namespace heracles {

	int type = 1;
	bool show_info = true;
	unsigned int point;
	double g3_2 = sqrt(3) / 2;

	// 窗口
	GLFWwindow* graphic_renderer::window_ = nullptr;

	// 着色器程序
	shader graphic_renderer::shader_program_;

	// 世界
	world* graphic_renderer::the_world_ = nullptr;

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

		//resource_manager::get_texture("test1").bind();
		glBindVertexArray(*body.get_id());

		set_shader("graphic", "color", 0.41f, 0.41f, 0.41f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, body.count());
		set_shader("graphic", "color", 0.9f, 0.9f, 0.9f);
		glDrawArrays(GL_LINE_LOOP, 0, body.count());
	}

	// 绘制接触点
	void graphic_renderer::draw_contact(const arbiter::ptr& arbiter) {
		glBindVertexArray(point);

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

		//float tex_coord[] = {
		//	0.0f, 0.0f,
		//	0.0f, 1.0f,
		//	1.0f, 0.0f,
		//	1.0f, 1.0f
		//};

		// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
		auto& vao = *body->get_id();
		unsigned int vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		// 处理顶点
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size * 2, nullptr, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(size));
		//glEnableVertexAttribArray(1);

		glBufferSubData(GL_ARRAY_BUFFER, 0, size, &vertices[0]);
		
		// 加载纹理
		//glBufferSubData(GL_ARRAY_BUFFER, size, size, tex_coord);
		//resource_manager::load_texture("src/resources/container.jpg", "test1");
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

	}

	// 渲染 - 所有 OpenGL 绘制放在这里进行
	void graphic_renderer::display() {
		glClearColor(0.117f, 0.117f, 0.117f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		the_world_->lock();
		for (auto &body : the_world_->get_bodies()) {
			draw_body(*std::dynamic_pointer_cast<rigid_body>(body));
		}

		if (show_info) {
			for (auto &arbiter : the_world_->get_arbiters()) {
				draw_contact(arbiter.second);
			}
		}
		the_world_->unlock();

		draw_text(true, "Heracles", (-win_width_+30.0f)/2.0f, (win_height_-100.0f)/2.0f, 1.0f, 1.0f, 1.0f, 1.0f);

		// glfw双缓冲+处理事件
		glfwSwapBuffers(window_);
	}

	// 物理引擎运行部分
	static std::atomic<bool> is_paused{ false };
	static std::atomic<bool> should_stop{ false };
	void graphic_renderer::heracles_run() {
		using namespace std::chrono_literals;
		while (!should_stop) {
			std::this_thread::sleep_for(10ms);
			auto dt = diff_time().count();

			if (!is_paused) {
				// 世界运行一个步长（Step）的运算
				the_world_->lock();
				the_world_->step(std::min(dt, 0.01));
				the_world_->unlock();
			}
		}
	}

	// 摄像机移动
	void graphic_renderer::move_camera(const vec2 translation) {
		view_ += translation;
		set_shader("graphic", "view", view_);
	}

	// 鼠标按键回调函数
	double x0, y0;
	rigid_body::ptr rb;
	void graphic_renderer::mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {
		
		if (action == GLFW_PRESS) switch (button) {
		//鼠标左键放置刚体
		case GLFW_MOUSE_BUTTON_LEFT: {
			glfwGetCursorPos(window, &x0, &y0);
			const auto half_width = win_width_ / 2.0f;
			const auto half_height = win_height_ / 2.0f;
			x0 = (x0 - half_width) / half_width;
			y0 = (-y0 + half_height) / half_height;
			break;
		}

		// 鼠标右键给某个刚体施加力
		case GLFW_MOUSE_BUTTON_RIGHT: {
			glfwGetCursorPos(window, &x0, &y0);
			const auto half_width = win_width_ / 2.0f;
			const auto half_height = win_height_ / 2.0f;

			x0 = (x0 - half_width) / half_width;
			y0 = (-y0 + half_height) / half_height;

			vec2 pos(x0, y0);
			pos = projection_.inv() * pos + view_;

			the_world_->lock();
			rb = the_world_->check_point_in_poly(pos);
			the_world_->unlock();
			break;
		}

		// 鼠标中键删除刚体
		case GLFW_MOUSE_BUTTON_MIDDLE: {
			glfwGetCursorPos(window, &x0, &y0);
			const auto half_width = win_width_ / 2.0f;
			const auto half_height = win_height_ / 2.0f;

			x0 = (x0 - half_width) / half_width;
			y0 = (-y0 + half_height) / half_height;

			vec2 pos(x0, y0);
			pos = projection_.inv() * pos + view_;

			the_world_->lock();
			rb = the_world_->check_point_in_poly(pos);
			the_world_->unlock();
			break;
		}
		default:;
		}

		if (action == GLFW_RELEASE) switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: {

			double x1, y1;
			glfwGetCursorPos(window, &x1, &y1);

			const auto half_width = win_width_ / 2.0f;
			const auto half_height = win_height_ / 2.0f;

			x1 = (x1 - half_width) / half_width;
			y1 = (-y1 + half_height) / half_height;

			vec2 pos((x0 + x1) / 2.0f, (y0 + y1) / 2.0f);
			pos = projection_.inv() * pos + view_;

			vec2 width_height(abs(x1 - x0), abs(y1 - y0));
			width_height = projection_.inv() * width_height;

			rigid_body::ptr body;
			the_world_->lock();
			// 世界创造刚体
			switch (type) {
			case 1:
				body = the_world_->create_ground(width_height.x, width_height.y, pos);
				break;
			case 2:
				body = the_world_->create_triangle(width_height.x*width_height.y / 2 * 50.0f, width_height.x, width_height.y, pos);
				break;
			case 3:
				body = the_world_->create_rectangle(width_height.x*width_height.y * 50.0f, width_height.x, width_height.y, pos);
				break;
			default:
				the_world_->unlock();
				return;
			}

			bind_vertex_array(body);

			the_world_->add(body);
			the_world_->unlock();

			std::cout << "Create Box (" << body->get_id() << ") : [" << pos.x << " " << pos.y << "]" << std::endl;
			break;
		}
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (rb) {
				double x1, y1;
				glfwGetCursorPos(window, &x1, &y1);
				const auto half_width = win_width_ / 2.0f;
				const auto half_height = win_height_ / 2.0f;

				x1 = (x1 - half_width) / half_width;
				y1 = (-y1 + half_height) / half_height;

				vec2 force(x0 - x1, y0 - y1);
				force = projection_.inv() * force;

				the_world_->add_impulse(rb, force);
			}
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			if (rb) {
				//the_world_->del(rb);//不能直接删除
				rb->set_world_position(vec2(inf, inf));
			}
			break;
		default:
			break;
		}
	}

	// 鼠标滚轮回调函数
	void graphic_renderer::scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
		if (zoom_ >= -20.0f && zoom_ <= 25.0f) {
			zoom_ += yoffset;
			projection_[0].x = 400.0f / win_width_ + zoom_ * 10.0f / win_width_;
			projection_[1].y = 400.0f / win_height_ + zoom_ * 10.0f / win_height_;
			set_shader("graphic", "projection", projection_);
		}
		if (zoom_ <= -20.0f)
			zoom_ = -20.0f;
		if (zoom_ >= 25.0f)
			zoom_ = 25.0f;
	}

	// 键盘按键回调函数
	void graphic_renderer::keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			is_paused = !is_paused;

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
			show_info = !show_info;

		if ((key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 || key == GLFW_KEY_4) && action == GLFW_PRESS)
			type = key - 48;

		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();
			the_world_->unlock();
		}

		// 金字塔Demo
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			auto body = the_world_->create_ground(10, 0.3, vec2(0, -2.25));
			bind_vertex_array(body);
			the_world_->add(body);

			vec2 x(-2.5f, -1.0f);
			for (auto i = 0; i < 9; ++i) {
				auto y = x;
				for (auto j = i; j < 9; ++j) {
					body = the_world_->create_rectangle(10, 0.4, 0.4, y);
					body->set_friction(0.2);
					bind_vertex_array(body);
					the_world_->add(body);
					y += vec2(0.6f, 0.0f);
				}
				x += vec2(0.3f, 0.6f);
			}

			the_world_->unlock();
		}

		//铰链Demo
		if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			const auto ground = the_world_->create_ground(10, 0.3, vec2(0, -2.25));
			ground->set_friction(0.4);
			bind_vertex_array(ground);
			the_world_->add(ground);

			const auto mass = 10.0f;
			const auto y = 2.0f;

			auto last = ground;
			for (auto i = 0; i < 15; ++i) {
				auto box = the_world_->create_rectangle(mass, 0.2, 0.075, vec2(0.15 + 0.35 * i, y));
				box->set_friction(0.4);
				bind_vertex_array(box);
				the_world_->add(box);
				const auto joint = the_world_->create_revolute_joint(last, box, vec2(0.35 * i, y));
				the_world_->add(joint);
				last = box;
			}

			the_world_->unlock();
		}

		//多米诺Demo
		if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			const auto ground = the_world_->create_ground(20, 0.3, vec2(5, -2.25));
			ground->set_friction(0.8);
			bind_vertex_array(ground);
			the_world_->add(ground);

			const auto platform = the_world_->create_ground(1, 0.15, vec2(-3.5, 0));
			platform->set_friction(0.2);
			bind_vertex_array(platform);
			the_world_->add(platform);

			auto bullet = the_world_->create_rectangle(20, 0.3, 0.3, vec2(-3.8, 0.3));
			bullet->set_friction(0.8);
			bind_vertex_array(bullet);
			the_world_->add(bullet);

			for (auto i = 0; i < 20; ++i) {
				auto box = the_world_->create_rectangle(20, 0.15, 2.5, vec2(-2.8 +  0.7 * i, -0.75));
				box->set_friction(0.8);
				bind_vertex_array(box);
				the_world_->add(box);
			}

			the_world_->unlock();
		}

		// 愤怒的小鸟Demo
		if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			const auto ground = the_world_->create_ground(10, 0.3, vec2(0, -2.25));
			ground->set_friction(0.95);
			bind_vertex_array(ground);
			the_world_->add(ground);

			const auto platform = the_world_->create_ground(0.5, 0.15, vec2(-2.8, -1.1));
			platform->set_friction(0.2);
			bind_vertex_array(platform);
			the_world_->add(platform);

			auto bird = the_world_->create_rectangle(10, 0.3, 0.3, vec2(-2.8, -0.8));
			bird->set_friction(0.95);
			bind_vertex_array(bird);
			the_world_->add(bird);

			for (auto i = 0; i < 4; ++i) {
				auto box1 = the_world_->create_rectangle(5, 0.25, 0.8, vec2(2.8, -1.7 + 0.93 * i));
				box1->set_friction(0.95);
				bind_vertex_array(box1);
				the_world_->add(box1);

				auto box2 = the_world_->create_rectangle(5, 0.25, 0.8, vec2(3.5, -1.7 + 0.93 * i));
				box2->set_friction(0.95);
				bind_vertex_array(box2);
				the_world_->add(box2);

				auto box3 = the_world_->create_rectangle(5, 0.1, 1.0, vec2(3.15, -1.235 + 0.93 * i));
				box3->set_friction(0.95);
				box3->set_rotation(pi/2);
				bind_vertex_array(box3);
				the_world_->add(box3);
			}

			auto top = the_world_->create_triangle(5, 1.0, 0.6, vec2(3.15, 2.0));
			top->set_friction(0.95);
			bind_vertex_array(top);
			the_world_->add(top);

			the_world_->unlock();
		}

		// 高尔顿钉板Demo
		if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			auto left = the_world_->create_ground(0.3, 10, vec2(4, 0));
			bind_vertex_array(left);
			the_world_->add(left);

			auto right = the_world_->create_ground(0.3, 10, vec2(-4, 0));
			bind_vertex_array(right);
			the_world_->add(right);

			auto left_ = the_world_->create_ground(0.01, 7, vec2(2, 1));
			bind_vertex_array(left_);
			left_->set_rotation(pi / 6);
			the_world_->add(left_);

			auto right_ = the_world_->create_ground(0.01, 7, vec2(-2, 1));
			bind_vertex_array(right_);
			right_->set_rotation(-pi / 6);
			the_world_->add(right_);

			auto body = the_world_->create_ground(10, 0.3, vec2(0, -4));
			bind_vertex_array(body);
			the_world_->add(body);

			body = the_world_->create_tri_ground(0.4, 0.4*g3_2, vec2(-0.4,3.6));
			body->set_friction(0.2);
			bind_vertex_array(body);
			the_world_->add(body);

			body = the_world_->create_tri_ground(0.4, 0.4*g3_2, vec2(0.4, 3.6));
			body->set_friction(0.2);
			bind_vertex_array(body);
			the_world_->add(body);

			vec2 x(-0.4f, 3.6f);
			for (auto i = 1; i < 9; ++i) {
				x -= vec2(0.4f, 0.8*g3_2);
				auto y = x;
				for (auto j = 0; j < i+2; ++j) {
					if (i == 8)
					{
						body = the_world_->create_ground(0.1, 1.73, y - vec2(0.0f, 1.03f));
						body->set_friction(0.2);
						bind_vertex_array(body);
						the_world_->add(body);
					}

					body = the_world_->create_tri_ground(0.4,0.4*g3_2, y);
					body->set_friction(0.2);
					bind_vertex_array(body);
					the_world_->add(body);

					y += vec2(0.8f, 0.0f);
				}
			}

			body = the_world_->create_rectangle(10,0.3, 0.3, vec2(0, 4));
			body->set_friction(0.2);
			bind_vertex_array(body);
			the_world_->add(body);

			the_world_->unlock();
		}

		// 打砖块Demo（未完成）
		if (key == GLFW_KEY_F6 && action == GLFW_PRESS) {
			the_world_->lock();
			the_world_->clear();

			const auto ground = the_world_->create_ground(10, 0.3, vec2(0, -2.25));
			ground->set_friction(0);
			bind_vertex_array(ground);
			the_world_->add(ground);

			const auto left = the_world_->create_ground(0.3, 10, vec2(-4, 0));
			left->set_friction(0);
			bind_vertex_array(left);
			the_world_->add(left);

			const auto right = the_world_->create_ground(0.3, 10, vec2(4, 0));
			right->set_friction(0);
			bind_vertex_array(right);
			the_world_->add(right);

			const auto top = the_world_->create_ground(10, 0.3, vec2(0, 2.25));
			top->set_friction(0);
			bind_vertex_array(top);
			the_world_->add(top);

			/*const auto platform = the_world_->create_ground(0.5, 0.15, vec2(-2.8, -1.1));
			ground->set_friction(0.2);
			bind_vertex_array(platform);
			the_world_->add(platform);

			auto bird = the_world_->create_rectangle(10, 0.3, 0.3, vec2(-2.8, -0.8));
			bird->set_friction(0.95);
			bind_vertex_array(bird);
			the_world_->add(bird);

			for (auto i = 0; i < 4; ++i) {
			auto box1 = the_world_->create_rectangle(5, 0.25, 0.8, vec2(2.8, -1.7 + 0.93 * i));
			box1->set_friction(0.95);
			bind_vertex_array(box1);
			the_world_->add(box1);

			auto box2 = the_world_->create_rectangle(5, 0.25, 0.8, vec2(3.5, -1.7 + 0.93 * i));
			box2->set_friction(0.95);
			bind_vertex_array(box2);
			the_world_->add(box2);

			auto box3 = the_world_->create_rectangle(5, 0.1, 1.0, vec2(3.15, -1.235 + 0.93 * i));
			box3->set_friction(0.95);
			box3->set_rotation(pi / 2);
			bind_vertex_array(box3);
			the_world_->add(box3);
			}

			auto top = the_world_->create_triangle(5, 1.0, 0.6, vec2(3.15, 2.0));
			top->set_friction(0.95);
			bind_vertex_array(top);
			the_world_->add(top);*/

			the_world_->unlock();
		}
	}

	// 处理输入
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
		glfwSetMouseButtonCallback(window_, mouse_callback);
		glfwSetScrollCallback(window_, scroll_callback);
		glfwSetKeyCallback(window_, keyboard_callback);

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
		text_->load("src/resources/consola.ttf", 48);

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
		glGenVertexArrays(1, &point);
		glGenBuffers(1, &vbo);
		glBindVertexArray(point);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof p, &p, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		// 设置点大小
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);

		// 构造世界
		the_world_ = new world({ 0.0f, -9.8f });

		return 0;
	}

	// 渲染主循环
	void graphic_renderer::loop() {
		std::thread heracles_thread(heracles_run);

		while (!glfwWindowShouldClose(window_)) {
			process_input();		// 处理输入
			display();				// 显示图像
			glfwPollEvents();		// 处理事件
		}

		glfwTerminate();
		should_stop = true;
		heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	}
}
