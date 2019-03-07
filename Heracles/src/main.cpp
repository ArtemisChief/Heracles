#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"

static GLFWwindow* window = nullptr;
static Shader *shader = nullptr;

// ����
static constexpr int WIN_WIDTH = 800;
static constexpr int WIN_HEIGHT = 800;

// ���Ƹ���
static void DrawBody(unsigned int &VAO) {

	Heracles::Vec2 translation(0, cos(glfwGetTime()));	//�������꣺ƽ��
	Heracles::Mat22 rotation(glfwGetTime());			//�������꣺��ת
	Heracles::Mat22 view(1, 0, 0, 1);		//���������
	Heracles::Mat22 projection(1, 0, 0, 1);	//ͶӰ͸������

	shader->setVec2("translation", translation);
	shader->setMat22("rotation", rotation);
	shader->setMat22("view", view);
	shader->setMat22("projection", projection);
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);

}

// ���ƽ���
static void DrawJoint() {

}

// ��������ʾdt
static void UpdateTitle(double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// ��Ⱦ				//�˴�Ӧ�޲�����û��ʵ�ָ������������ʱ��ôд
static void Display(unsigned int &VAO) {
	glClear(GL_COLOR_BUFFER_BIT);

	//for (Body: world.bodyList)
		DrawBody(VAO);

	// glfw˫����+�����¼�
	glfwSwapBuffers(window);
}

// �����������
static void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	}
}

// ʱ��ͬ��
static auto DiffTime() {
	using namespace std::chrono;
	using Seconds = duration<double>;
	static auto last_clock = high_resolution_clock::now();		// ÿ�ε���high_resolution_clock::now()
	const auto now = high_resolution_clock::now();				// ��һ��һ����ǰһ�δ�
	const auto dt = duration_cast<Seconds>(now - last_clock);	// ��֤tick����һ���ȶ���ʱ����
	UpdateTitle(dt.count());									// ��ʾdt
	last_clock = now;
	return dt;
}

// �����������в���
static std::atomic<bool> should_stop{ false };
static void HeraclesRun() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = DiffTime().count();

		// ��������һ��������Step��������
		// world.Step(dt);
	}
}


int main() {
	// glfw��ʼ��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw���ڴ���
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Heracles", nullptr, nullptr);
	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// glad����OpenGL����ָ��
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		return -2;
	}

	// ���첢ʹ�õ���ɫ����Ƭ����ɫ��
	shader = new Shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader->use();

	// ���ı��ε�Demo��֮�����ù���ģʽ���ɸ�����Ķ���
	// ���ö������飬���ö����������VAO���붥�㻺�����VBO��
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

	// ������
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// ���ð����ص�����
	glfwSetKeyCallback(window, Keyboard);

	// ��Ⱦ��ѭ��
	std::thread Heracles_thread(HeraclesRun);
	while (!glfwWindowShouldClose(window)) {
		Display(VAO);		// ��ʾͼ��
		glfwPollEvents();	// �������¼�
	}

	// ɾ����������
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw�սᣬ�ͷ���Դ
	glfwTerminate();
	should_stop = true;
	Heracles_thread.join();	// ��ֹreturnʱ���������̻߳�û������
	return 0;
}