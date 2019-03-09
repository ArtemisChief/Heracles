#include "game_renderer.h"

namespace heracles {

	// 窗口
	GLFWwindow* game_renderer::window_ = nullptr;

	// 着色器程序
	shader* game_renderer::shader_program_ = nullptr;

	// 世界
	world* game_renderer::the_world_ = new world({ 0.0f, -9.8f });

	// 窗口大小初始设置
	int game_renderer::win_width_ = 1600;
	int game_renderer::win_height_ = 900;

	// 视点向量与投影矩阵
	float game_renderer::zoom_ = 0.0f;
	vec2 game_renderer::view_(0.0f, 0.0f);
	mat22 game_renderer::projection_(8.0f / win_width_, 0, 0, 8.0f / win_height_);

	// 绘制刚体
	void game_renderer::draw_body(polygon_body& body) {
		shader_program_->set_vec2("translation", body.get_world_position());
		shader_program_->set_mat22("rotation", body.get_rotation());
		//glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(*body.get_id());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	// 绘制铰链
	void game_renderer::draw_joint() {

	}

	// 标题栏显示dt
	void game_renderer::update_title(const double dt) {
		std::stringstream ss;
		ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
		glfwSetWindowTitle(window_, ss.str().c_str());
	}

	// 时钟同步
	auto game_renderer::diff_time() {
		using namespace std::chrono;
		using seconds = duration<double>;
		static auto last_clock = high_resolution_clock::now();				// 每次调用high_resolution_clock::now()
		const auto now = high_resolution_clock::now();				// 后一次一定比前一次大
		const auto dt = duration_cast<seconds>(now - last_clock);	// 保证tick经过一个稳定的时间间隔
		update_title(dt.count());									// 显示dt
		last_clock = now;
		return dt;
	}

	// 绑定刚体对象的顶点，新创建的刚体只需要调用一次该函数
	void game_renderer::bind_vertex_array(polygon_body& body) {
		auto vertices = body.get_vertices();

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
		const auto vao = body.get_id();
		unsigned int vbo, ebo;
		glGenVertexArrays(1, vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		// 处理顶点
		glBindVertexArray(*vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		//glGenTextures(1, &texture);
		//glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		//// set the texture wrapping parameters
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//// set texture filtering parameters
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//// load image, create texture and generate mipmaps
		//stbi_set_flip_vertically_on_load(true);
		//int width, height, nrChannels;
		//// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
		//unsigned char *data = stbi_load("src/resources/test.jpg", &width, &height, &nrChannels, 0);
		//if (data) {
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//	glGenerateMipmap(GL_TEXTURE_2D);
		//}
		//else {
		//	std::cout << "Failed to load texture" << std::endl;
		//}
		//stbi_image_free(data);
	}

	// 渲染
	void game_renderer::display() {
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto &body : the_world_->get_bodies())
			draw_body(*std::dynamic_pointer_cast<polygon_body>(body).get());

		// glfw双缓冲+处理事件
		glfwSwapBuffers(window_);
	}

	// 摄像机移动
	void game_renderer::move_camera(const vec2 translation) {
		view_ += translation;
		shader_program_->set_vec2("view", view_);
	}

	// 鼠标按键回调函数
	void game_renderer::mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {
		if (action == GLFW_PRESS) switch (button) {
			//鼠标左键放置刚体
		case GLFW_MOUSE_BUTTON_LEFT: {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			const int half_width = win_width_ / 2;
			const int half_height = win_height_ / 2;
			vec2 pos((x - half_width) / half_width, (-y + half_height) / half_height);
			pos = projection_.inv() * pos + view_;

			// 世界创造刚体
			polygon_body::ptr body = the_world_->create_box(1, 10, 10, pos);
			the_world_->add(body);

			//绑定刚体的顶点属性
			bind_vertex_array(*std::dynamic_pointer_cast<polygon_body>(body).get());

			std::cout << "Create Box " << body->get_id() << ": [" << pos.x << " " << pos.y << "]" << std::endl;
		}

									 // 鼠标右键给某个刚体施加力
		case GLFW_MOUSE_BUTTON_RIGHT: {
			break;
		}
		}
	}

	// 鼠标滚轮回调函数
	void game_renderer::scroll_callback(GLFWwindow* window, const double xoffset, double yoffset) {
		if (zoom_ >= -10.0f && zoom_ <= 25.0f) {
			zoom_ += yoffset;
			projection_[0].x = 8.0f / win_width_ + zoom_ / (win_width_ * 2.5f);
			projection_[1].y = 8.0f / win_height_ + zoom_ / (win_height_ * 2.5f);
			shader_program_->set_mat22("projection", projection_);
		}
		if (zoom_ <= -10.0f)
			zoom_ = -10.0f;
		if (zoom_ >= 25.0f)
			zoom_ = 25.0f;
	}

	// 处理输入
	void game_renderer::process_input(GLFWwindow *window) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		auto camera_speed = 2.0f;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera_speed *= 3;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera_speed /= 3;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			move_camera(vec2(0, 1 * camera_speed));
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			move_camera(vec2(-1 * camera_speed, 0));
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			move_camera(vec2(0, -1 * camera_speed));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			move_camera(vec2(1 * camera_speed, 0));
	}

	// 窗口大小调整时对视口（Viewport）进行调整的回调函数
	void game_renderer::framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
		glViewport(0, 0, width, height);
		win_width_ = width;
		win_height_ = height;
		projection_[0].x = 8.0f / win_width_ + zoom_ / (win_width_ * 2.5f);
		projection_[1].y = 8.0f / win_height_ + zoom_ / (win_height_ * 2.5f);
		shader_program_->set_mat22("projection", projection_);
	}

	// 物理引擎运行部分
	static std::atomic<bool> should_stop{ false };
	void game_renderer::heracles_run() {
		using namespace std::chrono_literals;
		while (!should_stop) {
			std::this_thread::sleep_for(10ms);
			auto dt = diff_time().count();

			// 世界运行一个步长（Step）的运算
			the_world_->step(dt);
		}
	}

	// 初始化
	int game_renderer::init() {
		// glfw初始化
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// glfw窗口创建
		window_ = glfwCreateWindow(win_width_, win_height_, "Heracles", nullptr, nullptr);
		if (window_ == nullptr) {
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window_);
		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);

		// 设置鼠标回调函数
		glfwSetMouseButtonCallback(window_, mouse_callback);
		glfwSetScrollCallback(window_, scroll_callback);

		// glad加载OpenGL函数指针
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			return -2;
		}

		// 构造并使用点着色器和片段着色器
		shader_program_ = new shader("src/Shader/Shader.v", "src/Shader/Shader.f");
		shader_program_->use();
		shader_program_->set_vec2("view", view_);
		shader_program_->set_mat22("projection", projection_);

		// 线框模式
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		return 0;
	}

	// 渲染主循环
	void game_renderer::loop() {
		std::thread heracles_thread(heracles_run);

		while (!glfwWindowShouldClose(window_)) {
			process_input(window_);	// 处理输入
			display();				// 显示图像
			glfwPollEvents();		// 处理事件
		}

		glfwTerminate();
		should_stop = true;
		heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	}
}