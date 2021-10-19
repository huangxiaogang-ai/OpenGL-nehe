#ifndef SIMPLE_OBJ_LOADER_H_
#define SIMPLE_OBJ_LOADER_H_
/*/
 * ��ֻ��һ��ʾ���ļ�obj������ 
 * ��ʵ�õ�obj����Բ鿴https://github.com/syoyo/tinyobjloader                                                                     
*/
#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>       
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "shader.h"


// ��ʾһ����������
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

// ��ʾobj�ļ���һ�������λ�á���������ͷ����� ����
struct VertexCombineIndex
{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

// ��ʾһ��������Ⱦ����Сʵ��
class Mesh
{
public:
	void draw(Shader& shader) // ����Mesh
	{
		shader.use();
		glBindVertexArray(this->VAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureId);
		glUniform1i(glGetUniformLocation(shader.programId, "tex"), 0);

		glDrawArrays(GL_TRIANGLES, 0, this->vertData.size());

		glBindVertexArray(0);
		glUseProgram(0);
	}
	Mesh(){}
	Mesh(const std::vector<Vertex>& vertData, GLint textureId, bool bShowData = false) // ����һ��Mesh
	{
		this->vertData = vertData;
		this->textureId = textureId;
		this->setupMesh();
		if (bShowData)
		{
			const char * fileName = "vert-data.txt";
			std::ofstream file(fileName);
			for (std::vector<Vertex>::const_iterator it = this->vertData.begin(); it != this->vertData.end(); ++it)
			{
				file << glm::to_string(it->position) << ","
					<< glm::to_string(it->texCoords) << ","
					<< glm::to_string(it->normal) << std::endl;
			}
			file.close();
			std::cout << " vert data saved in file:" << fileName << std::endl;
		}
	}
	~Mesh()
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
	}
public:
	std::vector<Vertex> vertData;
	GLuint VAOId, VBOId;
	GLint textureId;

	void setupMesh()  // ����VAO,VBO�Ȼ�����
	{
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* this->vertData.size(),
			&this->vertData[0], GL_STATIC_DRAW);
		// ����λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// ������������
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// ���㷨��������
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class ObjLoader
{
public:
	static bool loadFromFile(const char* path,
		std::vector<Vertex>& vertData)
	{
		
		std::vector<VertexCombineIndex> vertComIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_textCoords;
		std::vector<glm::vec3> temp_normals;

		std::ifstream file(path);
		if (!file)
		{
			std::cerr << "Error::ObjLoader, could not open obj file:"
				<< path << " for reading." << std::endl;
			return false;
		}
		std::string line;
		while (getline(file, line))
		{
			if (line.substr(0, 2) == "vt") // ����������������
			{
				std::istringstream s(line.substr(2));
				glm::vec2 v;
				s >> v.x; 
				s >> v.y;
				v.y = -v.y;  // ע��������ص�dds���� Ҫ��y���з�ת
				temp_textCoords.push_back(v);
			}
			else if (line.substr(0, 2) == "vn") // ���㷨��������
			{
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_normals.push_back(v);
			}
			else if (line.substr(0, 1) == "v") // ����λ������
			{
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_vertices.push_back(v);
			}
			else if (line.substr(0, 1) == "f") // ������
			{
				std::istringstream vtns(line.substr(2));
				std::string vtn;
				while (vtns >> vtn) // ����һ���ж����������
				{
					VertexCombineIndex vertComIndex;
					std::replace(vtn.begin(), vtn.end(), '/', ' ');
					std::istringstream ivtn(vtn);
					if (vtn.find("  ") != std::string::npos) // û����������
					{
						std::cerr << "Error:ObjLoader, no texture data found within file:"
							<< path << std::endl;
						return false;
					}
					ivtn >> vertComIndex.posIndex
						>> vertComIndex.textCoordIndex
						>> vertComIndex.normIndex;
					
					vertComIndex.posIndex--;
					vertComIndex.textCoordIndex--;
					vertComIndex.normIndex--;
					vertComIndices.push_back(vertComIndex);
				}
			}
			else if (line[0] == '#') // ע�ͺ���
			{ }
			else  
			{
				// �������� ��ʱ������
			}
		}
		for (std::vector<GLuint>::size_type i = 0; i < vertComIndices.size(); ++i)
		{
			Vertex vert;
			VertexCombineIndex comIndex = vertComIndices[i];

			vert.position = temp_vertices[comIndex.posIndex];
			vert.texCoords = temp_textCoords[comIndex.textCoordIndex];
			vert.normal = temp_normals[comIndex.normIndex];

			vertData.push_back(vert);
		}

		return true;
	}
};


#endif