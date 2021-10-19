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
GLfloat colorMixValue = 0.5;

// ��ͬ�Ķ����������ݴ��ͷ�ʽ
// ��Щ������Ҫ����չʾ�����÷� ʵ��ʹ���Ӿ����������
void prepareVertData_moreVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);
void prepareVertData_seqBatchVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);
void prepareVertData_interleavedBatchVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);
void prepareVertData_glBufferSubData(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);
void prepareVertData_glMapBuffer(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);
void prepareVertData_glCopyBufferSubData(GLuint& VAOId, std::vector<GLuint>& VBOIdVec);

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
		"Demo of vertex attribute data(press A S to mix color)", NULL, NULL);
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
	
	// section1 ׼��������������
	GLuint VAOId;
	std::vector<GLuint> VBOIdVec;
	prepareVertData_glCopyBufferSubData(VAOId, VBOIdVec); // �滻��������Թ۲�������ʽ
	// Section2 ׼����ɫ������
	Shader shader("triangle.vertex", "triangle.frag");

	// Section3 ׼���������
	GLint textureId = TextureHelper::load2DTexture("../../resources/textures/window.png");

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
		// ��������Ԫ ���������
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(glGetUniformLocation(shader.programId, "tex"), 0); // ��������ԪΪ0��
		glUniform1f(glGetUniformLocation(shader.programId, "colorMixValue"), colorMixValue);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &VAOId);
	for (std::vector<GLuint>::const_iterator it = VBOIdVec.begin();
		VBOIdVec.end() != it; ++ it)
	{
		glDeleteBuffers(1, &(*it));
	}
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
		colorMixValue += 0.05f;
		if (colorMixValue > 1.0f)
			colorMixValue = 1.0f;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		colorMixValue -= 0.05f;
		if (colorMixValue < 0.0f)
			colorMixValue = 0.0f;
	}
}
// ��������ÿ��������VBO
void prepareVertData_moreVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// �������Զ���
	GLfloat vertPos[] = {
		-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,  // ����λ��
	};
	GLfloat vertColor[] = {
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // ��ɫ
	};
	GLfloat vertTextCoord[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f     // ��������
	};
	// �����������
	GLuint VBOId[3];
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	glGenBuffers(3, VBOId);
	// ����λ������
	glBindBuffer(GL_ARRAY_BUFFER, VBOId[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos), vertPos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glBindBuffer(GL_ARRAY_BUFFER, VBOId[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertColor), vertColor, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	// ������������
	glBindBuffer(GL_ARRAY_BUFFER, VBOId[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertTextCoord), vertTextCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId[0]);
	VBOIdVec.push_back(VBOId[1]);
	VBOIdVec.push_back(VBOId[2]);
}
// �������Ե�������������VBO
void prepareVertData_seqBatchVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// ��������������Ϊ����
	GLfloat vertices[] = {
		-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,  // ����λ��
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // ��ɫ
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f     // ��������
	};
	// �����������
	GLuint VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ���� ��������
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step3: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // �������� strideҲ��������Ϊ 3 * sizeof(GL_FLOAT)
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(9 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(18 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId);
}

// ��������֮�佻�������VBO
void prepareVertData_interleavedBatchVBO(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// ����ָ��������������
	GLfloat vertices[] = {
		// ����λ�� ��ɫ ��������
		-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
		0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
	};
	// �����������
	GLuint VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ���� ��������
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Step3: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT),(GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 
		8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId);
}
// ʹ��glBufferSubData���VBO
void prepareVertData_glBufferSubData(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// �������Զ���
	GLfloat vertPos[] = {
		-0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,  // ����λ��
	};
	GLfloat vertColor[] = {
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // ��ɫ
	};
	GLfloat vertTextCoord[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f     // ��������
	};
	// �����������
	GLuint VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ���� Ԥ����ռ�
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos)+sizeof(vertColor) + sizeof(vertTextCoord), 
		NULL, GL_STATIC_DRAW);  // Ԥ����ռ�
	// Step3: �����������
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertPos), vertPos);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertPos), sizeof(vertColor), vertColor);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertPos)+ sizeof(vertColor), sizeof(vertTextCoord), vertTextCoord);
	// Step4: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // �������� strideҲ��������Ϊ 3 * sizeof(GL_FLOAT)
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(9 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(18 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId);
}

// ʹ��glMapBuffer
void prepareVertData_glMapBuffer(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// ����ָ��������������
	GLfloat vertices[] = {
		// ����λ�� ��ɫ ��������
		-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
		0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
	};
	// �����������
	GLuint VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ����
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	// Step3: ӳ������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW); // Ԥ����ռ�
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!ptr)
	{
		std::cerr << " map buffer failed!" << std::endl;
		return;
	}
	memcpy(ptr, vertices, sizeof(vertices));  // ���Բ������ָ��ָ����ڴ�ռ�
	glUnmapBuffer(GL_ARRAY_BUFFER);
	// Step4: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId);
}

// ʹ��glCopyBufferSubData 
void prepareVertData_glCopyBufferSubData(GLuint& VAOId, std::vector<GLuint>& VBOIdVec)
{
	// ����ָ��������������
	GLfloat vertices[] = {
		// ����λ�� ��ɫ ��������
		-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
		0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
	};
	// �����������
	GLuint VBOId;
	// Step1: ��������VAO����
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	// Step2: ��������VBO ����
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	GLuint TempBufferId;
	glGenBuffers(1, &TempBufferId);
	glBindBuffer(GL_COPY_WRITE_BUFFER, TempBufferId);
	// Step3: ӳ�����ݵ�GL_COPY_WRITE_BUFFER
	glBufferData(GL_COPY_WRITE_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW); // Ԥ����ռ�
	void* ptr = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_WRITE_ONLY);
	if (!ptr)
	{
		std::cerr << " map buffer failed!" << std::endl;
		return;
	}
	memcpy(ptr, vertices, sizeof(vertices));  // ���Բ������ָ��ָ����ڴ�ռ�
	glUnmapBuffer(GL_COPY_WRITE_BUFFER);
	// Step4: �����ݿ�����VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW); // ע�⿽��ǰԤ����ռ�
	glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(vertices));
	glDeleteBuffers(1, &TempBufferId);
	// Step5: ָ��������ʽ  �����ö�������
	// ����λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// ������������
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBOIdVec.push_back(VBOId);
}