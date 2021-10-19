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
#include <sstream>

// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"
// ����������ظ�����
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

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

GLint drawMode = 1;

GLuint CubeVAOId, CubeVBOId;
GLuint quadVAOId, quadVBOId;
void prepareVBO();

struct SGBuffer
{
	GLuint FBOId;		// �洢FBO
	GLuint gPosTextId;	// �洢λ����Ϣ
	GLuint gNormalTextId; // �洢��������Ϣ
	GLuint gAlbedoSpecTextId;// �洢��������ɫ �;����ǿ��
};
SGBuffer gBuffer;

void renderPlane(Shader& shader);
void renderCubes(Shader& shader);
bool prepareGBuffer(SGBuffer& gBuffer);

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
		"Demo of Deferred shading(Press 1-5 to view)", NULL, NULL);
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

	// Section1 ׼���������ݺ�FBO
	prepareVBO();
	
	if (!prepareGBuffer(gBuffer))
	{
		std::cerr << "prepare G-buffer failed." << std::endl;
		glfwTerminate();
		char waitKey;
		std::cin >> waitKey;
		return -1;
	}

	// Section2 ����ģ��
	// Ϊ�˷������ģ�� ���Ǵ��ļ���ȡģ���ļ�·��
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
	Model objModel;
	if (!objModel.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	
	//Model objModel(modelFilePath.c_str());
	std::vector<glm::vec3> objPositions; // ģ��λ��
	objPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

	// Section3 ׼����ɫ������
	Shader gBufferShader("gbuffer.vertex", "gbuffer.frag");
	Shader sceneShader("scene.vertex", "scene.frag");

	// Section4 ׼����Դ����
	struct LightAttr
	{
		LightAttr(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec)
		{
			position = pos;
			diffuse = diff;
			specular = spec;
		}
		LightAttr(){}
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float linear;   // һ��˥��ϵ��
		float quadratic; // ����˥��ϵ��
	};
	srand(13);
	std::vector<LightAttr> seqLightAttr;
	const GLuint NR_LIGHTS = 32;
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		LightAttr lightAttr;
		// λ�����һ��ƫ����
		GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightAttr.position = glm::vec3(xPos, yPos, zPos);
		// ���һ��[0.5, 1.0)��ɫ
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; 
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5;
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5;
		lightAttr.diffuse = glm::vec3(rColor, gColor, bColor);
		lightAttr.specular = glm::vec3(rColor / 5.0f , gColor / 5.0f, bColor / 5.0f);
		lightAttr.linear = 0.7;
		lightAttr.quadratic = 1.8;
		seqLightAttr.push_back(lightAttr);
	}

	glEnable(GL_DEPTH_TEST);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������
		
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����

		// ������д�������ƴ���
		glm::mat4 model = glm::mat4();
		// ��һ�� ���Ƴ������������嵽GBuffer geometry-pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBOId);
			// �����ɫ������ ����Ϊָ����ɫ
			// ע��������Ҫ����alpha�ɷ�Ϊ0.0 ��Ϊ��Ҫ�õ�alpha�ɷ���Ϊ�����ǿ��
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gBufferShader.use();
			// ���ñ任����
			glUniformMatrix4fv(glGetUniformLocation(gBufferShader.programId, "projection"),
				1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(gBufferShader.programId, "view"),
				1, GL_FALSE, glm::value_ptr(view));
			// ��Ⱦ���ģ��
			for (size_t i = 0; i < objPositions.size(); ++i)
			{
				glm::mat4 model = glm::mat4();
				model = glm::translate(model, objPositions[i]);
				model = glm::scale(model, glm::vec3(0.25f));
				glUniformMatrix4fv(glGetUniformLocation(gBufferShader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
				objModel.draw(gBufferShader);
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// �ڶ�������GBuffer ��Ⱦ���� lighting-pass
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sceneShader.use();
		// ���ù�Դ����
		for (size_t i = 0; i < seqLightAttr.size(); ++i)
		{
			std::stringstream strStream;
			strStream << "lights[" << i << "]";
			std::string indexStr = strStream.str();
			
			glUniform3fv(glGetUniformLocation(sceneShader.programId,
				(indexStr + ".diffuse").c_str()), 1, glm::value_ptr(seqLightAttr[i].diffuse));
			glUniform3fv(glGetUniformLocation(sceneShader.programId,
				(indexStr + ".specular").c_str()), 1, glm::value_ptr(seqLightAttr[i].specular));
			glUniform3fv(glGetUniformLocation(sceneShader.programId,
				(indexStr + ".position").c_str()), 1, glm::value_ptr(seqLightAttr[i].position));
			glUniform1f(glGetUniformLocation(sceneShader.programId,
				(indexStr + ".linear").c_str()), seqLightAttr[i].linear);
			glUniform1f(glGetUniformLocation(sceneShader.programId,
				(indexStr + ".quadratic").c_str()), seqLightAttr[i].quadratic);
		}
		// ���ù۲���λ��
		GLint viewPosLoc = glGetUniformLocation(sceneShader.programId, "viewPos");
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		// ������ȾЧ��ģʽ
		glUniform1i(glGetUniformLocation(sceneShader.programId, "drawMode"), drawMode);
		// ��Ⱦ��һ��������
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer.gPosTextId);
		glUniform1i(glGetUniformLocation(sceneShader.programId, "gPostionText"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer.gNormalTextId);
		glUniform1i(glGetUniformLocation(sceneShader.programId, "gNormalText"), 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpecTextId);
		glUniform1i(glGetUniformLocation(sceneShader.programId, "gAlbedoSpecText"), 2);
		glBindVertexArray(quadVAOId);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &quadVAOId);
	glDeleteBuffers(1, &quadVBOId);
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
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		drawMode = 1;
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		drawMode = 2;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		drawMode = 3;
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		drawMode = 4;
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		drawMode = 5;
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

void prepareVBO()
{
	// ָ�������嶥���������� ����λ�� ������ ���� 
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// B
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	// A


		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0, 1.0f, // H
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// G
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// G
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// F
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0, 1.0f,   // H
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// D

		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // F
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // G
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // C
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // C
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// B
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// F

		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // G
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0, 1.0f, // H
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // D
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // D
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // C
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // G

		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // A
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // E
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // F
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // F
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // B
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // A
	};
	// �������建�����
	glGenVertexArrays(1, &CubeVAOId);
	glBindVertexArray(CubeVAOId);
	glGenBuffers(1, &CubeVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ָ������չʾ�ľ��ζ����������� λ�� ����
	GLfloat quadVertices[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	glGenVertexArrays(1, &quadVAOId);
	glGenBuffers(1, &quadVBOId);
	glBindVertexArray(quadVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
}
/*
* ����G-buffer
* �����洢λ�õ� RGB�ɷ�
* �洢�������RGB �;����ǿ�ȵ�alpha�ɷ�
* �洢��������RGB
*/
bool prepareGBuffer(SGBuffer& gBuffer)
{
	glGenFramebuffers(1, &gBuffer.FBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBOId);
	// λ����Ϣ
	glGenTextures(1, &gBuffer.gPosTextId);
	glBindTexture(GL_TEXTURE_2D, gBuffer.gPosTextId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.gPosTextId, 0);
	// ��������Ϣ
	glGenTextures(1, &gBuffer.gNormalTextId);
	glBindTexture(GL_TEXTURE_2D, gBuffer.gNormalTextId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.gNormalTextId, 0);
	// ��������Լ������ǿ��
	glGenTextures(1, &gBuffer.gAlbedoSpecTextId);
	glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpecTextId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.gAlbedoSpecTextId, 0);
	// MRT-��֪OPenGL����Ⱦ�����Ŀ�껺���� 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// ������rbo��
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - ���FBO������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}