// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
// ����SOIL��
#include <SOIL/SOIL.h>
// ����GLM��
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// �������ƶ�
void do_movement();

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
glm::vec3 lampPos(0.8f, 0.8f, 0.5f);

int main(int argc, char** argv)
{
	
	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of Basic lighting", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);
	// ע������¼��ص�����
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// ע���������¼��ص�����
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// ��겶�� ͣ���ڳ�����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) 
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	// Section1 ׼����������
	// ָ�������������� ����λ�� ���� ������
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,1.0f,	// A
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	// B
		0.5f, 0.5f, 0.5f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// A
		

		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E
		-0.5f, 0.5f, -0.5f,  0.0, 1.0f,  0.0f, 0.0f, -1.0f, // H
		0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		0.5f, 0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// F
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E

		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,	// D
		-0.5f, 0.5f, -0.5f, 1.0, 1.0f,   -1.0f, 0.0f, 0.0f, // H
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,	// D

		0.5f, -0.5f, -0.5f,1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // F
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // G
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // C
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,   1.0f, 0.0f, 0.0f, // C
		0.5f, -0.5f, 0.5f,0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // B
		0.5f, -0.5f, -0.5f,1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // F

		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G
		-0.5f, 0.5f, -0.5f, 0.0, 1.0f, 0.0f, 1.0f, 0.0f,    // H
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		0.5f, 0.5f, 0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,	// C
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G

		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  // A
		-0.5f, -0.5f, -0.5f,0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // E
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // F
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // F
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f, // B
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // A
	};

	// �������建�����
	GLuint VAOId, VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ���� ��������
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step3: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������������
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ���㷨��������
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ������Դ��VAO
	GLuint lampVAOId;
	glGenVertexArrays(1, &lampVAOId);
	glBindVertexArray(lampVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId); // ������������� �����ظ����Ͷ�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0); // ֻ��Ҫ����λ�ü���
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Section2 ׼����ɫ������
	Shader shader("cube.vertex", "cube.frag");
	Shader lampShaer("lamp.vertex", "lamp.frag");

	glEnable(GL_DEPTH_TEST);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		shader.use();
		// ���ù�Դ���Ժ�������ɫ����
		GLint objectColorLoc = glGetUniformLocation(shader.programId, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shader.programId, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shader.programId, "lightPos");
		// ���������ϵ�������ʱ viewPosʼ��λ��ԭ��
		//GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		// ��Դ��ʱ��䶯
		GLfloat radius = 2.0f;
		lampPos.x = radius * cos(glfwGetTime());
		lampPos.z = radius * sin(glfwGetTime());
		glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
		//glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		// ���ñ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		// ����������
		glm::mat4 model;
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ���ƹ�Դ �����������
		glBindVertexArray(lampVAOId);
		lampShaer.use();
		glUniformMatrix4fv(glGetUniformLocation(lampShaer.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lampShaer.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		model = glm::translate(model, lampPos);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lampShaer.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
	glDeleteVertexArrays(1, &lampVAOId);
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // �״�����ƶ�
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false; 
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}
// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}
// ����������ദ����̿���
void do_movement()
{
	
	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
}