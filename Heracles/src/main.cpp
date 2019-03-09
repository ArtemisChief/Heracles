#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"
#include "engine/body.h"
#include "engine/world.h"

static GLFWwindow* window = nullptr;
static shader* shader_program = nullptr;
static heracles::world the_world({ 0.0f, -9.8f });

// 设置
static int win_width = 1600;
static int win_height = 900;
// 视点向量与投影矩阵
static float zoom = 0.0f;
static heracles::vec2 view(0.0f, 0.0f);
static heracles::mat22 projection(8.0f / win_width, 0, 0, 8.0f / win_height);

// 绘制刚体
static void draw_body(heracles::polygon_body& body) {
	shader_program->set_vec2("translation", body.get_world_position());
	shader_program->set_mat22("rotation", body.get_rotation());
	glBindVertexArray(*body.get_id());
	glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_INT, nullptr);
}

// 绘制铰链
static void draw_joint() {
	
}

// 标题栏显示dt
static void update_title(const double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// 时钟同步
static auto diff_time() {
	using namespace std::chrono;
	using seconds = duration<double>;
	static auto last_clock = high_resolution_clock::now();		// 每次调用high_resolution_clock::now()
	const auto now = high_resolution_clock::now();				// 后一次一定比前一次大
	const auto dt = duration_cast<seconds>(now - last_clock);	// 保证tick经过一个稳定的时间间隔
	update_title(dt.count());									// 显示dt
	last_clock = now;
	return dt;
}

// 绑定刚体对象的顶点，新创建的刚体只需要调用一次该函数
static void bind_vertex_array(heracles::polygon_body& body) {
	heracles::body::vertex_list vertices = body.get_vertices();

	unsigned int indices[] = {
		0, 1,	// Bottom Edge
		1, 2,	// Right Edge
		2, 3,	// Top Edge
		3, 0	// Left Edge
	};

	// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
	unsigned int* vao = body.get_id();
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
}

// 渲染
static void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto &body:the_world.get_bodies())
		draw_body(*std::dynamic_pointer_cast<heracles::polygon_body>(body).get());

	// glfw双缓冲+处理事件
	glfwSwapBuffers(window);
}

// 摄像机移动
static void move_camera(const heracles::vec2 translation) {
	view += translation;
	shader_program->set_vec2("view", view);
}

// 鼠标按键回调函数
static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {
	if (action == GLFW_PRESS) switch (button) {
		//鼠标左键放置刚体
	case GLFW_MOUSE_BUTTON_LEFT: {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		const int half_width = win_width / 2;
		const int half_height = win_height / 2;
		heracles::vec2 pos((x - half_width) / half_width, (-y + half_height) / half_height);
		pos = projection.inv() * pos + view;

		// 世界创造刚体
		heracles::polygon_body::ptr body = the_world.create_box(1, 10, 10, pos);
		the_world.add(body);

		//绑定刚体的顶点属性
		bind_vertex_array(*std::dynamic_pointer_cast<heracles::polygon_body>(body).get());
		std::cout << "Box " << body->get_id() << ": [" << pos.x << " " << pos.y << "]" << std::endl;
	}
	
	// 鼠标右键给某个刚体施加力
	case GLFW_MOUSE_BUTTON_RIGHT: {
		break;
	}
	}
}

// 鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
	if (zoom >= -10.0f && zoom <= 25.0f) {
		zoom += yoffset;
		projection[0].x = 8.0f / win_width + zoom / (win_width * 2.5f);
		projection[1].y = 8.0f / win_height + zoom / (win_height * 2.5f);
		shader_program->set_mat22("projection", projection);
	}
	if (zoom <= -10.0f)
		zoom = -10.0f;
	if (zoom >= 25.0f)
		zoom = 25.0f;
}

// 处理输入
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	auto camera_speed = 2.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera_speed *= 3;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera_speed /= 3;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		move_camera(heracles::vec2(0, 1 * camera_speed));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		move_camera(heracles::vec2(-1 * camera_speed, 0));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		move_camera(heracles::vec2(0, -1 * camera_speed));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		move_camera(heracles::vec2(1 * camera_speed, 0));
}

// 物理引擎运行部分
static std::atomic<bool> should_stop{ false };
static void heracles_run() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = diff_time().count();

		// 世界运行一个步长（Step）的运算
		the_world.step(dt);
	}
}

// 窗口大小调整时对视口（Viewport）进行调整的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	win_width = width;
	win_height = height;
	projection[0].x = 8.0f / win_width + zoom / (win_width * 2.5f);
	projection[1].y = 8.0f / win_height + zoom / (win_height * 2.5f);
	shader_program->set_mat22("projection", projection);
}

int main() {
	// glfw初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw窗口创建
	window = glfwCreateWindow(win_width, win_height, "Heracles", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 设置鼠标回调函数
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad加载OpenGL函数指针
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		return -2;
	}

	// 构造并使用点着色器和片段着色器
	shader_program = new shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader_program->use();
	shader_program->set_vec2("view", view);
	shader_program->set_mat22("projection", projection);

	// 渲染主循环
	std::thread heracles_thread(heracles_run);
	while (!glfwWindowShouldClose(window)) {
		processInput(window);	// 处理输入
		display();				// 显示图像
		glfwPollEvents();		// 处理事件
	}

	// 删除顶点数据
	//glDeleteVertexArrays(1, &vao);
	//glDeleteBuffers(1, &vbo);
	//glDeleteBuffers(1, &ebo);

	// glfw终结，释放资源
	glfwTerminate();
	should_stop = true;
	heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	return 0;
}