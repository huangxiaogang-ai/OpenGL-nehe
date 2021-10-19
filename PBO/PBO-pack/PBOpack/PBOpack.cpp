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
#include <sstream>
#include <vector>

// ������ɫ�����ؿ�
#include "shader.h"
// ����������Ƹ�����
#include "camera.h"
// ����������ظ�����
#include "texture.h"
// ��ʱ��������
#include "Timer.h"
// �������������
#include "font.h"

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
glm::vec3 lampPos(0.5f, 0.5f, 0.3f);

GLuint quadVAOId, quadVBOId;
GLuint textVAOId, textVBOId;
GLuint cubeTextId, pixelTextId;
GLuint PBOIds[2];
void setupQuadVAO();
void preparePBO();
void initPixelText();
void drawRightSidePixel(Shader& shader);
void drawLeftSidePixel(Shader& shader);
// ͼƬ����
const int    SINGLE_SCREEN_WIDTH = WINDOW_WIDTH / 2;
const int    SINGLE_SCREEN_HEIGHT = WINDOW_HEIGHT;
const int	 DATA_SIZE = SINGLE_SCREEN_WIDTH * SINGLE_SCREEN_HEIGHT * 4;
const int    TEXT_WIDTH = 8;
const int    TEXT_HEIGHT = 30;
const GLenum PIXEL_FORMAT = GL_BGRA;
void updatePixels(unsigned char* src, int width, int height, int shift, unsigned char* dst);
GLubyte* colorBuffer = 0; // ��������
void initColorBuffer();
void releaseColorBuffer();

bool pboUsed = false;
Timer timer, t1;
float readTime, processTime;

