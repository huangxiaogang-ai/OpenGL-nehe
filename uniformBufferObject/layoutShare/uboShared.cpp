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
		"Demo of Using UBO in shader(layout=shared)", NULL, NULL);
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
	// ָ�������嶥���������� ����λ�� ������
	GLfloat cubeVertices[] = {
		// ���� ADC CBA
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		// ����EFG GHE
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		// ����� HDA AEH
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		// �Ҳ��� GFB BCG
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		
		// ���� ABF FEA
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		// ���� DHG GCD
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
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
		6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ���㷨��������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
		6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Section3 ׼����ɫ������
	Shader redShader("ubo_vs.vertex", "ubo_red.frag");
	Shader greenShader("ubo_vs.vertex", "ubo_green.frag");
	Shader blueShader("ubo_vs.vertex", "ubo_blue.frag");
	Shader yellowShader("ubo_vs.vertex", "ubo_yellow.frag");

	// Section4 ����std140 layout UBO ����shader�� uniform buffer�ͳ�����UBO��Ӧ����
	// step1 ��ȡshader�� uniform buffer ������
	GLuint redShaderIndex = glGetUniformBlockIndex(redShader.programId, "Matrices");
	GLuint greeShaderIndex = glGetUniformBlockIndex(greenShader.programId, "Matrices");
	GLuint blueShaderIndex = glGetUniformBlockIndex(blueShader.programId, "Matrices");
	GLuint yellowShaderIndex = glGetUniformBlockIndex(yellowShader.programId, "Matrices");
	// step2 ����shader�� uniform buffer ��������ָ���󶨵�
	glUniformBlockBinding(redShader.programId, redShaderIndex, 0); // �󶨵�Ϊ0
	glUniformBlockBinding(greenShader.programId, greeShaderIndex, 0);
	glUniformBlockBinding(blueShader.programId, blueShaderIndex, 0);
	glUniformBlockBinding(yellowShader.programId, yellowShaderIndex, 0);
	// step3 ����UBO �����ӵ�ָ���󶨵�
	GLuint UBOId;
	glGenBuffers(1, &UBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOId);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW); // Ԥ����ռ�
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBOId, 0, 2 * sizeof(glm::mat4)); // �󶨵�Ϊ0
	// step4 ֻ����һ����ֵ ͶӰ���� ����ͶӰ���󲻱�
	glBindBuffer(GL_UNIFORM_BUFFER, UBOId);
	glm::mat4 projection = glm::perspective(45.0f,
		(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f); // ͶӰ����
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Section5 �����һ�� share layout ��UBO
	redShaderIndex = glGetUniformBlockIndex(redShader.programId, "mixColorSettings");
	greeShaderIndex = glGetUniformBlockIndex(greenShader.programId, "mixColorSettings");
	blueShaderIndex = glGetUniformBlockIndex(blueShader.programId, "mixColorSettings");
	yellowShaderIndex = glGetUniformBlockIndex(yellowShader.programId, "mixColorSettings");
	// step1 ����shader�� uniform buffer ��������ָ���󶨵�
	glUniformBlockBinding(redShader.programId, redShaderIndex, 1); // �󶨵�Ϊ1
	glUniformBlockBinding(greenShader.programId, greeShaderIndex, 1);
	glUniformBlockBinding(blueShader.programId, blueShaderIndex, 1);
	glUniformBlockBinding(yellowShader.programId, yellowShaderIndex, 1);
	// step2 ����UBO �����ӵ�ָ���󶨵�
	GLuint colorUBOId;
	glGenBuffers(1, &colorUBOId);
	glBindBuffer(GL_UNIFORM_BUFFER, colorUBOId);
	// ��ȡUBO��С ��Ϊ������ͬ ֻ��Ҫ��һ��shader�л�ȡ��С����
	GLint blockSize;
	glGetActiveUniformBlockiv(redShader.programId, redShaderIndex,
		GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	if (blockSize <= 0)
	{
		std::cerr << "Error::glGetActiveUniformBlockiv, could not get ubo block size." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	glBufferData(GL_UNIFORM_BUFFER, blockSize, NULL, GL_DYNAMIC_DRAW); // Ԥ����ռ�
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, colorUBOId); // �󶨵�Ϊ1
	// step3 ��������
	// ͨ����ѯ��ȡuniform buffer�и���������������λ��ƫ��
	const GLchar* names[] = {
		"anotherColor", "mixValue"
	};
	GLuint indices[2];
	glGetUniformIndices(redShader.programId, 2, names, indices);
	GLint offset[2];
	glGetActiveUniformsiv(redShader.programId, 2, indices, GL_UNIFORM_OFFSET, offset);
	for (int i = 0; i < 2; ++i)
	{
		std::cout << "Get uniform buffer variable names: " << names[i] 
			<< " offset: " << offset[i] << std::endl;
	}
	// ʹ�û�ȡ��λ��ƫ����������
	glm::vec4 anotherColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
	GLfloat mixValue = 0.5f;
	glBindBuffer(GL_UNIFORM_BUFFER, colorUBOId);
	glBufferSubData(GL_UNIFORM_BUFFER, offset[0], sizeof(glm::vec4), glm::value_ptr(anotherColor));
	glBufferSubData(GL_UNIFORM_BUFFER, offset[1], sizeof(glm::vec4), &mixValue);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		// ����colorBuffer��ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		// ���colorBuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ���²���UBO����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		glBindBuffer(GL_UNIFORM_BUFFER, UBOId);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), 
			sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// ����4��shader����4��������
		glBindVertexArray(cubeVAOId);
		// ���ƺ�ɫ
		redShader.use();
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // ���Ͻ�
		glUniformMatrix4fv(glGetUniformLocation(redShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// ������ɫ
		greenShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // ���Ͻ�
		glUniformMatrix4fv(glGetUniformLocation(greenShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// ���ƻ�ɫ
		yellowShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // ���½�
		
		glUniformMatrix4fv(glGetUniformLocation(yellowShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// ������ɫ
		blueShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // ���½�
		glUniformMatrix4fv(glGetUniformLocation(blueShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		
		glBindVertexArray(0);
		glUseProgram(0);
		glDepthFunc(GL_LESS);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
	glDeleteBuffers(1, &UBOId);
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