#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"

static GLFWwindow* window = nullptr;
static Shader *shader = nullptr;

// 设置
static constexpr int WIN_WIDTH = 800;
static constexpr int WIN_HEIGHT = 800;

// 绘制刚体
static void DrawBody(unsigned int &VAO) {

	Heracles::Vec2 translation(0, cos(glfwGetTime()));	//世界坐标：平移
	Heracles::Mat22 rotation(glfwGetTime());			//世界坐标：旋转
	Heracles::Mat22 view(1, 0, 0, 1);		//摄像机坐标
	Heracles::Mat22 projection(1, 0, 0, 1);	//投影透视坐标

	shader->setVec2("translation", translation);
	shader->setMat22("rotation", rotation);
	shader->setMat22("view", view);
	shader->setMat22("projection", projection);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);

}

// 绘制铰链
static void DrawJoint() {

}

// 标题栏显示dt
static void UpdateTitle(double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// 渲染				//此处应无参数，没有实现刚体和世界类暂时这么写
static void Display(unsigned int &VAO) {
	glClear(GL_COLOR_BUFFER_BIT);

	//for (Body: world.bodyList)
		DrawBody(VAO);

	// glfw双缓冲+处理事件
	glfwSwapBuffers(window);
}

// 处理输入操作
static void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	}
}

// 时钟同步
static auto DiffTime() {
	using namespace std::chrono;
	using Seconds = duration<double>;
	static auto last_clock = high_resolution_clock::now();		// 每次调用high_resolution_clock::now()
	const auto now = high_resolution_clock::now();				// 后一次一定比前一次大
	const auto dt = duration_cast<Seconds>(now - last_clock);	// 保证tick经过一个稳定的时间间隔
	UpdateTitle(dt.count());									// 显示dt
	last_clock = now;
	return dt;
}

// 物理引擎运行部分
static std::atomic<bool> should_stop{ false };
static void HeraclesRun() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = DiffTime().count();

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
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Heracles", nullptr, nullptr);
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
	shader = new Shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader->use();

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

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// 处理顶点
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// 设置按键回调函数
	glfwSetKeyCallback(window, Keyboard);

	// 渲染主循环
	std::thread Heracles_thread(HeraclesRun);
	while (!glfwWindowShouldClose(window)) {
		Display(VAO);		// 显示图像
		glfwPollEvents();	// 处理按键事件
	}

	// 删除顶点数据
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw终结，释放资源
	glfwTerminate();
	should_stop = true;
	Heracles_thread.join();	// 防止return时物理引擎线程还没运行完
	return 0;
}