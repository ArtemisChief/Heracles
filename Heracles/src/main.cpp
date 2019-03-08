#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"

static GLFWwindow* window = nullptr;
static shader *shader_program = nullptr;

// ����
static constexpr int win_width = 800;
static constexpr int win_height = 800;

// ���Ƹ���
static void draw_body(unsigned int &vao) {
	const heracles::vec2 translation(0, cos(glfwGetTime()));	//�������꣺ƽ��
	const heracles::mat22 rotation(glfwGetTime());			//�������꣺��ת
	const heracles::mat22 view(1, 0, 0, 1);		//���������
	const heracles::mat22 projection(1, 0, 0, 1);	//ͶӰ͸������

	shader_program->set_vec2("translation", translation);
	shader_program->set_mat22("rotation", rotation);
	shader_program->set_mat22("view", view);
	shader_program->set_mat22("projection", projection);
	glBindVertexArray(vao);
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);
}

// ���ƽ���
static void draw_joint() {
	
}

// ��������ʾdt
static void UpdateTitle(const double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// ��Ⱦ				//�˴�Ӧ�޲�����û��ʵ�ָ������������ʱ��ôд
static void display(unsigned int &vao) {
	glClear(GL_COLOR_BUFFER_BIT);

	//for (Body: world.bodyList)
		draw_body(vao);

	// glfw˫����+�����¼�
	glfwSwapBuffers(window);
}

// �����������
static void keyboard(GLFWwindow* window, const int key, int scancode, int action, int mods) {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	default: ;
	}
}

// ʱ��ͬ��
static auto diff_time() {
	using namespace std::chrono;
	using seconds = duration<double>;
	static auto last_clock = high_resolution_clock::now();		// ÿ�ε���high_resolution_clock::now()
	const auto now = high_resolution_clock::now();				// ��һ��һ����ǰһ�δ�
	const auto dt = duration_cast<seconds>(now - last_clock);	// ��֤tick����һ���ȶ���ʱ����
	UpdateTitle(dt.count());									// ��ʾdt
	last_clock = now;
	return dt;
}

// �����������в���
static std::atomic<bool> should_stop{ false };
static void heracles_run() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = diff_time().count();

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
	window = glfwCreateWindow(win_width, win_height, "Heracles", nullptr, nullptr);
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
	shader_program = new shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader_program->use();

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

	unsigned int vbo, vao, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// ������
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// ���ð����ص�����
	glfwSetKeyCallback(window, keyboard);

	// ��Ⱦ��ѭ��
	std::thread heracles_thread(heracles_run);
	while (!glfwWindowShouldClose(window)) {
		display(vao);		// ��ʾͼ��
		glfwPollEvents();	// �������¼�
	}

	// ɾ����������
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	// glfw�սᣬ�ͷ���Դ
	glfwTerminate();
	should_stop = true;
	heracles_thread.join();	// ��ֹreturnʱ���������̻߳�û������
	return 0;
}