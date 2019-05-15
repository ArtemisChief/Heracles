#include "graphic_renderer.h"
#include "user_interface.h"
#include "level_manager.h"
#include <sstream>
#include <iostream>

namespace heracles {

	double user_interface::mouse_x, user_interface::mouse_y;

	int user_interface::curr_type = 1;

	rigid_body::ptr user_interface::curr_body;

	rigid_body::ptr user_interface::target_body;

	bool user_interface::is_locked = false;

	bool user_interface::show_info = true;

	// 更新GUI
	void user_interface::update_gui() {
		if (show_info) {
			graphic_renderer::draw_text(true, "Heracles", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 100.0f) / 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);

			switch (curr_type) {
			case 1:
				graphic_renderer::draw_text(true, "Function: Drag Body", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				break;
			case 2:
				graphic_renderer::draw_text(true, "Function: Place Triangle", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				break;
			case 3:
				graphic_renderer::draw_text(true, "Function: Place Rectangle", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				break;
			case 4:
				graphic_renderer::draw_text(true, "Function: Place Triangle Ground", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				break;
			case 5:
				graphic_renderer::draw_text(true, "Function: Place Rectangle Ground", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				break;
			default:
				break;
			}

			if(is_locked)
				graphic_renderer::draw_text(true, "Locking", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 1650.0f) / 2.0f, 0.37f, 1.0f, 1.0f, 1.0f);
			else
				graphic_renderer::draw_text(true, "Not Locking", (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 1650.0f) / 2.0f, 0.37f, 1.0f, 1.0f, 1.0f);

			std::string out = "Gravitational Acceleration: ";
			std::stringstream g_stringstream;
			if (world::g.y < 0)
				out += "minus";
			g_stringstream << world::g.y;
			out += g_stringstream.str().substr(0, 4);

			graphic_renderer::draw_text(true, out, (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 1770.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

			out = "K Bias Factor: ";
			g_stringstream.str("");
			g_stringstream << arbiter::k_bias_factor;
			out += g_stringstream.str().substr(0, 4);

			graphic_renderer::draw_text(true, out, (-graphic_renderer::win_width_ + 30.0f) / 2.0f, (graphic_renderer::win_height_ - 1720.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

			graphic_renderer::draw_text(true, "Press F1 to F6 to load demo and R to reset", (-graphic_renderer::win_width_ + 2508.0f) / 2.0f, (graphic_renderer::win_height_ - 70.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press 1 to 5 to change function", (-graphic_renderer::win_width_ + 2682.0f) / 2.0f, (graphic_renderer::win_height_ - 110.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press WASD to move and shift or ctrl to change speed", (-graphic_renderer::win_width_ + 2354.0f) / 2.0f, (graphic_renderer::win_height_ - 150.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Left Mouse Button to implement selected function", (-graphic_renderer::win_width_ + 2322.0f) / 2.0f, (graphic_renderer::win_height_ - 190.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Right Mouse Button to apply impulse on body", (-graphic_renderer::win_width_ + 2400.0f) / 2.0f, (graphic_renderer::win_height_ - 230.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Right Middle Button to remove selected body", (-graphic_renderer::win_width_ + 2400.0f) / 2.0f, (graphic_renderer::win_height_ - 270.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press E to toggle locking body. Press Q to cancel selected body", (-graphic_renderer::win_width_ + 2180.0f) / 2.0f, (graphic_renderer::win_height_ - 310.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Space to pause or resume. Roll Scroll to zoom in or out", (-graphic_renderer::win_width_ + 2212.0f) / 2.0f, (graphic_renderer::win_height_ - 350.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Add or Divide to change Gravitational Acceleration", (-graphic_renderer::win_width_ + 2290.0f) / 2.0f, (graphic_renderer::win_height_ - 390.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Left or Right Bracket to change K Bias Factor", (-graphic_renderer::win_width_ + 2368.0f) / 2.0f, (graphic_renderer::win_height_ - 430.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Up or Down to change the Mass of selected body", (-graphic_renderer::win_width_ + 2352.0f) / 2.0f, (graphic_renderer::win_height_ - 470.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Left or Right to change Friction Factor of selected body", (-graphic_renderer::win_width_ + 2196.0f) / 2.0f, (graphic_renderer::win_height_ - 510.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);
			graphic_renderer::draw_text(true, "Press Tab to toggle the UI", (-graphic_renderer::win_width_ + 2756.0f) / 2.0f, (graphic_renderer::win_height_ - 550.0f) / 2.0f, 0.3f, 1.0f, 1.0f, 1.0f);

			if (glfwGetMouseButton(graphic_renderer::window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
				curr_body = nullptr;
				target_body = nullptr;
			}

			if (curr_body != nullptr && curr_body != target_body && !is_locked && glfwGetMouseButton(graphic_renderer::window_ ,GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
				target_body = curr_body;

			if (target_body) {
				const auto mass = target_body->get_mass();
				if (mass == inf)
					g_stringstream.str("inf");
				else {
					g_stringstream.str("");
					g_stringstream << mass;
				}
				graphic_renderer::draw_text(true, "Mass: ", (-graphic_renderer::win_width_ + 2978.0f) / 2.0f, (graphic_renderer::win_height_ - 1570.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				graphic_renderer::draw_text(true, g_stringstream.str().substr(0, 4), (-graphic_renderer::win_width_ + 3088.0f) / 2.0f, (graphic_renderer::win_height_ - 1570.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

				if (target_body->get_velocity().magnitude() >= -0.01 &&target_body->get_velocity().magnitude() <= 0.01)
					g_stringstream.str("0.0");
				else {
					g_stringstream.str("");
					g_stringstream << target_body->get_velocity().magnitude();
				}
				graphic_renderer::draw_text(true, "Speed: ", (-graphic_renderer::win_width_ + 2958.0f) / 2.0f, (graphic_renderer::win_height_ - 1620.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				graphic_renderer::draw_text(true, g_stringstream.str().substr(0, 4), (-graphic_renderer::win_width_ + 3088.0f) / 2.0f, (graphic_renderer::win_height_ - 1620.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

				if (target_body->get_angular_velocity() >= -0.01 &&target_body->get_angular_velocity() <= 0.01)
					g_stringstream.str("0.0");
				else {
					g_stringstream.str("");
					g_stringstream << (target_body->get_angular_velocity() > 0 ? target_body->get_angular_velocity() : -target_body->get_angular_velocity());
				}
				graphic_renderer::draw_text(true, "Angular Speed: ", (-graphic_renderer::win_width_ + 2792.0f) / 2.0f, (graphic_renderer::win_height_ - 1670.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				graphic_renderer::draw_text(true, g_stringstream.str().substr(0, 4), (-graphic_renderer::win_width_ + 3088.0f) / 2.0f, (graphic_renderer::win_height_ - 1670.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

				g_stringstream.str("");
				g_stringstream << target_body->get_inertia();
				graphic_renderer::draw_text(true, "Inertia: ", (-graphic_renderer::win_width_ + 2917.0f) / 2.0f, (graphic_renderer::win_height_ - 1720.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				graphic_renderer::draw_text(true, g_stringstream.str().substr(0, 4), (-graphic_renderer::win_width_ + 3088.0f) / 2.0f, (graphic_renderer::win_height_ - 1720.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);

				if (target_body->get_friction() >= -0.01 &&target_body->get_friction() <= 0.01)
					g_stringstream.str("0.0");
				else {
					g_stringstream.str("");
					g_stringstream << target_body->get_friction();
				}
				graphic_renderer::draw_text(true, "Friction Factor: ", (-graphic_renderer::win_width_ + 2751.0f) / 2.0f, (graphic_renderer::win_height_ - 1770.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
				graphic_renderer::draw_text(true, g_stringstream.str().substr(0, 4), (-graphic_renderer::win_width_ + 3088.0f) / 2.0f, (graphic_renderer::win_height_ - 1770.0f) / 2.0f, 0.4f, 1.0f, 1.0f, 1.0f);
			}
		}
	}

	// 鼠标坐标回调函数
	void user_interface::cursor_callback(GLFWwindow* window, double x, double y) {
		const auto half_width = graphic_renderer::win_width_ / 2.0f;
		const auto half_height = graphic_renderer::win_height_ / 2.0f;

		x = (x - half_width) / half_width;
		y = (-y + half_height) / half_height;

		vec2 pos(x, y);
		pos = graphic_renderer::projection_.inv() * pos + graphic_renderer::view_;

		world::lock();
		curr_body = world::check_point_in_poly(pos);
		world::unlock();
	}

	// 鼠标按键回调函数
	void user_interface::mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {

		// 鼠标按下
		if (action == GLFW_PRESS) switch (button) {

		// 鼠标左键放置刚体
		case GLFW_MOUSE_BUTTON_LEFT: {
			if (curr_type != 1) {
				glfwGetCursorPos(window, &mouse_x, &mouse_y);
				const auto half_width = graphic_renderer::win_width_ / 2.0f;
				const auto half_height = graphic_renderer::win_height_ / 2.0f;

				mouse_x = (mouse_x - half_width) / half_width;
				mouse_y = (-mouse_y + half_height) / half_height;
			}
			break;
		}

		// 鼠标右键给某个刚体施加力
		case GLFW_MOUSE_BUTTON_RIGHT: {
			glfwGetCursorPos(graphic_renderer::window_, &mouse_x, &mouse_y);
			const auto half_width = graphic_renderer::win_width_ / 2.0f;
			const auto half_height = graphic_renderer::win_height_ / 2.0f;

			mouse_x = (mouse_x - half_width) / half_width;
			mouse_y = (-mouse_y + half_height) / half_height;
			break;
		}

		// 鼠标中键删除刚体
		case GLFW_MOUSE_BUTTON_MIDDLE:
			if (target_body)
				target_body->set_world_position(vec2(inf, inf));
			break;

		default:
			break;
		}


		//鼠标抬起
		if (action == GLFW_RELEASE) switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT: {
			if (curr_type != 1) {
				double x1, y1;
				glfwGetCursorPos(window, &x1, &y1);

				const auto half_width = graphic_renderer::win_width_ / 2.0f;
				const auto half_height = graphic_renderer::win_height_ / 2.0f;

				x1 = (x1 - half_width) / half_width;
				y1 = (-y1 + half_height) / half_height;

				vec2 pos((mouse_x + x1) / 2.0f, (mouse_y + y1) / 2.0f);
				pos = graphic_renderer::projection_.inv() * pos + graphic_renderer::view_;

				vec2 width_height(abs(x1 - mouse_x), abs(y1 - mouse_y));
				width_height = graphic_renderer::projection_.inv() * width_height;

				rigid_body::ptr body;
				world::lock();

				// 世界创造刚体
				switch (curr_type) {
				case 2:
					body = world::create_triangle(width_height.x*width_height.y / 2 * 50.0f, width_height.x, width_height.y, pos);
					break;
				case 3:
					body = world::create_rectangle(width_height.x*width_height.y * 50.0f, width_height.x, width_height.y, pos);
					break;
				case 4:
					body = world::create_tri_ground(width_height.x, width_height.y, pos);
					break;
				case 5:
					body = world::create_ground(width_height.x, width_height.y, pos);
					break;
				default:
					world::unlock();
					return;
				}

				graphic_renderer::bind_vertex_array(body);

				world::add(body);

				world::unlock();

				std::cout << "Create Box (" << body->get_id() << ") : [" << pos.x << " " << pos.y << "]" << std::endl;
			}
			break;
		}

		case GLFW_MOUSE_BUTTON_RIGHT:
			if (target_body) {
				double x1, y1;
				glfwGetCursorPos(window, &x1, &y1);
				const auto half_width = graphic_renderer::win_width_ / 2.0f;
				const auto half_height = graphic_renderer::win_height_ / 2.0f;

				x1 = (x1 - half_width) / half_width;
				y1 = (-y1 + half_height) / half_height;

				vec2 force(mouse_x - x1, mouse_y - y1);
				force = graphic_renderer::projection_.inv() * force;

				world::add_impulse(target_body, force);
			}
			break;

		default:
			break;
		}
	}

	// 鼠标滚轮回调函数
	void user_interface::scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
		if (graphic_renderer::zoom_ >= -20.0f && graphic_renderer::zoom_ <= 25.0f) {
			graphic_renderer::zoom_ += yoffset;
			graphic_renderer::projection_[0].x = 400.0f / graphic_renderer::win_width_ + graphic_renderer::zoom_ * 10.0f / graphic_renderer::win_width_;
			graphic_renderer::projection_[1].y = 400.0f / graphic_renderer::win_height_ + graphic_renderer::zoom_ * 10.0f / graphic_renderer::win_height_;
			graphic_renderer::set_shader("graphic", "projection", graphic_renderer::projection_);
		}
		if (graphic_renderer::zoom_ <= -20.0f)
			graphic_renderer::zoom_ = -20.0f;
		if (graphic_renderer::zoom_ >= 25.0f)
			graphic_renderer::zoom_ = 25.0f;
	}

	// 键盘按键回调函数
	void user_interface::keyboard_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
		if (action == GLFW_PRESS) switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(graphic_renderer::window_, true);
			break;

		case GLFW_KEY_SPACE:
			world::is_paused = !world::is_paused;
			break;

		case GLFW_KEY_TAB:
			show_info = !show_info;
			break;

		case GLFW_KEY_E:
			is_locked = !is_locked;
			break;

		case GLFW_KEY_Q:
			target_body = nullptr;
			break;;

		case GLFW_KEY_1:
		case GLFW_KEY_2:
		case GLFW_KEY_3:
		case GLFW_KEY_4:
		case GLFW_KEY_5:
			curr_type = key - 48;
			break;

		case GLFW_KEY_R:
			target_body = nullptr;
			world::lock();
			world::clear();
			world::unlock();
			break;

		// 抽方块Demo
		case GLFW_KEY_F1: 
			level_manager::level_1();
			break;
		
		// 铰链Demo
		case GLFW_KEY_F2: 
			level_manager::level_2();
			break;
		
		// 多米诺Demo
		case GLFW_KEY_F3: 
			level_manager::level_3();
			break;
		
		// 愤怒的小鸟Demo
		case GLFW_KEY_F4: 
			level_manager::level_4();
			break;

		 // 高尔顿钉板Demo
		case GLFW_KEY_F5: 
			level_manager::level_5();
			break;
		
		 // 牛顿摆Demo
		case GLFW_KEY_F6: 
			level_manager::level_6();
			break;
		
		default:
			break;
		}

		switch (key) {
		case GLFW_KEY_EQUAL:
			world::g -= vec2(0, 0.1);
			if (world::g.y >= -0.01 && world::g.y <= 0.01)
				world::g = vec2(0, 0);
			break;

		case GLFW_KEY_MINUS:
			world::g += vec2(0, 0.1);
			if (world::g.y >= -0.01 && world::g.y <= 0.01)
				world::g = vec2(0, 0);
			break;

		case GLFW_KEY_RIGHT_BRACKET:
			if (arbiter::k_bias_factor <= 0.99) 
				arbiter::k_bias_factor += 0.01;
			if (arbiter::k_bias_factor >= 0.988 && arbiter::k_bias_factor <= 1.1) 
				arbiter::k_bias_factor = 1.0;
			break;

		case GLFW_KEY_LEFT_BRACKET:
			if (arbiter::k_bias_factor > 0.21) 
				arbiter::k_bias_factor -= 0.01;
			break;

		case GLFW_KEY_UP:
			target_body->set_mass(target_body->get_mass() + 5);
			break;

		case GLFW_KEY_DOWN:
			if (target_body->get_mass() > 5)
				target_body->set_mass(target_body->get_mass() - 5);
			break;

		case GLFW_KEY_LEFT:
			if (target_body->get_friction() >= 0.051)
				target_body->set_friction(target_body->get_friction() - 0.05);
			if (target_body->get_friction() > 0.049 && target_body->get_friction() < 0.051)
				target_body->set_friction(0.0);
			break;

		case GLFW_KEY_RIGHT:
			if (target_body->get_friction() < 0.95)
				target_body->set_friction(target_body->get_friction() + 0.05);
			break;

		default:
			break;
		}
	}

	// 处理高速输入
	void user_interface::process_input() {
		if (curr_type == 1 && target_body && glfwGetMouseButton(graphic_renderer::window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			double x, y;
			glfwGetCursorPos(graphic_renderer::window_, &x, &y);
			const auto half_width = graphic_renderer::win_width_ / 2.0f;
			const auto half_height = graphic_renderer::win_height_ / 2.0f;

			x = (x - half_width) / half_width;
			y = (-y + half_height) / half_height;

			vec2 pos(x, y);
			pos = graphic_renderer::projection_.inv() * pos + graphic_renderer::view_;

			target_body->set_world_position(pos);
			target_body->set_velocity(vec2(0, 0));
			target_body->set_angular_velocity(0);
		}

		auto camera_speed = 0.02f;
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera_speed *= 8;
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera_speed /= 4;
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_W) == GLFW_PRESS)
			graphic_renderer::move_camera(vec2(0, 1 * camera_speed));
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_A) == GLFW_PRESS)
			graphic_renderer::move_camera(vec2(-1 * camera_speed, 0));
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_S) == GLFW_PRESS)
			graphic_renderer::move_camera(vec2(0, -1 * camera_speed));
		if (glfwGetKey(graphic_renderer::window_, GLFW_KEY_D) == GLFW_PRESS)
			graphic_renderer::move_camera(vec2(1 * camera_speed, 0));
	}

}