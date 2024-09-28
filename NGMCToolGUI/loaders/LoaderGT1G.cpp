#include "stdafx.h"

#include "LoaderGT1G.h"

namespace NGMC
{
	extern bool g_ReadColorMapAsLuminance;

	//	public:
	LoaderGT1G::LoaderGT1G(MemoryBuffer& memoryBuffer)
		: m_IsFileInMemory(true),
		m_MemBuf(memoryBuffer), m_FilePath(L"")
	{
	}

	LoaderGT1G::LoaderGT1G(const wchar_t* path)
		: m_IsFileInMemory(false),
		m_MemBuf(), m_FilePath(path)
	{
	}

	LoaderGT1G::LoaderGT1G(File* p_File)
		: m_IsFileInMemory(p_File->IsFileInMemory())
	{
		if (m_IsFileInMemory)
		{
			*this = LoaderGT1G(p_File->GetMemoryBuffer());
		}
		else
		{
			*this = LoaderGT1G(p_File->GetFilePath());
		}
	}

	LoaderGT1G::~LoaderGT1G()
	{
	}

	bool LoaderGT1G::GetHeader(GT1GHeader& outHeader) 
	{
		if(m_IsFileInMemory)
		{
			return GetHeaderFromMemory(outHeader);
		}
		else
		{
			return GetHeaderFromDisk(outHeader);
		}
	}

	GT1GVersion LoaderGT1G::GetVersion()
	{
		if (m_IsFileInMemory)
		{
			return GetVersionFromMemory();
		}
		else
		{
			return GetVersionFromDisk();
		}
	}

	unsigned int LoaderGT1G::GetTextureCount()
	{
		if (m_IsFileInMemory)
		{
			return GetTextureCountFromMemory();
		}
		else
		{
			return GetTextureCountFromDisk();
		}
	}

	unsigned int LoaderGT1G::GetTextureMipMapCount(unsigned int textureIndex)
	{
		if (m_IsFileInMemory)
		{
			return GetMipMapCountFromMemory(textureIndex);
		}
		else
		{
			return GetMipMapCountFromDisk(textureIndex);
		}
	}

	PixelFormat LoaderGT1G::GetTexturePixelFormat(unsigned int textureIndex)
	{
		if (m_IsFileInMemory)
		{
			return GetTexturePixelFormatFromMemory(textureIndex);
		}
		else
		{
			return GetTexturePixelFormatFromDisk(textureIndex);
		}
	}

	uint32_t LoaderGT1G::GetTextureFlags(unsigned int textureIndex)
	{
		if (m_IsFileInMemory)
		{
			return GetTextureFlagsFromMemory(textureIndex);
		}
		else
		{
			return GetTextureFlagsFromDisk(textureIndex);
		}
	}

	bool LoaderGT1G::GetTextureExtraFlags(unsigned int textureIndex, uint32_t* outExtraFlags0, uint32_t* outExtraFlags1, uint32_t* outExtraFlags2)
	{
		if (m_IsFileInMemory)
		{
			return GetTextureExtraFlagsFromMemory(textureIndex, outExtraFlags0, outExtraFlags1, outExtraFlags2);
		}
		else
		{
			return GetTextureExtraFlagsFromDisk(textureIndex, outExtraFlags0, outExtraFlags1, outExtraFlags2);
		}
	}

	bool LoaderGT1G::GetTexturesInfo(GT1GHeader& inHeader, unsigned int* mipMapCounts, PixelFormat* formats, unsigned int* widths, unsigned int* heights, uint32_t* flags, uint32_t* outExtraFlags0s, uint32_t* outExtraFlags1s, uint32_t* outExtraFlags2s)
	{
		if (m_IsFileInMemory)
		{
			return GetTexturesInfoFromMemory(inHeader, mipMapCounts, formats, widths, heights, flags, outExtraFlags0s, outExtraFlags1s, outExtraFlags2s);
		}
		else
		{
			return GetTexturesInfoFromDisk(inHeader, mipMapCounts, formats, widths, heights, flags, outExtraFlags0s, outExtraFlags1s, outExtraFlags2s);
		}
	}

	unsigned int LoaderGT1G::GetTextureMipMapWidth(unsigned int textureIndex, unsigned int mipLevel)
	{
		if (m_IsFileInMemory)
		{
			return GetTextureWidthFromMemory(textureIndex, mipLevel);
		}
		else
		{
			return GetTextureWidthFromDisk(textureIndex, mipLevel);
		}
	}

