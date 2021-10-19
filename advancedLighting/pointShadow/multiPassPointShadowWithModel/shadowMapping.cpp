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
#include "Shader.h"
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
const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 1.5f, 7.0f));
glm::vec3 lightPos(0.0f, 4.0f, 5.0f);
Model objModel;
GLuint CubeVAOId, CubeVBOId;
GLuint planeVAOId, planeVBOId;
GLuint quadVAOId, quadVBOId;
GLuint planeTextId;

void prepareVBO();
bool prepareDepthFBO(GLuint& depthCubeMapTextId, GLuint& fboId);
void renderScene(Shader &shader);
GLboolean bUseShadow = true;
GLboolean bUsePCF = false;

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
		"Demo of Shadow Mapping(Press B P to view effect)", NULL, NULL);
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
	prepareVBO();

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
	if (!objModel.loadModel(modelFilePath))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// Section2 ��������
	planeTextId = TextureHelper::load2DTexture("../../resources/textures/wood.png");

	// Section3 ׼����ɫ������
	Shader shader("scene.vertex", "scene.frag");
	Shader depthMapShader("depthMap.vertex", "depthMap.frag", "depthMap.gs"); // ���뼸����ɫ��

	// Section4 ׼�����ڻ���depth-map��FBO
	GLuint depthFBOId, depthCubeMapTextId;
	if (!prepareDepthFBO(depthCubeMapTextId, depthFBOId))
	{
		std::cout << "Error::FBO :" << " not complete." << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

		//lightPos.z = sin(glfwGetTime() * 0.5) * 3.0; // ��Դ��ʱ��䶯
		
		// ���ݹ�Դλ�� ��̬����任����Դ����ϵ�ľ���
		GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
		GLfloat near = 1.0f;
		GLfloat far = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); // ͸�Ӿ��󲻱�
		std::vector<glm::mat4> shadowTransforms;
		// ����ÿ�����ָ��ͬ����� ͸�Ӿ��� * �ӱ任����
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, 
			lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
			lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
			lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
			lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos,
			lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, 
			lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		// Pass1 �ӹ�Դ�ǶȻ��Ƴ�����depth cubemap
		// ע�����﷢��6���任���󼴿� ���ٵ�������6�����Ӧ�ĳ��� �������ü�����ɫ��һ�λ������
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBOId);
			glClear(GL_DEPTH_BUFFER_BIT);
			depthMapShader.use();
			for (GLuint i = 0; i < 6; ++i)
			{
				glUniformMatrix4fv(glGetUniformLocation(depthMapShader.programId,
					("shadowMatrices[" + std::to_string(i) + "]").c_str()),
					1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
			}
			glUniform1f(glGetUniformLocation(depthMapShader.programId, "far_plane"), far);
			glUniform3fv(glGetUniformLocation(depthMapShader.programId, "lightPos"), 1, &lightPos[0]);
			renderScene(depthMapShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Pass2 ����depth-map ��Ⱦ���ճ���
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.mouse_zoom),
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f); // ͶӰ����
		glm::mat4 view = camera.getViewMatrix(); // �ӱ任����
		// ���ù�Դ����
		GLint lightAmbientLoc = glGetUniformLocation(shader.programId, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(shader.programId, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(shader.programId, "light.specular");
		GLint lightPosLoc = glGetUniformLocation(shader.programId, "light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		// ���ù۲���λ��
		GLint viewPosLoc = glGetUniformLocation(shader.programId, "viewPos");
		glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);
		// ���ñ任����
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		// ���ñ任����Դ����ϵ�ľ���
		glUniform1f(glGetUniformLocation(shader.programId, "far_plane"), far);
		glUniform1i(glGetUniformLocation(shader.programId, "bUseShadow"), bUseShadow); // ���ʹ����boolѡ�� �ǵô��ݸ���ɫ��
		glUniform1i(glGetUniformLocation(shader.programId, "bUsePCF"), bUsePCF);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTextId); // ע���滻ԭ����GL_TEXTURE_2D
		glUniform1i(glGetUniformLocation(shader.programId, "depthCubeMap"), 4);
		renderScene(shader);
		glBindVertexArray(0);

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glfwTerminate();

	return 0;
}
void prepareVBO()
{
	// ָ��ƽ��Ķ����������� ����λ�� ����  ������ 
	GLfloat planeVertices[] = {
		25.0f, -0.5f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 25.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, 25.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		25.0f, -0.5f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		25.0f, -0.5f, -25.0f, 25.0f, 25.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 25.0f, 0.0f, 1.0f, 0.0f
	};

	// �������建�����
	// Step1: ��������VAO����
	glGenVertexArrays(1, &planeVAOId);
	glBindVertexArray(planeVAOId);
	// Step2: ��������VBO ���� ��������
	glGenBuffers(1, &planeVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
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

	// ָ�������嶥���������� ����λ�� ���� ������
	GLfloat cubeVertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// A
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// B
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// C
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// C
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// A


		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E
		-0.5f, 0.5f, -0.5f, 0.0, 1.0f, 0.0f, 0.0f, -1.0f, // H
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// F
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E

		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,	// D
		-0.5f, 0.5f, -0.5f, 1.0, 1.0f, -1.0f, 0.0f, 0.0f, // H
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// A
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,	// D

		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // F
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // G
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // C
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // C
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // B
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // F

		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G
		-0.5f, 0.5f, -0.5f, 0.0, 1.0f, 0.0f, 1.0f, 0.0f,    // H
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// C
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // A
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // E
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // F
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, // F
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // B
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // A
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// ָ������չʾdepth-map�ľ��ζ����������� λ�� ����
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
* ׼��ʹ��cubeMap��FBO
*/
bool prepareDepthFBO(GLuint& depthCubeMapTextId, GLuint& fboId)
{   
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glGenTextures(1, &depthCubeMapTextId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTextId);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // �ضϵ���Ե �������ظ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// ���ñ�Ե��ɫ
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubeMapTextId, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMapTextId, 0);
	glDrawBuffer(GL_NONE); // ֪ͨOpenGL���buffer��������дcolor buffer
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
void renderScene(Shader &shader)
{
	glUniform1i(glGetUniformLocation(shader.programId, "reverse_normals"), 0); // ������ʹ������ֵ
	// ���Ƶذ�
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTextId);
	glUniform1i(glGetUniformLocation(shader.programId, "texture_diffuse0"), 0);
	glUniform1i(glGetUniformLocation(shader.programId, "texture_specular0"), 0);
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(planeVAOId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 4.8f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	// ����һ��������
	glBindVertexArray(CubeVAOId);
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	// ����ģ��
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.3f)); 
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); 
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	objModel.draw(shader);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.6f, -0.05f, 1.3f));
	model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	objModel.draw(shader);

	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.2f, -0.05f, 1.5f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"), 1, GL_FALSE, glm::value_ptr(model));
	objModel.draw(shader);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	else if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		bUseShadow = !bUseShadow;
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		bUsePCF = !bUsePCF;
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