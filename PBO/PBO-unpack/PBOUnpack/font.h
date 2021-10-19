#ifndef _OPENGL_FONT_H_
#define _OPENGL_FONT_H_

#include <GLEW/glew.h>
#include <ft2build.h>
#include <GLM/glm.hpp>
#include <map>
#include <string>
#include <iostream>

#include FT_FREETYPE_H

struct FontCharacter
{
	GLuint textureId; // �洢��������
	glm::ivec2 size;  // ���δ�С
	glm::ivec2 bearing; // bearingx bearingy
	GLuint advance;  // �����¸����ε�ˮƽ����
};

class FontResourceManager
{
private:
	typedef std::map<std::string, FT_Face> FontFaceMapType;
public:
	static FontResourceManager& getInstance()
	{
		static FontResourceManager instance;
		return instance;
	}
	~FontResourceManager()
	{
		for (FontFaceMapType::const_iterator it = _faceMap.begin(); it != _faceMap.end(); ++it)
		{
			if (it->second)
			{
				FT_Done_Face(it->second);
			}
		}
		FT_Done_FreeType(_ft);
	}
private:
	FontResourceManager()
	{
		if (FT_Init_FreeType(&_ft))
		{
			std::cerr << "ERROR::FontResourceManager: Could not init FreeType Library" << std::endl;
		}	
	}
	FontResourceManager(FontResourceManager const&){} // ����ģʽ��ʵ��
	void operator=(FontResourceManager const&){} // ����ģʽ��ʵ��
public:
	void loadFont(const std::string& fontName, const std::string& fontPath)
	{
		if (NULL != getFontFace(fontName))
		{
			return;
		}
		FT_Face face;
		if (FT_New_Face(_ft, fontPath.c_str(), 0, &face))
		{
			std::cerr << "ERROR::FontResourceManager: Failed to load font with name=" 
				<< fontPath << std::endl;
			return;
		}
		_faceMap[fontName] = face;
		FT_Select_Charmap(face, FT_ENCODING_UNICODE); // unicode����
	}
	bool loadASCIIChar(const std::string& fontName, const int fontSize,
		std::map<wchar_t, FontCharacter>& characters)
	{
		// ��Ϊ����ʹ��1���ֽڵ���ɫ�������洢���� ���Խ��Ĭ�ϵ�4�ֽڶ�������
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		FT_Face face = getFontFace(fontName);
		if (NULL == face)
		{
			std::cerr << "ERROR::FontResourceManager: Failed to get font with name="
				<< fontName << std::endl;
			return false;
		}
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		for (GLubyte c = 0; c < 255; c++)
		{
			if (!isprint(c))
			{
				continue;
			}
			// �����ַ������� 
			// FT_LOAD_RENDER  ѡ���֪freeType����һ�� 8-bit grayscale bitmap image
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cerr << "ERROR::FontResourceManager: Failed to load Glyph with char=" << c << std::endl;
				continue;
			}
			// Ϊ�����Ӧ�����δ���һ������ �����Ա���������
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
				);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			FontCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::pair<GLchar, FontCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}
	bool loadUnicodeText(const std::string& fontName, const int fontSize,
		const std::wstring& text,
		std::map<wchar_t, FontCharacter>& characters)
	{
		// ��Ϊ����ʹ��1���ֽڵ���ɫ�������洢���� ���Խ��Ĭ�ϵ�4�ֽڶ�������
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		FT_Face face = getFontFace(fontName);
		if (NULL == face)
		{
			std::cerr << "ERROR::FontResourceManager: Failed to get font with name="
				<< fontName << std::endl;
			return false;
		}
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
		{
			wchar_t wchar = *it;
			// �����ַ������� 
			// FT_LOAD_RENDER  ѡ���֪freeType����һ�� 8-bit grayscale bitmap image
			if (FT_Load_Char(face, wchar, FT_LOAD_RENDER))
			{
				std::cerr << "ERROR::FontResourceManager: Failed to load Glyph with char=" << wchar << std::endl;
				continue;
			}
			// Ϊ�����Ӧ�����δ���һ������ �����Ա���������
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
				);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FontCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::pair<wchar_t, FontCharacter>(wchar, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}
private:
	FT_Face getFontFace(const std::string& fontName)
	{
		if (_faceMap.count(fontName) > 0)
		{
			return _faceMap[fontName];
		}
		return NULL;
	}
private:
	FT_Library _ft;
	FontFaceMapType _faceMap;
};


#endif