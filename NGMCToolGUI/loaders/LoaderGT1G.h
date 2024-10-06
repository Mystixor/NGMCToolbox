#pragma once

#include "MemoryBuffer.h"
#include "File.h"
#include "DataReader.h"

namespace NGMC
{
	namespace GT1G
	{
		//	The GT1G version struct
		struct Version
		{
			uint8_t major, submajor, minor, patch;
		};

		//	The GT1G file header struct
		struct Header
		{
			char magic[4];
			Version version;
			uint32_t fileSize;
			uint32_t textureOffsetTableAddress;
			uint32_t textureCount;
			uint32_t dat_14, dat_18;
		};

		//	Formats used in GT1G texture files
		enum PixelFormat
		{
			RGBA8_BGRA_u8 = 0x1,
			RGBA8_RGBA_u8 = 0x2,
			CompressedRgbS3tcDxt1Ext_06 = 0x06,
			CompressedRgbS3tcDxt1Ext_59 = 0x59,
			CompressedRgbaS3tcDxt5Ext_08 = 0x08,
			CompressedRgbaS3tcDxt5Ext_5B = 0x5B,
			ColorMap_u8 = 0x72,
			unsupportedFormat
		};
	}

	//	Builds a GT1G file from the specified count of textures, raw image datas, pixel widths and heights, counts of mipmaps and pixel formats and loads its raw data into the specified outBuffer.
	static bool BuildGT1G(MemoryBuffer& outBuffer, unsigned int textureCount, MemoryBuffer* imageDatas, unsigned int* widths, unsigned int* heights, unsigned int* mipMapCounts, GT1G::PixelFormat* formats)
	{
		bool isSuccess = false;

		size_t fileSize = sizeof(GT1G::Header) + (size_t)2 * textureCount * sizeof(uint32_t);
		std::vector<MemoryBuffer> textureHeaderDatas(textureCount);

		for (unsigned int i = 0; i < textureCount; i++)
		{
			uint8_t widthExponent = std::log2(widths[i]);
			uint8_t heightExponent = std::log2(heights[i]);
			if (mipMapCounts[i] > 0xF || widthExponent > 0xF || heightExponent > 0xF || std::pow(2, widthExponent) != widths[i] || std::pow(2, heightExponent) != heights[i])
			{
				return false;
			}

			uint32_t flags;
			uint32_t extraFlags0;
			uint32_t extraFlags1;
			uint32_t extraFlags2;
			size_t textureHeaderDataSize;
			switch (formats[i])
			{
			case GT1G::PixelFormat::ColorMap_u8:
				flags		= 0x10211000;
				extraFlags0 = 0x0000000C;
				extraFlags1 = 0x00000000;
				extraFlags2 = 0x00000000;
				textureHeaderDataSize = (size_t)0x08 + 0x0C;
				break;
			default:
				flags		= 0x00211000;
				extraFlags0	= 0x00000000;
				extraFlags1	= 0x00000000;
				extraFlags2	= 0x00000000;
				textureHeaderDataSize = (size_t)0x08;
			}

			fileSize += textureHeaderDataSize + imageDatas[i].GetSize();
			textureHeaderDatas[i].PrepareBuffer(textureHeaderDataSize);

			textureHeaderDatas[i].Seek(0, MemoryBuffer::beg);

			uint8_t mipMapCount = (mipMapCounts[i] << 4) & 0xF0;
			textureHeaderDatas[i].Write((char*)&mipMapCount, sizeof(mipMapCount));

			textureHeaderDatas[i].Write((char*)&formats[i], sizeof(uint8_t));

			uint16_t dimensions = ((heightExponent << 4) & 0xF0) | widthExponent;
			textureHeaderDatas[i].Write((char*)&dimensions, sizeof(dimensions));

			textureHeaderDatas[i].Write((char*)&flags, sizeof(flags));

			if ((flags >> 24) == 0x10)
			{
				textureHeaderDatas[i].Write((char*)&extraFlags0, sizeof(extraFlags0));
				textureHeaderDatas[i].Write((char*)&extraFlags1, sizeof(extraFlags1));
				textureHeaderDatas[i].Write((char*)&extraFlags2, sizeof(extraFlags2));
			}
		}

		outBuffer.PrepareBuffer(fileSize);

		outBuffer.Seek(0, MemoryBuffer::beg);
		outBuffer.Write("GT1G", 4);
		outBuffer.Write("0600", 4);
		outBuffer.Write((char*)&fileSize, 4);

		size_t offset = textureCount * sizeof(uint32_t);			// size of both normalMapFlags and offsetTable EACH
		size_t offsetTableAddress = sizeof(GT1G::Header) + offset;	// sizeof(GT1GHeader) + "sizeof(normalMapFlags)"

		outBuffer.Write((char*)&offsetTableAddress, 4);
		outBuffer.Write((char*)&textureCount, 4);

		unsigned int U00 = 0x0A;
		unsigned int U01 = 0x00;
		outBuffer.Write((char*)&U00, 4);
		outBuffer.Write((char*)&U01, 4);

		//	To-do: Write normalMapFlags here

		outBuffer.Seek(offsetTableAddress, MemoryBuffer::beg);
		for (unsigned int i = 0; i < textureCount; i++)
		{
			outBuffer.Write((char*)&offset, sizeof(uint32_t));
			offset += textureHeaderDatas[i].GetSize() + imageDatas[i].GetSize();
		}

		for (unsigned int i = 0; i < textureCount; i++)
		{
			outBuffer.Write(textureHeaderDatas[i].GetBaseAddress(), textureHeaderDatas[i].GetSize());
			outBuffer.Write(imageDatas[i].GetBaseAddress(), imageDatas[i].GetSize());
		}

		isSuccess = true;

		return isSuccess;
	}
	
