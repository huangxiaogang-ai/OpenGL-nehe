// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
// ����SOIL��
#include <SOIL/SOIL.h>

#include <iostream>
#include <vector>

// ������ɫ�����ؿ�
#include "shader.h"
// ����������ظ�����
#include "texture.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
GLfloat mixValue = 0.4f; // �����ϲ���

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
		"Demo of mixing 2D texture(press A and S to adjust)", NULL, NULL);
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
	// ָ�������������� ����λ�� ��ɫ ����
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 3
	};
	GLushort indices[] = {
		0, 1, 2,  // ��һ��������
		0, 2, 3   // �ڶ���������
	};
	// �����������
	GLuint VAOId, VBOId, EBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ���� ��������
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step3: ��������EBO ���� ��������
	glGenBuffers(1, &EBOId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Step4: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // ע�ⲻҪ���EBO��
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Section2 ׼����ɫ������
	Shader shader("triangle.vertex", "triangle.frag");

	// Section3 ׼���������
	GLint textureId1 = TextureHelper::load2DTexture("../../resources/textures/wood.png");
	GLint textureId2 = TextureHelper::load2DTexture("../../resources/textures/cat.png");

	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // ����������� ���̵��¼�

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ������д�������ƴ���
		glBindVertexArray(VAOId);
		shader.use();
		// ���ö������Ԫ ���������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId1);
		glUniform1i(glGetUniformLocation(shader.programId, "tex1"), 0); // ��������ԪΪ0��
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureId2);
		glUniform1i(glGetUniformLocation(shader.programId, "tex2"), 1); // ��������ԪΪ1��

		glUniform1f(glGetUniformLocation(shader.programId, "mixValue"),
			mixValue); // ���������ϲ���
		// ʹ����������
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		mixValue += 0.05f;
		if (mixValue > 1.0f)
			mixValue = 1.0f;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		mixValue -= 0.05f;
		if (mixValue < 0.0f)
			mixValue = 0.0f;
	}

}