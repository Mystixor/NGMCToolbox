#include "stdafx.h"

#include "LoaderGT1G.h"

namespace NGMC
{
	using namespace GT1G;

	extern bool g_ReadColorMapAsLuminance;


	LoaderGT1G::LoaderGT1G(MemoryBuffer& memoryBuffer)
	{
		m_Reader.Init(memoryBuffer);
	}
	
	LoaderGT1G::LoaderGT1G(const wchar_t* path)
	{
		m_Reader.Init(path);
	}
	
	LoaderGT1G::LoaderGT1G(File* p_File)
	{
		m_Reader.Init(*p_File);
	}

	LoaderGT1G::~LoaderGT1G()
	{
	}

	bool LoaderGT1G::GetHeader(Header& outHeader)
	{
		bool isSuccess = false;

		if (m_Reader.GetFileSize() >= sizeof(Header))
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(0x00, MemoryBuffer::beg);
			m_Reader.ReadValue(outHeader);

			m_Reader.Seek(pos, MemoryBuffer::beg);

			isSuccess = true;
		}

		return isSuccess;
	}

	Version LoaderGT1G::GetVersion()
	{
		Version version = Version();

		if (m_Reader.GetFileSize() >= 4 + sizeof(version))
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(4, MemoryBuffer::beg);
			m_Reader.ReadValue(version);

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return version;
	}

	unsigned int LoaderGT1G::GetTextureCount()
	{
		uint32_t textureCount = 0U;

		if (m_Reader.GetFileSize() >= offsetof(Header, Header::textureCount) + sizeof(textureCount))
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(offsetof(Header, Header::textureCount), MemoryBuffer::beg);
			textureCount = m_Reader.ReadUInt32();

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return textureCount;
	}

	unsigned int LoaderGT1G::GetTextureMipMapCount(unsigned int textureIndex)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;

			m_Reader.Seek(pos, MemoryBuffer::beg);

			return mipMapCount;
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureMipMapWidth(unsigned int textureIndex, unsigned int mipLevel)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				m_Reader.Seek(1, MemoryBuffer::cur);
				uint16_t width = m_Reader.ReadUInt16();

				m_Reader.Seek(pos, MemoryBuffer::beg);

				return std::pow(2, (width & 0x0F) - mipLevel);
			}

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureMipMapHeight(unsigned int textureIndex, unsigned int mipLevel)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				m_Reader.Seek(1, MemoryBuffer::cur);
				uint16_t height = m_Reader.ReadUInt16();

				m_Reader.Seek(pos, MemoryBuffer::beg);

				return std::pow(2, (height >> 4) - mipLevel);
			}

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	PixelFormat LoaderGT1G::GetTexturePixelFormat(unsigned int textureIndex)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset + 1, MemoryBuffer::beg);
			uint8_t format = m_Reader.ReadUInt8();

			m_Reader.Seek(pos, MemoryBuffer::beg);

			return (PixelFormat)format;
		}

		return PixelFormat::ColorMap_u8;
	}

	uint32_t LoaderGT1G::GetTextureFlags(unsigned int textureIndex)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset + 4, MemoryBuffer::beg);
			uint32_t flags = m_Reader.ReadUInt32();

			m_Reader.Seek(pos, MemoryBuffer::beg);

			return flags;
		}

		return 0;
	}

	bool LoaderGT1G::GetTextureExtraFlags(unsigned int textureIndex, uint32_t* outExtraFlags0, uint32_t* outExtraFlags1, uint32_t* outExtraFlags2)
	{
		Header header;
		if (!GetHeader(header))
			return false;

		bool isSuccess = false;
		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset + 4, MemoryBuffer::beg);
			uint32_t flags = m_Reader.ReadUInt32();

			if ((flags >> 24) == 0x10)
			{
				*outExtraFlags0 = m_Reader.ReadUInt32();
				*outExtraFlags1 = m_Reader.ReadUInt32();
				*outExtraFlags2 = m_Reader.ReadUInt32();

				isSuccess = true;
			}

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

	bool LoaderGT1G::GetTexturesInfo(Header& inHeader, unsigned int* mipMapCounts, PixelFormat* formats, unsigned int* widths, unsigned int* heights, uint32_t* flags, uint32_t* outExtraFlags0s, uint32_t* outExtraFlags1s, uint32_t* outExtraFlags2s)
	{
		bool isSuccess = false;

		{
			std::streamoff pos = m_Reader.Tell();

			for (unsigned int i = 0; i < inHeader.textureCount; i++)
			{
				m_Reader.Seek(inHeader.textureOffsetTableAddress + i * sizeof(uint32_t), MemoryBuffer::beg);
				uint32_t textureOffset = m_Reader.ReadUInt32();

				m_Reader.Seek((size_t)inHeader.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

				uint8_t mipMapCount = m_Reader.ReadUInt8();
				mipMapCount = mipMapCount >> 4;
				mipMapCounts[i] = (unsigned int)mipMapCount;

				uint8_t format = m_Reader.ReadUInt8();
				formats[i] = (PixelFormat)format;

				uint16_t dimensions = m_Reader.ReadUInt16();
				widths[i] = (unsigned int)std::pow(2, (dimensions & 0x0F));
				heights[i] = (unsigned int)std::pow(2, (dimensions >> 4));

				flags[i] = m_Reader.ReadUInt32();

				if ((flags[i] >> 24) == 0x10)
				{
					outExtraFlags0s[i] = m_Reader.ReadUInt32();
					outExtraFlags1s[i] = m_Reader.ReadUInt32();
					outExtraFlags2s[i] = m_Reader.ReadUInt32();
				}
			}

			isSuccess = true;

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

	size_t LoaderGT1G::GetMipDataSize(unsigned int textureIndex, unsigned int mipLevel)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint8_t format = m_Reader.ReadUInt8();

				uint16_t dimensions = m_Reader.ReadUInt16();
				unsigned int width = std::pow(2, (dimensions & 0x0F));
				unsigned int height = std::pow(2, (dimensions >> 4));

				size_t blockSize = 0;
				bool isCompressed = false;
				switch ((PixelFormat)format)
				{
				case PixelFormat::RGBA8_BGRA_u8:
				case PixelFormat::RGBA8_RGBA_u8:
				{
					blockSize = 32;
					break;
				}
				case PixelFormat::ColorMap_u8:
				{
					blockSize = 8;
					break;
				}
				case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
				case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
				{
					isCompressed = true;
					switch ((PixelFormat)format)
					{
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						blockSize = 8;
						break;
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						blockSize = 16;
						break;
					}

					break;
				}
				}

				size_t dataSize = 0;
				unsigned int denom = std::pow(2, mipLevel);
				if (isCompressed)
				{
					dataSize = max(1, ((width / denom + 3) / 4)) * max(1, ((height / denom + 3) / 4)) * blockSize;
				}
				else
				{
					dataSize = (width / denom) * (height / denom) * blockSize / 8;
				}

				if (((PixelFormat)format == PixelFormat::ColorMap_u8) && (textureIndex == 1) && (!g_ReadColorMapAsLuminance))
				{
					dataSize *= 4;
				}

				m_Reader.Seek(pos, MemoryBuffer::beg);

				return dataSize;
			}

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	bool LoaderGT1G::GetMipData(MemoryBuffer& outBuffer, unsigned int textureIndex, unsigned int mipLevel)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek((std::streamoff)header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint8_t format = m_Reader.ReadUInt8();

				uint16_t dimensions = m_Reader.ReadUInt16();
				unsigned int width = std::pow(2, (dimensions & 0x0F));
				unsigned int height = std::pow(2, (dimensions >> 4));

				uint32_t flags = m_Reader.ReadUInt32();

				if ((flags >> 24) == 0x10)
				{
					m_Reader.Seek(12, MemoryBuffer::cur);
				}

				size_t blockSize = 0;
				bool isCompressed = false;
				switch ((PixelFormat)format)
				{
				case PixelFormat::RGBA8_BGRA_u8:
				case PixelFormat::RGBA8_RGBA_u8:
				{
					blockSize = 32;
					break;
				}
				case PixelFormat::ColorMap_u8:
				{
					blockSize = 8;
					break;
				}
				case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
				case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
				{
					isCompressed = true;
					switch ((PixelFormat)format)
					{
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						blockSize = 8;
						break;
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						blockSize = 16;
						break;
					}

					break;
				}
				}

				size_t dataSize = 0;
				unsigned int denom = 1;
				for (unsigned int i = 0; true; i++)
				{
					if (isCompressed)
					{
						dataSize = max(1, ((width / denom + 3) / 4)) * max(1, ((height / denom + 3) / 4)) * blockSize;
					}
					else
					{
						dataSize = (width / denom) * (height / denom) * blockSize / 8;
					}

					if (i < mipLevel)
					{
						denom *= 2;
						m_Reader.Seek(dataSize, MemoryBuffer::cur);
					}
					else
					{
						break;
					}
				}

				if (((PixelFormat)format == PixelFormat::ColorMap_u8) && (textureIndex == 1) && (!g_ReadColorMapAsLuminance))
				{
					//	Hack(?) to render PixelFormat::ColorMap_u8
					MemoryBuffer colorBuffer;
					if (GetMipData(colorBuffer, 0, 0))
					{
						MemoryBuffer indexBuffer;
						m_Reader.ReadArray(indexBuffer.PrepareBuffer(dataSize), dataSize);
						char* outData = outBuffer.PrepareBuffer(dataSize * 4);
						char* indexData = indexBuffer.GetBaseAddress();
						char* colorData = colorBuffer.GetBaseAddress();

						for (unsigned int v = 0; v < height; v++)
						{
							for (unsigned int u = 0; u < width; u++)
							{
								unsigned char colorIndex = 0;
								memcpy((void*)&colorIndex, (void*)((uintptr_t)indexData + v * width + u), sizeof(char));
								memcpy((void*)((uintptr_t)outData + (v * width + u) * 4), (void*)((uintptr_t)colorData + colorIndex * 4), 4);
							}
						}
					}
				}
				else
				{
					m_Reader.ReadArray(outBuffer.PrepareBuffer(dataSize), dataSize);
				}

				return true;
			}
		}

		return false;
	}

	bool LoaderGT1G::GetImageData(MemoryBuffer& outBuffer, unsigned int textureIndex)
	{
		Header header;
		GetHeader(header);

		if (textureIndex < header.textureCount)
		{
			m_Reader.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(uint32_t), MemoryBuffer::beg);
			uint32_t textureOffset = m_Reader.ReadUInt32();

			m_Reader.Seek((std::streamoff)header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount = m_Reader.ReadUInt8();
			mipMapCount = mipMapCount >> 4;


			uint8_t format = m_Reader.ReadUInt8();

			uint16_t dimensions = m_Reader.ReadUInt16();
			unsigned int width = std::pow(2, (dimensions & 0x0F));
			unsigned int height = std::pow(2, (dimensions >> 4));

			uint32_t flags = m_Reader.ReadUInt32();

			if ((flags >> 24) == 0x10)
			{
				m_Reader.Seek(12, MemoryBuffer::cur);
			}

			size_t blockSize = 0;
			bool isCompressed = false;
			switch ((PixelFormat)format)
			{
			case PixelFormat::RGBA8_BGRA_u8:
			case PixelFormat::RGBA8_RGBA_u8:
			{
				blockSize = 32;
				break;
			}
			case PixelFormat::ColorMap_u8:
			{
				blockSize = 8;
				break;
			}
			case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
			case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
			case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
			case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
			{
				isCompressed = true;
				switch ((PixelFormat)format)
				{
				case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
				case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
					blockSize = 8;
					break;
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
					blockSize = 16;
					break;
				}

				break;
			}
			}

			size_t dataSize = 0;
			unsigned int denom = 1;
			for (unsigned int i = 0; i < mipMapCount; i++)
			{
				if (isCompressed)
				{
					dataSize += max(1, ((width / denom + 3) / 4)) * max(1, ((height / denom + 3) / 4)) * blockSize;
				}
				else
				{
					dataSize += (width / denom) * (height / denom) * blockSize / 8;
				}

				denom *= 2;
			}

			if (((PixelFormat)format == PixelFormat::ColorMap_u8) && (textureIndex == 1) && (!g_ReadColorMapAsLuminance))
			{
				//	Hack(?) to render PixelFormat::ColorMap_u8
				MemoryBuffer colorBuffer;
				if (GetMipData(colorBuffer, 0, 0))
				{
					MemoryBuffer indexBuffer;
					m_Reader.ReadArray(indexBuffer.PrepareBuffer(dataSize), dataSize);
					char* outData = outBuffer.PrepareBuffer(dataSize * 4);
					char* indexData = indexBuffer.GetBaseAddress();
					char* colorData = colorBuffer.GetBaseAddress();

					for (unsigned int v = 0; v < height; v++)
					{
						for (unsigned int u = 0; u < width; u++)
						{
							unsigned char colorIndex = 0;
							memcpy((void*)&colorIndex, (void*)((uintptr_t)indexData + v * width + u), sizeof(char));
							memcpy((void*)((uintptr_t)outData + (v * width + u) * 4), (void*)((uintptr_t)colorData + colorIndex * 4), 4);
						}
					}
				}
			}
			else
			{
				m_Reader.ReadArray(outBuffer.PrepareBuffer(dataSize), dataSize);
			}

			return true;
		}

		return false;
	}
}