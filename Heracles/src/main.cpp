#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"

static GLFWwindow* window = nullptr;
static shader *shader_program = nullptr;

// 设置
static constexpr int win_width = 800;
static constexpr int win_height = 800;

// 绘制刚体
static void draw_body(unsigned int &vao) {
	const heracles::vec2 translation(0, cos(glfwGetTime()));	//世界坐标：平移
	const heracles::mat22 rotation(glfwGetTime());			//世界坐标：旋转
	const heracles::mat22 view(1, 0, 0, 1);		//摄像机坐标
	const heracles::mat22 projection(1, 0, 0, 1);	//投影透视坐标

	shader_program->set_vec2("translation", translation);
	shader_program->set_mat22("rotation", rotation);
	shader_program->set_mat22("view", view);
	shader_program->set_mat22("projection", projection);
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);
}

// 绘制铰链
static void draw_joint() {
	
}

// 标题栏显示dt
static void UpdateTitle(const double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// 渲染				//此处应无参数，没有实现刚体和世界类暂时这么写
static void display(unsigned int &vao) {
	glClear(GL_COLOR_BUFFER_BIT);

	//for (Body: world.bodyList)
		draw_body(vao);

	// glfw双缓冲+处理事件
	glfwSwapBuffers(window);
}

// 处理输入操作
static void keyboard(GLFWwindow* window, const int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	default: ;
	}
}

// 时钟同步
static auto diff_time() {
	using namespace std::chrono;
	using seconds = duration<double>;
	static auto last_clock = high_resolution_clock::now();		// 每次调用high_resolution_clock::now()
	const auto now = high_resolution_clock::now();				// 后一次一定比前一次大
	const auto dt = duration_cast<seconds>(now - last_clock);	// 保证tick经过一个稳定的时间间隔
	UpdateTitle(dt.count());									// 显示dt
	last_clock = now;
	return dt;
}

// 物理引擎运行部分
static std::atomic<bool> should_stop{ false };
static void heracles_run() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = diff_time().count();

		// 世界运行一个步长（Step）的运算
		// world.Step(dt);
	}
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
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// glad加载OpenGL函数指针
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		return -2;
	}

	// 构造并使用点着色器和片段着色器
	shader_program = new shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader_program->use();

	// 画四边形的Demo，之后会改用工厂模式生成刚体类的对象
	// 设置顶点数组，配置顶点数组对象（VAO）与顶点缓冲对象（VBO）
	float vertices[] = {
		-0.5f, -0.5f, // Bottom Left
		 0.5f, -0.5f, // Bottom Right
		 0.5f,  0.5f, // Top Right
		-0.5f,  0.5f  // Top Left
	};

	unsigned int indices[] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0
	};

	unsigned int vbo, vao, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// 处理顶点
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// 设置按键回调函数
	glfwSetKeyCallback(window, keyboard);

	// 渲染主循环
	std::thread heracles_thread(heracles_run);
	while (!glfwWindowShouldClose(window)) {
		display(vao);		// 显示图像
		glfwPollEvents();	// 处理按键事件
	}

	// 删除顶点数据
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	// glfw终结，释放资源
	glfwTerminate();
	should_stop = true;
	heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	return 0;
}