void renderInfo(Shader& shader);
bool isFirstRendered = false;
void renderInitScene(Shader& shader);
void renderScene(Shader& shader);
void renderText(Shader &shader, std::wstring text,
	GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void printTransferRate();
std::map<wchar_t, FontCharacter> unicodeCharacters;

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
		"Demo of PBO(asynchronous DMA transfer)", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 300, 100);
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
	setupQuadVAO();
	preparePBO();
	initColorBuffer();

	// Section2 ��������
	FontResourceManager::getInstance().loadFont("arial", "../../resources/fonts/arial.ttf");
	FontResourceManager::getInstance().loadASCIIChar("arial", 38, unicodeCharacters);

	// Section3 ��������
	cubeTextId = TextureHelper::load2DTexture("../../resources/textures/wood.png");
	initPixelText();

	// Section4 ׼����ɫ������
	Shader shader("scene.vertex", "scene.frag");
	Shader textShader("text.vertex", "text.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		// ������д�������ƴ���
		// �Ȼ�������ͼƬ
		shader.use();
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		if (!isFirstRendered)
		{
			renderInitScene(shader);
			isFirstRendered = true;
		}
		else
		{
			renderScene(shader);
		}
		// ���Ҳ������Ϣ���� ����������ֻḴ�Ƶ��Ҳ�
		textShader.use();
		projection = glm::ortho(0.0f, (GLfloat)(SINGLE_SCREEN_WIDTH),
		0.0f, (GLfloat)SINGLE_SCREEN_HEIGHT);
		view = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(textShader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(textShader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		glUniformMatrix4fv(glGetUniformLocation(textShader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		renderInfo(textShader);
		printTransferRate();

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glDeleteVertexArrays(1, &quadVAOId);
	glDeleteBuffers(1, &quadVBOId);
	glDeleteVertexArrays(1, &textVAOId);
	glDeleteBuffers(1, &textVBOId);
	glDeleteBuffers(2, PBOIds);  // ע���ͷ�PBO
	releaseColorBuffer();

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
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		pboUsed = !pboUsed;
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

// ׼��PBO
void preparePBO()
{
	glGenBuffers(2, PBOIds);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[0]);
	//  GL_STREAM_DRAW���ڴ������ݵ�texture object GL_STREAM_READ ���ڶ�ȡFBO������
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[1]);
	glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

// �ı����ص�����
void updatePixels(unsigned char* src, int width, int height, int shift, unsigned char* dst)
{
	if (!src || !dst)
		return;

	int value;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			++src;    // ����alpha
			++dst;
		}
	}
}

// �������ε�VAO����
void setupQuadVAO()
{
	// ������������ λ�� ����
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

	glGenVertexArrays(1, &textVAOId);
	glGenBuffers(1, &textVBOId);
	glBindVertexArray(textVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, textVBOId);
	// �������ֵľ��ζ����������� λ�� ���� �Ƕ�̬���������
	// ����Ԥ����ռ�
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	// xy ��ʾλ�� zw��ʾ��������
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}

// ����һ����ʼ���� ��֤��FBO��ʹ��glReadPixelsʱ������
void renderInitScene(Shader& shader)
{
	// �������������
	drawLeftSidePixel(shader);
	// �����ұ�������
	drawRightSidePixel(shader);
}
void drawLeftSidePixel(Shader& shader)
{
	glViewport(0, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTextId);
	glBindVertexArray(quadVAOId);
	glUniform1i(glGetUniformLocation(shader.programId, "cubeText"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void drawRightSidePixel(Shader& shader)
{
	glViewport(SINGLE_SCREEN_WIDTH, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT);
	// ʹ����ɫ��ʱ �������ַ�ʽ��Ч ʹ��һ���������
	/*
	glDrawBuffer(GL_BACK);
	glRasterPos2i(0, 0);
	glDrawPixels(SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, colorBuffer);
	*/
	glBindVertexArray(quadVAOId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pixelTextId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)colorBuffer);
	glUniform1i(glGetUniformLocation(shader.programId, "cubeText"), 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void renderScene(Shader& shader)
{
	static int shift = 0;
	static int index = 0;				// ���ڴ�Ĭ��FBO��ȡ��PBO
	int nextIndex = 0;                  // ���ڴ�PBO�������� ���Ƶ�Ĭ��FBO
	shift = ++shift % 200;
	glReadBuffer(GL_FRONT); // ���ö�ȡ��FBO
	if (pboUsed)
	{
		index = (index + 1) % 2;
		nextIndex = (index + 1) % 2;
		// ��ʼFBO��PBO���Ʋ��� pack����
		t1.start();
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[index]);
		// OpenGLִ���첽��DMA���� �������������Ż� ��ʱCPU����ִ����������
		glReadPixels(0, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

		// �����ȡ��������ʱ��
		t1.stop();
		readTime = t1.getElapsedTimeInMilliSec();


		// ��ʼ�޸�nextIndexָ���PBO������
		t1.start();

		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[nextIndex]);
		glBufferData(GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
		// ��PBOӳ�䵽�û��ڴ�ռ� Ȼ���޸�PBO������
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr)
		{
			// ����ӳ�����ڴ�����
			updatePixels(ptr, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, shift, colorBuffer);
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER); // �ͷ�ӳ���client memory
		}

		// �������PBO��������ʱ��
		t1.stop();
		processTime = t1.getElapsedTimeInMilliSec();
		// ע�� ��Ҫ��дΪglBindBuf(GL_PIXEL_UNPACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}
	else
	{
		// ��ʹ��PBO�ķ�ʽ ��ȡ��client memory���޸�
		t1.start();
		glReadPixels(0, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, colorBuffer);
		t1.stop();
		readTime = t1.getElapsedTimeInMilliSec();

		// �޸��ڴ�����
		t1.start();
		updatePixels(colorBuffer, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, shift, colorBuffer);
		t1.stop();
		processTime = t1.getElapsedTimeInMilliSec();
	}
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawLeftSidePixel(shader);
	drawRightSidePixel(shader);
}

void renderInfo(Shader& shader)
{
	float color[4] = { 1, 1, 1, 1 };

	std::wstringstream ss;
	ss << "PBO: ";
	if (pboUsed)
		ss << "on" << std::ends;
	else
		ss << "off" << std::ends;

	renderText(shader, ss.str(), 1, WINDOW_HEIGHT - TEXT_HEIGHT, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << std::fixed << std::setprecision(3);
	ss << "Read Time: " << readTime << " ms" << std::ends;
	renderText(shader, ss.str(), 1, WINDOW_HEIGHT - (2 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Process Time: " << processTime << " ms" << std::ends;
	renderText(shader, ss.str(), 1, WINDOW_HEIGHT - (3 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Press SPACE key to toggle PBO on/off." << std::ends;
	renderText(shader, ss.str(), 1, 1, 0.6f, glm::vec3(0.0f, 0.0f, 1.0f));

	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}
void printTransferRate()
{
	const double INV_MEGA = 1.0 / (1024 * 1024);
	static Timer timer;
	static int count = 0;
	static std::stringstream ss;
	double elapsedTime;

	// ѭ��ֱ��1sʱ�䵽
	elapsedTime = timer.getElapsedTime();
	if (elapsedTime < 1.0)
	{
		++count;
	}
	else
	{
		std::cout << std::fixed << std::setprecision(1);
		std::cout << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE * INV_MEGA << " MB/s. (" << count / elapsedTime << " FPS)\n";
		std::cout << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
		count = 0;                      // ���ü�����
		timer.start();                  // ���¿�ʼ��ʱ��
	}
}
void initColorBuffer()
{
	colorBuffer = new GLubyte[DATA_SIZE];
	memset(colorBuffer, 255, DATA_SIZE);
}
void releaseColorBuffer()
{
	delete[] colorBuffer;
}
void renderText(Shader &shader, std::wstring text,
	GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	shader.use();
	glUniform3f(glGetUniformLocation(shader.programId, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAOId);

	// ���������ַ������ַ�
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		if (unicodeCharacters.count(*c) <= 0)
		{
			continue;
		}
		FontCharacter ch = unicodeCharacters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// ����������ζ�Ӧ�ľ���λ������
		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },

			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// ������ַ���Ӧ������
		glBindTexture(GL_TEXTURE_2D, ch.textureId);
		// ��̬����VBO������ ��Ϊ�����ʾ���ֵľ���λ�������б䶯 ��Ҫ��̬����
		glBindBuffer(GL_ARRAY_BUFFER, textVBOId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// ��Ϊadvance ��1/64���ر�ʾ���뵥λ ����������λ���ʾһ�����ؾ���
		x += (ch.advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void initPixelText()
{
	glGenTextures(1, &pixelTextId);
	glBindTexture(GL_TEXTURE_2D, pixelTextId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)colorBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
}
