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
#include <cstdlib>
// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"
// �������������
#include "texture.h"

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
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
		"Demo of stencil testing", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
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
		std::system("pause");
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	//Section1 ������������
	// ָ�������嶥���������� ����λ�� ��ɫ ����
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// A
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,	// B
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,1.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A


		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		-0.5f, -0.5f, -0.5f,0.0f, 0.0f,	// E

		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D
		-0.5f, 0.5f, -0.5f,1.0, 1.0f,   // H
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// D

		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f,0.0f, 1.0f,	// C
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,	// B
		0.5f, -0.5f, -0.5f,1.0f, 0.0f,	// F

		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G
		-0.5f, 0.5f, -0.5f,0.0, 1.0f,   // H
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		-0.5f, 0.5f, 0.5f,0.0f, 0.0f,	// D
		0.5f, 0.5f, 0.5f,1.0f, 0.0f,	// C
		0.5f, 0.5f, -0.5f,1.0f, 1.0f,	// G

		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,// E
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, -0.5f,1.0f, 1.0f,	// F
		0.5f, -0.5f, 0.5f,1.0f, 0.0f,	// B
		-0.5f, -0.5f, 0.5f,0.0f, 0.0f,	// A
	};
	// �ذ嶥���������� ����λ�� ��������(���õ�ֵ����1.0�����ظ�)
	GLfloat planeVertices[] = {
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
		5.0f, -0.5f, -5.0f, 2.0f, 2.0f,  // D
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C

		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C
		-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,  // B
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
	};
	// ģ�����
	GLfloat stencilVertices[] = {
		-0.5f, -0.5f, 0.5f, // A
		-0.5f, 0.5f, 0.5f,	// B
		-1.5f, 0.5f, 0.5f,  // C

		-1.5f, 0.5f, 0.5f,  // C
		-1.5f, -0.5f, 0.5f, // D
		-0.5f, -0.5f, 0.5f  // A
	};
	// Section2 ׼���������
	GLuint cubeVAOId, cubeVBOId;
	glGenVertexArrays(1, &cubeVAOId);
	glGenBuffers(1, &cubeVBOId);
	glBindVertexArray(cubeVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������������
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 
		5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLuint planeVAOId, planeVBOId;
	glGenVertexArrays(1, &planeVAOId);
	glGenBuffers(1, &planeVBOId);
	glBindVertexArray(planeVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������������
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLuint stencilVAOId, stencilVBOId;
	glGenVertexArrays(1, &stencilVAOId);
	glGenBuffers(1, &stencilVBOId);
	glBindVertexArray(stencilVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, stencilVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stencilVertices), stencilVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Section3 ��������
	GLuint cubeTextId = TextureHelper::load2DTexture("../../resources/textures/marble.jpg");
	GLuint planeTextId = TextureHelper::load2DTexture("../../resources/textures/metal.png");
	// Section4 ׼����ɫ������
	Shader shader("stencilTest.vertex", "stencilTest.frag");
	Shader stencilShader("singleColor.vertex", "singleColor.frag");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		// �����ɫ������ ��Ȼ����� ģ�建����
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// ����ģ��
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // ��ģ����Ժ���Ȳ��Զ�ͨ��ʱ����ģ�建����

		stencilShader.use();
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 100.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		glUniformMatrix4fv(glGetUniformLocation(stencilShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(stencilShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(stencilShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(stencilVAOId);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// ����ʵ�ʳ���
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilMask(0x00); // ��ֹд��stencil
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(cubeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTextId);
		// ���Ƶ�һ��������
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36); // �����36�����ڹ��������εĶ������ �����������εĸ���12
		// ���Ƶڶ���������
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// ����ƽ��
		glBindVertexArray(planeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTextId);
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glStencilMask(0xFF);  // �ٴ�����д��ģ�建���� �Ա��´ε���ʱ���
		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteVertexArrays(1, &planeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
	glDeleteBuffers(1, &planeVBOId);
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