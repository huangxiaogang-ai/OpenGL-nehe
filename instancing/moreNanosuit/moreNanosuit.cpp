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
// ����ģ�͵���
#include "model.h"

// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// �������ƶ�
void do_movement();

void prepareInstanceMatrices(std::vector<glm::mat4>& modelMatrices, 
	const int amount, const Model& instanceModel);

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
const int INSTANCE_COUNT = 441;  // ��д���Կ�ƽ��������
const GLfloat DELTA_ROW = 15.0f, DELTA_COL = 10.0f;
const int ROWS = (int)(sqrt(INSTANCE_COUNT)), COLS = (int)(sqrt(INSTANCE_COUNT));
const GLfloat X_LEN = DELTA_ROW * (ROWS - 1), Z_LEN = DELTA_COL * (COLS - 1); // ƽ��x z ���򳤶�
Camera camera(glm::vec3(X_LEN / 5.0f, 14.0f, Z_LEN / 2.0f + 25.0f));

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
		"Demo of instancing model", NULL, NULL);
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
	
	//Section1 ����ģ������ Ϊ�˷������ģ�� ���Ǵ��ļ���ȡģ���ļ�·��
	Model nanosuit;
	std::ifstream modelPath("modelPath.txt");
	if (!modelPath)
	{
		std::cerr << "Error::could not read model path file." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	std::string modelFilePath;
	std::getline(modelPath, modelFilePath);
	if (modelFilePath.empty())
	{
		std::cerr << "Error::model path empty." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	if (!nanosuit.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// section2 ָ�����涥�����ݺͼ�������
	// �ذ嶥���������� ����λ�� ��������(���õ�ֵ����1.0�����ظ�)
	GLfloat planeVertices[] = {
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
		5.0f, -0.5f, -5.0f, 2.0f, 2.0f,  // D
		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C

		-5.0f, -0.5f, -5.0f, 0.0f, 2.0f, // C
		-5.0f, -0.5f, 5.0f, 0.0f, 0.0f,  // B
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f,   // A
	};
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
	GLuint planeTextId = TextureHelper::load2DTexture("../../resources/textures/metal.png");
	// Section2 ׼����ɫ������
	Shader shader("plane.vertex", "plane.frag");
	Shader instanceShader("instance.vertex", "instance.frag");

	// Section3 Ϊ���ʵ��׼��ģ�ͱ任����
	std::vector<glm::mat4> modelMatrices;
	prepareInstanceMatrices(modelMatrices, INSTANCE_COUNT, nanosuit);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		std::cout << " time used in this frame: " << deltaTime << " seconds." << std::endl;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		// �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0f, 1000.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		// ������д�������ƴ���
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glBindVertexArray(planeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTextId);
		glm::mat4 model = glm::mat4();
		model = glm::scale(model, glm::vec3(X_LEN / 5.0f, 1.0, Z_LEN / 5.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// ���ƶ������ս������
		instanceShader.use();
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(instanceShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		const std::vector<Mesh>& meshes = nanosuit.getMeshes();
		for (std::vector<Mesh>::size_type i = 0; i < meshes.size(); ++i)
		{
			glBindVertexArray(meshes[i].getVAOId());
			const int texUnitCnt = meshes[i].bindTextures(instanceShader);
			glDrawElementsInstanced(GL_TRIANGLES, 
				meshes[i].getIndices().size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
			glBindVertexArray(0);
			meshes[i].unBindTextures(texUnitCnt);
		}

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
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
// Ϊʵ��׼��ģ�ͱ任����
// �����ϸ�ڿ��Բ����
void prepareInstanceMatrices(std::vector<glm::mat4>& modelMatrices, 
	const int amount, const Model& instanceModel)
{
	for (GLint row = -ROWS / 2; row <= ROWS / 2; ++row)
	{
		for (GLint col = - COLS / 2; col <= COLS / 2; ++col)
		{ 
			glm::vec3 translation;
			translation.x = -(X_LEN / 2.0) + (col + COLS / 2) * DELTA_ROW;
			translation.y = -0.75f;
			translation.z = -(Z_LEN / 2.0) + (row + ROWS / 2) * DELTA_COL;
			glm::mat4 model;
			model = glm::translate(model, translation);
			modelMatrices.push_back(model);
		}
	}
	// ����instance array
	GLuint modelMatricesVBOId;
	glGenBuffers(1, &modelMatricesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices.size(), 
		&modelMatrices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Ϊģ����ÿ��mesh ����model matrix
	// ��4��vec4�������mat4����
	const std::vector<Mesh>& meshes = instanceModel.getMeshes();
	for (std::vector<Mesh>::size_type i = 0; i < meshes.size(); ++i)
	{
		glBindVertexArray(meshes[i].getVAOId());
		glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBOId);
		// ��һ��
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 
			4 * sizeof(glm::vec4), (GLvoid*)0);
		// �ڶ���
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 
			4 * sizeof(glm::vec4), (GLvoid*)(sizeof(glm::vec4)));
		// ������
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 
			4 * sizeof(glm::vec4), (GLvoid*)(2 * sizeof(glm::vec4)));
		// ������
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE,
			4 * sizeof(glm::vec4), (GLvoid*)(3 * sizeof(glm::vec4)));

		// ע��������Ҫ����ʵ�����ݸ���ѡ�� ָ��1��ʾ ÿ��ʵ������һ��
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}