#pragma once

#include <glad/glad.h>

#include "stb_image_impl.h"

#include <iostream>

class Texture2D
{
private:
	unsigned int m_TextureID;
	int m_width, m_height;
	unsigned char* data;
	int m_nrChannels;

public:
	Texture2D() = default;

	void load(GLenum wrapType, GLint minFilter, GLint magFilter, const std::string textureFile, GLint internalFormat, GLenum format);

	unsigned int getTextureID() const;

	void bindTexture() const;

	void loadTexture(char const* path);
};

void Texture2D::load(GLenum wrapType, GLint minFilter, GLint magFilter, const std::string textureFile, GLint internalFormat, GLenum format)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(textureFile.c_str(), &m_width, &m_height, &m_nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	else
	{
		std::cout << "Failed to load texture: " << textureFile << std::endl;
	}

	stbi_image_free(data);
}

unsigned int Texture2D::getTextureID() const
{
	return m_TextureID;
}

void Texture2D::bindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture2D::loadTexture(char const* path)
{
	glGenTextures(1, &m_TextureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else
			exit(-1);

		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
}