	//	Class for loading data from a GT1G file.
	class LoaderGT1G
	{
	public:
		//	Constructs a GT1G Loader object from the raw memory of a GT1G texture file.
		LoaderGT1G(MemoryBuffer& memoryBuffer);
		
		//	Constructs a GT1G Loader object with a wide char string path of a GT1G texture file.
		LoaderGT1G(const wchar_t* path);
		
		//	Constructs a GT1G Loader object with a GT1G file object.
		LoaderGT1G(File* p_File);

		//	Deconstructs the GT1G Loader object.
		~LoaderGT1G();

		//	Loads the header into the specified outHeader.
		bool				GetHeader(GT1G::Header& outHeader);

		//	Returns the file format version.
		GT1G::Version		GetVersion();

		//	Returns the count of textures in the file.
		unsigned int		GetTextureCount();

		//	Returns the count of mipmaps in the texture at the specified texture index.
		unsigned int		GetTextureMipMapCount(unsigned int textureIndex);

		//	Returns the PixelFormat of the texture at the specified texture index.
		GT1G::PixelFormat	GetTexturePixelFormat(unsigned int textureIndex);

		//	Returns the texture flags of the texture at the specified texture index.
		uint32_t			GetTextureFlags(unsigned int textureIndex);

		//	Loads the extra texture flags of the texture at the specified texture index into the memory at the specified addresses if the extra flags are set, returns whether the extra flags are set.
		bool				GetTextureExtraFlags(unsigned int textureIndex, uint32_t* outExtraFlags0, uint32_t* outExtraFlags1, uint32_t* outExtraFlags2);
		
		//	Loads the header as well as the counts of mipmaps, the formats, pixel widths and heights, flags and extra flags into the memory at the specified addresses of all textures in the file.
		bool				GetTexturesInfo(GT1G::Header& inHeader, unsigned int* mipMapCounts, GT1G::PixelFormat* formats, unsigned int* widths, unsigned int* heights, uint32_t* flags, uint32_t* outExtraFlags0s, uint32_t* outExtraFlags1s, uint32_t* outExtraFlags2s);
		
		//	Returns the width in pixels of the specified mip level at the specified texture index.
		unsigned int		GetTextureMipMapWidth(unsigned int textureIndex, unsigned int mipLevel = 0U);
		
		//	Returns the height in pixels of the specified mip level at the specified texture index.
		unsigned int		GetTextureMipMapHeight(unsigned int textureIndex, unsigned int mipLevel = 0U);
		
		//	Returns the size of the raw image data of the specified mip level at the specified texture index.
		size_t				GetMipDataSize(unsigned int textureIndex, unsigned int mipLevel = 0U);
		
		//	Loads the raw image data of the specified mip level at the specified texture index into the specified outBuffer, returns whether the operation was successful.
		bool				GetMipData(MemoryBuffer& outBuffer, unsigned int textureIndex, unsigned int mipLevel = 0U);
		
		//	Loads the raw image data including all mipmaps of the texture at the specified texture index into the specified outBuffer, returns whether the operation was successful.
		bool				GetImageData(MemoryBuffer& outBuffer, unsigned int textureIndex);
		
	private:
		//	The DataReader object responsible for reading data from the file associated with the loader object.
		DataReader m_Reader;
	};
}