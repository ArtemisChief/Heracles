#include <atomic>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "util/shader.h"
#include "engine/body.h"

static GLFWwindow* window = nullptr;
static shader* shader_program = nullptr;

// ����
static constexpr int win_width = 800;
static constexpr int win_height = 800;

// ���Ƹ���
static void draw_body(const heracles::body& body) {
	shader_program->set_vec2("translation",body.get_world_position());
	shader_program->set_mat22("rotation", body.get_rotation());
	glBindVertexArray(body.get_id());
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

// ��Ⱦ				//�˴�Ӧ�޲�����û��ʵ����������ʱ��ôд
static void display(const heracles::body& body) {
	glClear(GL_COLOR_BUFFER_BIT);

	//for (body: world.bodyList)
		draw_body(body);

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

	shader_program->set_mat22("view", heracles::mat22(1, 0, 0, 1));
	shader_program->set_mat22("projection", heracles::mat22(0.01, 0, 0, 0.01));

	// ���ı��ε�Demo��֮�����ù���ģʽ���ɸ�����Ķ���
	// ���ö������飬���ö����������VAO���붥�㻺�����VBO��
	heracles::body::vertex_list vertices = {
		heracles::vec2(-5.0f, -5.0f),	// Bottom Left
		heracles::vec2( 5.0f, -5.0f),	// Bottom Right
		heracles::vec2( 5.0f,  5.0f),	// Top Right
		heracles::vec2(-5.0f,  5.0f)	// Top Left
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

	const heracles::polygon_body test_polygon_body(vao, 1.0, vertices);

	// ������
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	// ���ð����ص�����
	glfwSetKeyCallback(window, keyboard);

	// ��Ⱦ��ѭ��
	std::thread heracles_thread(heracles_run);
	while (!glfwWindowShouldClose(window)) {
		display(test_polygon_body);		// ��ʾͼ��
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