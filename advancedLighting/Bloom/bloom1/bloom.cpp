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

GLboolean bUseBloom = true;
GLfloat explosureFactor = 1.0f;

GLuint CubeVAOId, CubeVBOId;
GLuint quadVAOId, quadVBOId;
GLuint planeTextId, containerTextId;
void prepareVBO();

const int FBO_CNT = 2;
GLuint HDRTextIds[FBO_CNT], HDRFBOId;
GLuint BlurFBOIds[FBO_CNT];
GLuint BlurTextIds[FBO_CNT];

bool prepareHDRFBO(GLuint& FBOId, GLuint* TextIds, int textCnt);
bool prepareBlurFBO(GLuint* FBOIds, GLuint* TextIds, int cnt);

void renderPlane(Shader& shader);
void renderCubes(Shader& shader);

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
		"Demo of Bloom(Press up down space key)", NULL, NULL);
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
	if (!prepareHDRFBO(HDRFBOId, HDRTextIds, FBO_CNT))
	{
		std::cerr << "prepare hdr fbo failed." << std::endl;
		glfwTerminate();
		char waitKey;
		std::cin >> waitKey;
		return -1;
	}
	if (!prepareBlurFBO(BlurFBOIds, BlurTextIds, FBO_CNT))
	{
		std::cerr << "prepare hdr blur failed." << std::endl;
		glfwTerminate();
		char waitKey;
		std::cin >> waitKey;
		return -1;
	}
	// Section2 ��������
	planeTextId = TextureHelper::load2DTexture("../../resources/textures/wood.png", GL_SRGB); // ʹ��������ɫ�ռ������);
	containerTextId = TextureHelper::load2DTexture("../../resources/textures/container.jpg", GL_SRGB);
	// Section3 ׼����ɫ������
	Shader shader("scene.vertex", "scene.frag");
	Shader lightSourceShader("scene.vertex", "lightSource.frag");
	Shader blurShader("blur.vertex", "blur.frag");
	Shader bloomFinalShader("bloomFinal.vertex", "bloomFinal.frag");

	// Section4 ׼����Դ����
	struct LightAttr
	{
		LightAttr(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec)
		{
			position = pos;
			diffuse = diff;
			specular = spec;
		}
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	std::vector<LightAttr> seqLightAttr;
	seqLightAttr.push_back(LightAttr(glm::vec3(0.0f, 0.5f, 1.5f),
		glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(3.0f / 2.0f, 3.0f / 2.0f, 3.0f / 2.0f)));
	seqLightAttr.push_back(LightAttr(glm::vec3(-4.0f, 0.5f, -3.0f),
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(5.0f / 2.0f, 0.0f, 0.0f)));
	seqLightAttr.push_back(LightAttr(glm::vec3(3.0f, 0.5f, 1.0f),
		glm::vec3(0.0f, 2.5f, 0.0f),
		glm::vec3(0.0f, 2.5f / 2.0f, 0.0f)));
	seqLightAttr.push_back(LightAttr(glm::vec3(-0.8f, 2.4f, -1.0f),
		glm::vec3(0.0f, 0.0f, 15.0f),
		glm::vec3(0.0f, 0.0f, 15.0f / 2.0f)));

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
		// ��һ�� ���Ƴ�����HDR Frame buffer ���������color buffer
		glBindFramebuffer(GL_FRAMEBUFFER, HDRFBOId);
			// �����ɫ������ ����Ϊָ����ɫ
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.use();
			// ���ù�Դ����
			for (size_t i = 0; i < seqLightAttr.size(); ++i)
			{
				std::stringstream strStream;
				strStream << "lights[" << i << "]";
				std::string indexStr = strStream.str();
				glUniform3fv(glGetUniformLocation(shader.programId,
					(indexStr + ".diffuse").c_str()), 1, glm::value_ptr(seqLightAttr[i].diffuse));
				glUniform3fv(glGetUniformLocation(shader.programId,
					(indexStr + ".specular").c_str()), 1, glm::value_ptr(seqLightAttr[i].specular));
				glUniform3fv(glGetUniformLocation(shader.programId,
					(indexStr + ".position").c_str()), 1, glm::value_ptr(seqLightAttr[i].position));
			}
			// ���ù۲���λ��
			GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
			glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
			// ���ñ任����
			glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
				1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
				1, GL_FALSE, glm::value_ptr(view));
			
			// �Ȼ��Ƶذ�
			renderPlane(shader);
			// Ȼ����Ƴ�����������
			renderCubes(shader);
			// ������������ģ��Ĺ�Դ
			lightSourceShader.use();
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.programId, "projection"),
				1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.programId, "view"),
				1, GL_FALSE, glm::value_ptr(view));
			glBindVertexArray(CubeVAOId);
				for (GLuint i = 0; i < seqLightAttr.size(); i++)
				{
					model = glm::mat4();
					model = glm::translate(model, glm::vec3(seqLightAttr[i].position));
					model = glm::scale(model, glm::vec3(0.5f));
					glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
					glUniform3fv(glGetUniformLocation(lightSourceShader.programId, "lightColor"), 1, glm::value_ptr(seqLightAttr[i].diffuse));
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// �ڶ��� ȡHDR�и�������color buffer ʹ�ø�˹ģ�� �����blur fbo
		GLboolean horizontal = true, first_iteration = true;
		GLuint amount = 10;
		blurShader.use();
		for (GLuint i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, BlurFBOIds[horizontal]);
			glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(glGetUniformLocation(blurShader.programId, "horizontal"), horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? HDRTextIds[1] : BlurTextIds[!horizontal]);
			glBindVertexArray(quadVAOId);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		// ������ ���HDR FBO����ͨcolor buffer �͸�˹ģ������������color buffer �����Ĭ��FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		bloomFinalShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HDRTextIds[0]);
		glUniform1i(glGetUniformLocation(bloomFinalShader.programId, "sceneText"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, BlurTextIds[!horizontal]);
		glUniform1i(glGetUniformLocation(bloomFinalShader.programId, "blurText"), 1);
		glUniform1i(glGetUniformLocation(bloomFinalShader.programId, "bUseBloom"), bUseBloom);
		glUniform1f(glGetUniformLocation(bloomFinalShader.programId, "explosureFactor"), explosureFactor);
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
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		explosureFactor += 0.1f;
		std::cout << "explosure factor : " << explosureFactor << std::endl;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		explosureFactor -= 0.1f;
		std::cout << "explosure factor : " << explosureFactor << std::endl;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		bUseBloom = !bUseBloom;
		std::cout << "using bloom: " << (bUseBloom ? "true" : "false") << std::endl;
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
* ׼��һ��FBO�а������color buffer ����HDR �Լ���������
*/
bool prepareHDRFBO(GLuint& FBOId, GLuint* TextIds, int textCnt)
{
	glGenFramebuffers(1, &FBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOId);
	glGenTextures(textCnt, TextIds);
	// �� ���colorbuffer
	for (int i = 0; i < textCnt; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, TextIds[i]);
		// ����color buffer  ע������ĸ�ʽ��Ҫָ�� GL_RGB16F GL_RGBA16F �����ม���ʾ�Ĵ洢
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, TextIds[i], 0);
	}
	// ��depth buffer
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// Ҫ��ʽ��֪OpenGL������Ⱦ��color buffer
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// ��鹹���Ƿ�ɹ�
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
/*
* �����������ģ�������FBO
*/
bool prepareBlurFBO(GLuint* FBOIds, GLuint* TextIds, int cnt)
{
	glGenFramebuffers(cnt, FBOIds);
	glGenTextures(cnt, TextIds);
	for (int i = 0; i < cnt; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOIds[i]);
		glBindTexture(GL_TEXTURE_2D, TextIds[i]);
		// ����color buffer  ע������ĸ�ʽ��Ҫָ�� GL_RGB16F GL_RGBA16F �����ม���ʾ�Ĵ洢
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextIds[i], 0);
		// ���ﲻ��Ҫdepth buffer
		// ��鹹���Ƿ�ɹ�
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
void renderPlane(Shader& shader)
{
	glBindVertexArray(CubeVAOId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTextId);
	glUniform1i(glGetUniformLocation(shader.programId, "text"), 0);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(25.0f, 1.0f, 25.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
void renderCubes(Shader& shader)
{
	glBindVertexArray(CubeVAOId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, containerTextId);
	glUniform1i(glGetUniformLocation(shader.programId, "text"), 0);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
	model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(2.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
	model = glm::rotate(model, 23.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(2.5));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
	model = glm::rotate(model, 124.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(2.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}