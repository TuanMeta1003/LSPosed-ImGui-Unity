#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct TextureInfo {
	ImTextureID textureId;
	int width;
	int height;
};

TextureInfo createTextureFromMem(const unsigned char *buf, int len) {
	int w, h, n;
	stbi_uc *data = stbi_load_from_memory(buf, len, &w, &h, &n, 0);
	if (!data) {
		return {0, 0, 0};
	}

	GLenum format;
	if (n == 3) {
		format = GL_RGB;
	} else if (n == 4) {
		format = GL_RGBA;
	} else {
		stbi_image_free(data);
		return {0, 0, 0};
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	TextureInfo textureInfo;
	textureInfo.textureId = (ImTextureID)texture;
	textureInfo.width = w;
	textureInfo.height = h;

	return textureInfo;
}