	unsigned int LoaderGT1G::GetTextureMipMapHeight(unsigned int textureIndex, unsigned int mipLevel)
	{
		if (m_IsFileInMemory)
		{
			return GetTextureHeightFromMemory(textureIndex, mipLevel);
		}
		else
		{
			return GetTextureHeightFromDisk(textureIndex, mipLevel);
		}
	}

	size_t LoaderGT1G::GetMipDataSize(unsigned int textureIndex, unsigned int mipLevel)
	{
		if (m_IsFileInMemory)
		{
			return GetMipDataSizeFromMemory(textureIndex, mipLevel);
		}
		else
		{
			return GetMipDataSizeFromDisk(textureIndex, mipLevel);
		}
	}

	bool LoaderGT1G::GetMipData(MemoryBuffer& outBuffer, unsigned int textureIndex, unsigned int mipLevel)
	{
		if (m_IsFileInMemory)
		{
			return GetMipDataFromMemory(outBuffer, textureIndex, mipLevel);
		}
		else
		{
			return GetMipDataFromDisk(outBuffer, textureIndex, mipLevel);
		}
	}

	bool LoaderGT1G::GetImageData(MemoryBuffer& outBuffer, unsigned int textureIndex)
	{
		if (m_IsFileInMemory)
		{
			return GetImageDataFromMemory(outBuffer, textureIndex);
		}
		else
		{
			return GetImageDataFromDisk(outBuffer, textureIndex);
		}
	}

	//	private:
	bool LoaderGT1G::GetHeaderFromDisk(GT1GHeader& outHeader)
	{
		bool isSuccess = false;

		std::ifstream stream(m_FilePath, std::ios::binary);

		if (stream)
		{
			stream.seekg(0, std::ios_base::beg);
			stream.read((char*)&outHeader, sizeof(outHeader));

			isSuccess = true;
		}

		return isSuccess;
	}

	bool LoaderGT1G::GetHeaderFromMemory(GT1GHeader& outHeader)
	{
		bool isSuccess = false;

		if (m_MemBuf.GetSize() >= sizeof(GT1GHeader))
		{
			uintptr_t pos = m_MemBuf.Tell();

			m_MemBuf.Seek(0x00, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&outHeader, sizeof(outHeader));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);

			isSuccess = true;
		}

