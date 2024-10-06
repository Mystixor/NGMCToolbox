#include "stdafx.h"

#include "BasePreviewImage.h"

namespace NGMC
{
	BasePreviewImage::BasePreviewImage(File& file)
		: BasePreview(file),
		m_Image(nullptr), m_ImageInUse(false),
		m_ImageWidth(0), m_ImageHeight(0), m_ImageDisplayWidth(0), m_ImageDisplayHeight(0)
	{
	}

	BasePreviewImage::~BasePreviewImage()
	{
		FreeTexture();
	}

	void BasePreviewImage::UploadTexture(unsigned char* imageData, int imageWidth, int imageHeight, DDSFormat pixelFormat)
	{
		// Create a OpenGL texture identifier
		GLuint imageTexture;
		glGenTextures(1, &imageTexture);
		glBindTexture(GL_TEXTURE_2D, imageTexture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//	linear when zoomed out for nicer visuals
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//	nearest when zoomed in for pixel accuracy

		// Setup texture wrapping parameters so the texture does not repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Upload pixels into texture
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		GLint internalFormat = GL_RGBA;
		GLenum format = GL_RGBA;
		GLenum type = GL_UNSIGNED_BYTE;

		switch (pixelFormat)
		{
		case DDSFormat::BGRA8:
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DDSFormat::RGBA8:
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DDSFormat::DXT1:
			internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			break;
		case DDSFormat::DXT5:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		case DDSFormat::LUMINANCE:
			internalFormat = GL_LUMINANCE;
			format = GL_LUMINANCE;
			break;
		}

		switch (pixelFormat)
		{
		case DDSFormat::BGRA8:
		case DDSFormat::RGBA8:
		case DDSFormat::LUMINANCE:
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, format, type, imageData);
			break;
		case DDSFormat::DXT1:
		{
			GLsizei imageSize = max(1, ((imageWidth + 3) / 4)) * max(1, ((imageHeight + 3) / 4)) * 8;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, imageSize, imageData);
			break;
		}
		case DDSFormat::DXT5:
		{
			GLsizei imageSize = max(1, ((imageWidth + 3) / 4)) * max(1, ((imageHeight + 3) / 4)) * 16;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, imageSize, imageData);
			break;
		}
		}


		m_Image = (void*)(intptr_t)imageTexture;
		m_ImageWidth = imageWidth;
		m_ImageHeight = imageHeight;
		m_ImageInUse = true;
	}

	void BasePreviewImage::FreeTexture()
	{
		if (m_ImageInUse)
		{
			GLuint imageTexture = (GLuint)(intptr_t)m_Image;
			glDeleteTextures(1, &imageTexture);

			m_ImageInUse = false;
		}
	}
}