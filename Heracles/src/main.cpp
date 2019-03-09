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

// ����
static int win_width = 1600;
static int win_height = 900;
// �ӵ�������ͶӰ����
static float zoom = 0.0f;
static heracles::vec2 view(0.0f, 0.0f);
static heracles::mat22 projection(8.0f / win_width, 0, 0, 8.0f / win_height);

// ���Ƹ���
static void draw_body(heracles::polygon_body& body) {
	shader_program->set_vec2("translation", body.get_world_position());
	shader_program->set_mat22("rotation", body.get_rotation());
	glBindVertexArray(*body.get_id());
	glDrawElements(GL_LINE_STRIP, 8, GL_UNSIGNED_INT, nullptr);
}

// ���ƽ���
static void draw_joint() {
	
}

// ��������ʾdt
static void update_title(const double dt) {
	std::stringstream ss;
	ss << "Heracles - dt: " << std::to_string(dt * 1000).substr(0, 5) << " ms";
	glfwSetWindowTitle(window, ss.str().c_str());
}

// ʱ��ͬ��
static auto diff_time() {
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
static void bind_vertex_array(heracles::polygon_body& body) {
	heracles::body::vertex_list vertices = body.get_vertices();

	unsigned int indices[] = {
		0, 1,	// Bottom Edge
		1, 2,	// Right Edge
		2, 3,	// Top Edge
		3, 0	// Left Edge
	};

	// ���ö������飬���ö����������VAO���붥�㻺�����VBO��
	unsigned int* vao = body.get_id();
	unsigned int vbo, ebo;
	glGenVertexArrays(1, vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// ������
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
}

// ��Ⱦ
static void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (auto &body:the_world.get_bodies())
		draw_body(*std::dynamic_pointer_cast<heracles::polygon_body>(body).get());

	// glfw˫����+�����¼�
	glfwSwapBuffers(window);
}

// ������ƶ�
static void move_camera(const heracles::vec2 translation) {
	view += translation;
	shader_program->set_vec2("view", view);
}

// ��갴���ص�����
static void mouse_callback(GLFWwindow* window, const int button, const int action, const int mods) {
	if (action == GLFW_PRESS) switch (button) {
		//���������ø���
	case GLFW_MOUSE_BUTTON_LEFT: {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		const int half_width = win_width / 2;
		const int half_height = win_height / 2;
		heracles::vec2 pos((x - half_width) / half_width, (-y + half_height) / half_height);
		pos = projection.inv() * pos + view;

		// ���紴�����
		heracles::polygon_body::ptr body = the_world.create_box(1, 10, 10, pos);
		the_world.add(body);

		//�󶨸���Ķ�������
		bind_vertex_array(*std::dynamic_pointer_cast<heracles::polygon_body>(body).get());
		std::cout << "Box " << body->get_id() << ": [" << pos.x << " " << pos.y << "]" << std::endl;
	}
	
	// ����Ҽ���ĳ������ʩ����
	case GLFW_MOUSE_BUTTON_RIGHT: {
		break;
	}
	}
}

// �����ֻص�����
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

// ��������
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

// �����������в���
static std::atomic<bool> should_stop{ false };
static void heracles_run() {
	using namespace std::chrono_literals;
	while (!should_stop) {
		std::this_thread::sleep_for(10ms);
		auto dt = diff_time().count();

		// ��������һ��������Step��������
		the_world.step(dt);
	}
}

// ���ڴ�С����ʱ���ӿڣ�Viewport�����е����Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	win_width = width;
	win_height = height;
	projection[0].x = 8.0f / win_width + zoom / (win_width * 2.5f);
	projection[1].y = 8.0f / win_height + zoom / (win_height * 2.5f);
	shader_program->set_mat22("projection", projection);
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// �������ص�����
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad����OpenGL����ָ��
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		return -2;
	}

	// ���첢ʹ�õ���ɫ����Ƭ����ɫ��
	shader_program = new shader("src/Shader/Shader.v", "src/Shader/Shader.f");
	shader_program->use();
	shader_program->set_vec2("view", view);
	shader_program->set_mat22("projection", projection);

	// ��Ⱦ��ѭ��
	std::thread heracles_thread(heracles_run);
	while (!glfwWindowShouldClose(window)) {
		processInput(window);	// ��������
		display();				// ��ʾͼ��
		glfwPollEvents();		// �����¼�
	}

	// ɾ����������
	//glDeleteVertexArrays(1, &vao);
	//glDeleteBuffers(1, &vbo);
	//glDeleteBuffers(1, &ebo);

	// glfw�սᣬ�ͷ���Դ
	glfwTerminate();
	should_stop = true;
	heracles_thread.join();	// ��ֹreturnʱ���������̻߳�û������
	return 0;
}