		return isSuccess;
	}

	GT1GVersion LoaderGT1G::GetVersionFromDisk()
	{
		GT1GVersion version = GT1GVersion();

		std::ifstream stream(m_FilePath, std::ios::binary);

		if (stream)
		{
			stream.seekg(offsetof(GT1GHeader, GT1GHeader::version), std::ios_base::beg);
			stream.read((char*)&version, sizeof(version));
		}

		return version;
	}

	GT1GVersion LoaderGT1G::GetVersionFromMemory()
	{
		GT1GVersion version = GT1GVersion();

		if (m_MemBuf.GetSize() >= 4 + sizeof(version))
		{
			uintptr_t pos = m_MemBuf.Tell();

			m_MemBuf.Seek(4, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&version, sizeof(version));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return version;
	}

	unsigned int LoaderGT1G::GetTextureCountFromDisk()
	{
		uint32_t textureCount = 0U;

		std::ifstream stream(m_FilePath, std::ios::binary);
		
		if (stream)
		{
			stream.seekg(offsetof(GT1GHeader, GT1GHeader::textureCount), std::ios_base::beg);
			stream.read((char*)&textureCount, sizeof(textureCount));
		}

		return textureCount;
	}

	unsigned int LoaderGT1G::GetTextureCountFromMemory()
	{
		uint32_t textureCount = 0U;

		if (m_MemBuf.GetSize() >= offsetof(GT1GHeader, GT1GHeader::textureCount) + sizeof(textureCount))
		{
			uintptr_t pos = m_MemBuf.Tell();

			m_MemBuf.Seek(offsetof(GT1GHeader, GT1GHeader::textureCount), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureCount, sizeof(textureCount));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return textureCount;
	}

	unsigned int LoaderGT1G::GetMipMapCountFromDisk(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				uint8_t mipMapCount;
				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;

				return mipMapCount;
			}
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetMipMapCountFromMemory(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			uint8_t mipMapCount;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;

			m_MemBuf.Seek(pos, MemoryBuffer::beg);

			return mipMapCount;
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureWidthFromDisk(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				uint8_t mipMapCount;
				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;

				if (mipLevel < mipMapCount)
				{
					uint16_t width;
					stream.seekg(1, std::ios_base::cur);
					stream.read((char*)&width, sizeof(width));

					return std::pow(2, (width & 0x0F) - mipLevel);
				}
			}
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureWidthFromMemory(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			uint8_t mipMapCount;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint16_t width;
				m_MemBuf.Seek(1, MemoryBuffer::cur);
				m_MemBuf.Read((char*)&width, sizeof(width));

				m_MemBuf.Seek(pos, MemoryBuffer::beg);

				return std::pow(2, (width & 0x0F) - mipLevel);
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureHeightFromDisk(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				uint8_t mipMapCount;
				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;

				if (mipLevel < mipMapCount)
				{
					uint16_t height;
					stream.seekg(1, std::ios_base::cur);
					stream.read((char*)&height, sizeof(height));

					return std::pow(2, (height >> 4) - mipLevel);
				}
			}
		}

		return 0;
	}

	unsigned int LoaderGT1G::GetTextureHeightFromMemory(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			uint8_t mipMapCount;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint16_t height;
				m_MemBuf.Seek(1, MemoryBuffer::cur);
				m_MemBuf.Read((char*)&height, sizeof(height));

				m_MemBuf.Seek(pos, MemoryBuffer::beg);

				return std::pow(2, (height >> 4) - mipLevel);
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	PixelFormat LoaderGT1G::GetTexturePixelFormatFromDisk(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				char format;
				stream.seekg(header.textureOffsetTableAddress + textureOffset + 1, std::ios_base::beg);
				stream.read(&format, sizeof(char));

				return (PixelFormat)format;
			}
		}

		return PixelFormat::ColorMap_u8;
	}
	
	PixelFormat LoaderGT1G::GetTexturePixelFormatFromMemory(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			char format;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset + 1, MemoryBuffer::beg);
			m_MemBuf.Read(&format, sizeof(char));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);

			return (PixelFormat)format;
		}

		return PixelFormat::ColorMap_u8;
	}

	uint32_t LoaderGT1G::GetTextureFlagsFromDisk(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				uint32_t flags;
				stream.seekg(header.textureOffsetTableAddress + textureOffset + 4, std::ios_base::beg);
				stream.read((char*)&flags, sizeof(flags));

				return flags;
			}
		}

		return 0;
	}

	uint32_t LoaderGT1G::GetTextureFlagsFromMemory(unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			uint32_t flags;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset + 4, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&flags, sizeof(flags));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);

			return flags;
		}

		return 0;
	}

	bool LoaderGT1G::GetTextureExtraFlagsFromDisk(unsigned int textureIndex, uint32_t* outExtraFlags0, uint32_t* outExtraFlags1, uint32_t* outExtraFlags2)
	{
		GT1GHeader header;
		if (!GetHeaderFromDisk(header))
			return false;

		bool isSuccess = false;
		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				uint32_t flags;
				stream.seekg(header.textureOffsetTableAddress + textureOffset + 4, std::ios_base::beg);
				stream.read((char*)&flags, sizeof(flags));

				if ((flags >> 24) == 0x10)
				{
					stream.read((char*)outExtraFlags0, sizeof(uint32_t));
					stream.read((char*)outExtraFlags1, sizeof(uint32_t));
					stream.read((char*)outExtraFlags2, sizeof(uint32_t));

					isSuccess = true;
				}
			}
		}

		return isSuccess;
	}

	bool LoaderGT1G::GetTextureExtraFlagsFromMemory(unsigned int textureIndex, uint32_t* outExtraFlags0, uint32_t* outExtraFlags1, uint32_t* outExtraFlags2)
	{
		GT1GHeader header;
		if (!GetHeaderFromMemory(header))
			return false;

		bool isSuccess = false;
		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			uint32_t flags;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset + 4, MemoryBuffer::beg);
			m_MemBuf.Read((char*)&flags, sizeof(flags));

			if ((flags >> 24) == 0x10)
			{
				m_MemBuf.Read((char*)outExtraFlags0, sizeof(uint32_t));
				m_MemBuf.Read((char*)outExtraFlags1, sizeof(uint32_t));
				m_MemBuf.Read((char*)outExtraFlags2, sizeof(uint32_t));

				isSuccess = true;
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

	bool LoaderGT1G::GetTexturesInfoFromDisk(GT1GHeader& inHeader, unsigned int* mipMapCounts, PixelFormat* formats, unsigned int* widths, unsigned int* heights, uint32_t* flags, uint32_t* outExtraFlags0s, uint32_t* outExtraFlags1s, uint32_t* outExtraFlags2s)
	{
		bool isSuccess = false;

		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				for (unsigned int i = 0; i < inHeader.textureCount; i++)
				{
					uint32_t textureOffset;
					stream.seekg(inHeader.textureOffsetTableAddress + i * sizeof(textureOffset), std::ios_base::beg);
					stream.read((char*)&textureOffset, sizeof(textureOffset));

					stream.seekg((size_t)inHeader.textureOffsetTableAddress + textureOffset, std::ios_base::beg);

					uint8_t mipMapCount;
					stream.read((char*)&mipMapCount, sizeof(mipMapCount));
					mipMapCount = mipMapCount >> 4;
					mipMapCounts[i] = (unsigned int)mipMapCount;

					uint8_t format;
					stream.read((char*)&format, sizeof(format));
					formats[i] = (PixelFormat)format;

					uint16_t dimensions;
					stream.read((char*)&dimensions, sizeof(dimensions));
					widths[i] = (unsigned int)std::pow(2, (dimensions & 0x0F));
					heights[i] = (unsigned int)std::pow(2, (dimensions >> 4));

					stream.read((char*)&flags[i], sizeof(uint32_t));

					if ((flags[i] >> 24) == 0x10)
					{
						stream.read((char*)&outExtraFlags0s[i], sizeof(uint32_t));
						stream.read((char*)&outExtraFlags1s[i], sizeof(uint32_t));
						stream.read((char*)&outExtraFlags2s[i], sizeof(uint32_t));
					}
				}

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderGT1G::GetTexturesInfoFromMemory(GT1GHeader& inHeader, unsigned int* mipMapCounts, PixelFormat* formats, unsigned int* widths, unsigned int* heights, uint32_t* flags, uint32_t* outExtraFlags0s, uint32_t* outExtraFlags1s, uint32_t* outExtraFlags2s)
	{
		bool isSuccess = false;

		{
			uintptr_t pos = m_MemBuf.Tell();

			for (unsigned int i = 0; i < inHeader.textureCount; i++)
			{
				uint32_t textureOffset;
				m_MemBuf.Seek(inHeader.textureOffsetTableAddress + i * sizeof(textureOffset), MemoryBuffer::beg);
				m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

				m_MemBuf.Seek((size_t)inHeader.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

				uint8_t mipMapCount;
				m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;
				mipMapCounts[i] = (unsigned int)mipMapCount;

				uint8_t format;
				m_MemBuf.Read((char*)&format, sizeof(format));
				formats[i] = (PixelFormat)format;

				uint16_t dimensions;
				m_MemBuf.Read((char*)&dimensions, sizeof(dimensions));
				widths[i] = (unsigned int)std::pow(2, (dimensions & 0x0F));
				heights[i] = (unsigned int)std::pow(2, (dimensions >> 4));

				m_MemBuf.Read((char*)&flags[i], sizeof(uint32_t));

				if ((flags[i] >> 24) == 0x10)
				{
					m_MemBuf.Read((char*)&outExtraFlags0s[i], sizeof(uint32_t));
					m_MemBuf.Read((char*)&outExtraFlags1s[i], sizeof(uint32_t));
					m_MemBuf.Read((char*)&outExtraFlags2s[i], sizeof(uint32_t));
				}
			}

			isSuccess = true;

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

	size_t LoaderGT1G::GetMipDataSizeFromDisk(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);

				uint8_t mipMapCount;
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;

				if (mipLevel < mipMapCount)
				{
					uint8_t format;
					stream.read((char*)&format, sizeof(format));

					uint16_t dimensions;
					stream.read((char*)&dimensions, sizeof(dimensions));
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

					return dataSize;
				}
			}
		}

		return 0;
	}

	size_t LoaderGT1G::GetMipDataSizeFromMemory(unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount;
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint8_t format;
				m_MemBuf.Read((char*)&format, sizeof(format));

				uint16_t dimensions;
				m_MemBuf.Read((char*)&dimensions, sizeof(dimensions));
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

				m_MemBuf.Seek(pos, MemoryBuffer::beg);

				return dataSize;
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return 0;
	}

	bool LoaderGT1G::GetMipDataFromDisk(MemoryBuffer& outBuffer, unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);

				uint8_t mipMapCount;
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;

				if (mipLevel < mipMapCount)
				{
					uint8_t format;
					stream.read((char*)&format, sizeof(format));

					uint16_t dimensions;
					stream.read((char*)&dimensions, sizeof(dimensions));
					unsigned int width = std::pow(2, (dimensions & 0x0F));
					unsigned int height = std::pow(2, (dimensions >> 4));

					uint32_t flags;
					stream.read((char*)&flags, sizeof(flags));

					if ((flags >> 24) == 0x10)
					{
						stream.seekg(12, std::ios_base::cur);
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
							stream.seekg(dataSize, std::ios_base::cur);
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
							stream.read(indexBuffer.PrepareBuffer(dataSize), dataSize);
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
						stream.read(outBuffer.PrepareBuffer(dataSize), dataSize);
					}


					return true;
				}
			}
		}

		return false;
	}

	bool LoaderGT1G::GetMipDataFromMemory(MemoryBuffer& outBuffer, unsigned int textureIndex, unsigned int mipLevel)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount;
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;

			if (mipLevel < mipMapCount)
			{
				uint8_t format;
				m_MemBuf.Read((char*)&format, sizeof(format));

				uint16_t dimensions;
				m_MemBuf.Read((char*)&dimensions, sizeof(dimensions));
				unsigned int width = std::pow(2, (dimensions & 0x0F));
				unsigned int height = std::pow(2, (dimensions >> 4));

				uint32_t flags;
				m_MemBuf.Read((char*)&flags, sizeof(flags));

				if ((flags >> 24) == 0x10)
				{
					m_MemBuf.Seek(12, MemoryBuffer::cur);
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
						m_MemBuf.Seek(dataSize, MemoryBuffer::cur);
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
						m_MemBuf.Read(indexBuffer.PrepareBuffer(dataSize), dataSize);
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
					m_MemBuf.Read(outBuffer.PrepareBuffer(dataSize), dataSize);
				}

				m_MemBuf.Seek(pos, MemoryBuffer::beg);

				return true;
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}

		return false;
	}
	
	bool LoaderGT1G::GetImageDataFromDisk(MemoryBuffer& outBuffer, unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromDisk(header);

		if (textureIndex < header.textureCount)
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				uint32_t textureOffset;
				stream.seekg(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), std::ios_base::beg);
				stream.read((char*)&textureOffset, sizeof(textureOffset));

				stream.seekg(header.textureOffsetTableAddress + textureOffset, std::ios_base::beg);

				uint8_t mipMapCount;
				stream.read((char*)&mipMapCount, sizeof(mipMapCount));
				mipMapCount = mipMapCount >> 4;


				uint8_t format;
				stream.read((char*)&format, sizeof(format));

				uint16_t dimensions;
				stream.read((char*)&dimensions, sizeof(dimensions));
				unsigned int width = std::pow(2, (dimensions & 0x0F));
				unsigned int height = std::pow(2, (dimensions >> 4));

				uint32_t flags;
				stream.read((char*)&flags, sizeof(flags));

				if ((flags >> 24) == 0x10)
				{
					stream.seekg(12, std::ios_base::cur);
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
						stream.read(indexBuffer.PrepareBuffer(dataSize), dataSize);
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
					stream.read(outBuffer.PrepareBuffer(dataSize), dataSize);
				}


				return true;
			}
		}

		return false;
	}

	bool LoaderGT1G::GetImageDataFromMemory(MemoryBuffer& outBuffer, unsigned int textureIndex)
	{
		GT1GHeader header;
		GetHeaderFromMemory(header);

		if (textureIndex < header.textureCount)
		{
			uintptr_t pos = m_MemBuf.Tell();

			uint32_t textureOffset;
			m_MemBuf.Seek(header.textureOffsetTableAddress + textureIndex * sizeof(textureOffset), MemoryBuffer::beg);
			m_MemBuf.Read((char*)&textureOffset, sizeof(textureOffset));

			m_MemBuf.Seek(header.textureOffsetTableAddress + textureOffset, MemoryBuffer::beg);

			uint8_t mipMapCount;
			m_MemBuf.Read((char*)&mipMapCount, sizeof(mipMapCount));
			mipMapCount = mipMapCount >> 4;


			uint8_t format;
			m_MemBuf.Read((char*)&format, sizeof(format));

			uint16_t dimensions;
			m_MemBuf.Read((char*)&dimensions, sizeof(dimensions));
			unsigned int width = std::pow(2, (dimensions & 0x0F));
			unsigned int height = std::pow(2, (dimensions >> 4));

			uint32_t flags;
			m_MemBuf.Read((char*)&flags, sizeof(flags));

			if ((flags >> 24) == 0x10)
			{
				m_MemBuf.Seek(12, MemoryBuffer::cur);
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
					m_MemBuf.Read(indexBuffer.PrepareBuffer(dataSize), dataSize);
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
				m_MemBuf.Read(outBuffer.PrepareBuffer(dataSize), dataSize);
			}

			m_MemBuf.Seek(pos, MemoryBuffer::beg);

			return true;
		}

		return false;
	}
}