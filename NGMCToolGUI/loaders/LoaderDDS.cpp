#include "stdafx.h"
#include "LoaderDDS.h"

namespace NGMC
{
	//	public:
	LoaderDDS::LoaderDDS(MemoryBuffer& memoryBuffer)
		: m_IsFileInMemory(true),
		m_MemBuf(memoryBuffer), m_FilePath(L"")
	{
	}

	LoaderDDS::LoaderDDS(const wchar_t* path)
		: m_IsFileInMemory(false),
		m_MemBuf(), m_FilePath(path)
	{
	}

	LoaderDDS::LoaderDDS(File* p_File)
		: m_IsFileInMemory(p_File->IsFileInMemory())
	{
		if (m_IsFileInMemory)
		{
			*this = LoaderDDS(p_File->GetMemoryBuffer());
		}
		else
		{
			*this = LoaderDDS(p_File->GetFilePath());
		}
	}

	LoaderDDS::~LoaderDDS()
	{
	}

	bool LoaderDDS::GetHeader(DDS_HEADER& outHeader)
	{
		if (m_IsFileInMemory)
		{
			return GetHeaderFromMemory(outHeader);
		}
		else
		{
			return GetHeaderFromDisk(outHeader);
		}
	}

	bool LoaderDDS::GetMipData(MemoryBuffer& outBuffer, unsigned int mipLevel)
	{
		if (m_IsFileInMemory)
		{
			return GetMipDataFromMemory(outBuffer, mipLevel);
		}
		else
		{
			return GetMipDataFromDisk(outBuffer, mipLevel);
		}
	}

	bool LoaderDDS::GetImageData(MemoryBuffer& outBuffer)
	{
		if (m_IsFileInMemory)
		{
			return GetImageDataFromMemory(outBuffer);
		}
		else
		{
			return GetImageDataFromDisk(outBuffer);
		}
	}

	DDSFormat LoaderDDS::GetPixelFormat()
	{
		DDSFormat format = DDSFormat::unsupported;

		DDS_HEADER ddsHeader;
		if (GetHeader(ddsHeader))
		{
			if (ddsHeader.ddspf.dwFlags & DDPF_FOURCC)
			{
				switch (ddsHeader.ddspf.dwFourCC)
				{
				case '1TXD':
				{
					format = DDSFormat::DXT1;
					break;
				}
				case '5TXD':
					format = DDSFormat::DXT5;
					break;
				}
			}
			else
			{
				if (ddsHeader.ddspf.dwFlags & DDPF_LUMINANCE)
				{
					format = DDSFormat::LUMINANCE;
				}
				else if (ddsHeader.ddspf.dwFlags & DDPF_ALPHAPIXELS && ddsHeader.ddspf.dwFlags & DDPF_RGB)
				{
					if (ddsHeader.ddspf.dwGBitMask == 0x0000FF00 && ddsHeader.ddspf.dwABitMask == 0xFF000000)
					{
						if (ddsHeader.ddspf.dwRBitMask == 0x000000FF && ddsHeader.ddspf.dwBBitMask == 0x00FF0000)
						{
							format = DDSFormat::RGBA8;
						}
						else if (ddsHeader.ddspf.dwRBitMask == 0x00FF0000 && ddsHeader.ddspf.dwBBitMask == 0x000000FF)
						{
							format = DDSFormat::BGRA8;
						}
					}
				}
			}
		}

		return format;
	}

	//	private:
	bool LoaderDDS::GetHeaderFromDisk(DDS_HEADER& outHeader)
	{
		bool isSuccess = false;

		std::ifstream inStream(m_FilePath, std::ios::binary);

		if (inStream)
		{
			inStream.seekg(0, std::ios_base::beg);

			DWORD magic;
			inStream.read((char*)&magic, 4);

			if (magic == ddsMagic)
			{
				inStream.read((char*)&outHeader, sizeof(DDS_HEADER));

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetHeaderFromMemory(DDS_HEADER& outHeader)
	{
		bool isSuccess = false;

		if (m_MemBuf.GetSize() >= 4 + sizeof(DDS_HEADER))
		{
			char* ddsData = m_MemBuf.GetBaseAddress();

			if (*(DWORD*)ddsData == ddsMagic)
			{
				memcpy((void*)&outHeader, (void*)((uintptr_t)ddsData + 4), sizeof(DDS_HEADER));

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetMipDataFromDisk(MemoryBuffer& outBuffer, unsigned int mipLevel)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeaderFromDisk(ddsHeader))
		{
			if (mipLevel < ddsHeader.dwMipMapCount)
			{
				std::ifstream inStream(m_FilePath, std::ios::binary);

				if (inStream)
				{
					inStream.seekg(4 + sizeof(DDS_HEADER), std::ios_base::beg);

					bool isCompressed = false;
					size_t blockSize = 0;

					if (ddsHeader.ddspf.dwFlags & DDPF_FOURCC)
					{
						isCompressed = true;
						switch (ddsHeader.ddspf.dwFourCC)
						{
						case '1TXD':
						{
							//	"DXT1"
							blockSize = 8;
							break;
						}
						case '5TXD':
							//	"DXT5"
							blockSize = 16;
							break;
						}
					}

					size_t dataSize = 0;
					unsigned int denom = 1;
					for (unsigned int i = 0; true; i++)
					{
						if (isCompressed)
						{
							dataSize = max(1, ((ddsHeader.dwWidth / denom + 3) / 4)) * max(1, ((ddsHeader.dwHeight / denom + 3) / 4)) * blockSize;
						}
						else
						{
							dataSize = (size_t)(ddsHeader.dwWidth / denom) * (ddsHeader.dwHeight / denom) * ddsHeader.ddspf.dwRGBBitCount / 8;
						}

						if (i < mipLevel)
						{
							denom *= 2;
							inStream.seekg(dataSize, std::ios_base::cur);
						}
						else
						{
							break;
						}
					}

					inStream.read(outBuffer.PrepareBuffer(dataSize), dataSize);

					isSuccess = true;
				}
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetMipDataFromMemory(MemoryBuffer& outBuffer, unsigned int mipLevel)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeaderFromMemory(ddsHeader))
		{
			if (mipLevel < ddsHeader.dwMipMapCount)
			{
				m_MemBuf.Seek(4 + sizeof(DDS_HEADER), MemoryBuffer::beg);

				bool isCompressed = false;
				size_t blockSize = 0;

				if (ddsHeader.ddspf.dwFlags & DDPF_FOURCC)
				{
					isCompressed = true;
					switch (ddsHeader.ddspf.dwFourCC)
					{
					case '1TXD':
					{
						//	"DXT1"
						blockSize = 8;
						break;
					}
					case '5TXD':
						//	"DXT5"
						blockSize = 16;
						break;
					}
				}

				size_t dataSize = 0;
				unsigned int denom = 1;
				for (unsigned int i = 0; true; i++)
				{
					if (isCompressed)
					{
						dataSize = max(1, ((ddsHeader.dwWidth / denom + 3) / 4)) * max(1, ((ddsHeader.dwHeight / denom + 3) / 4)) * blockSize;
					}
					else
					{
						dataSize = (ddsHeader.dwWidth / denom) * (ddsHeader.dwHeight / denom) * ddsHeader.ddspf.dwRGBBitCount / 8;
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

				m_MemBuf.Read(outBuffer.PrepareBuffer(dataSize), dataSize);

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetImageDataFromDisk(MemoryBuffer& outBuffer)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeaderFromDisk(ddsHeader))
		{
			std::ifstream inStream(m_FilePath, std::ios::binary);

			if (inStream)
			{
				inStream.seekg(4 + sizeof(DDS_HEADER), std::ios_base::beg);

				bool isCompressed = false;
				size_t blockSize = 0;

				if (ddsHeader.ddspf.dwFlags & DDPF_FOURCC)
				{
					isCompressed = true;
					switch (ddsHeader.ddspf.dwFourCC)
					{
					case '1TXD':
					{
						//	"DXT1"
						blockSize = 8;
						break;
					}
					case '5TXD':
						//	"DXT5"
						blockSize = 16;
						break;
					}
				}

				size_t dataSize = 0;
				unsigned int denom = 1;
				for (unsigned int i = 0; i < ddsHeader.dwMipMapCount; i++)
				{
					if (isCompressed)
					{
						dataSize += max(1, ((ddsHeader.dwWidth / denom + 3) / 4)) * max(1, ((ddsHeader.dwHeight / denom + 3) / 4)) * blockSize;
					}
					else
					{
						dataSize += (size_t)(ddsHeader.dwWidth / denom) * (ddsHeader.dwHeight / denom) * ddsHeader.ddspf.dwRGBBitCount / 8;
					}

					denom *= 2;
				}

				inStream.read(outBuffer.PrepareBuffer(dataSize), dataSize);

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetImageDataFromMemory(MemoryBuffer& outBuffer)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeaderFromMemory(ddsHeader))
		{
			m_MemBuf.Seek(4 + sizeof(DDS_HEADER), MemoryBuffer::beg);

			bool isCompressed = false;
			size_t blockSize = 0;

			if (ddsHeader.ddspf.dwFlags & DDPF_FOURCC)
			{
				isCompressed = true;
				switch (ddsHeader.ddspf.dwFourCC)
				{
				case '1TXD':
				{
					//	"DXT1"
					blockSize = 8;
					break;
				}
				case '5TXD':
					//	"DXT5"
					blockSize = 16;
					break;
				}
			}

			size_t dataSize = 0;
			unsigned int denom = 1;
			for (unsigned int i = 0; i < ddsHeader.dwMipMapCount; i++)
			{
				if (isCompressed)
				{
					dataSize += max(1, ((ddsHeader.dwWidth / denom + 3) / 4)) * max(1, ((ddsHeader.dwHeight / denom + 3) / 4)) * blockSize;
				}
				else
				{
					dataSize += (ddsHeader.dwWidth / denom) * (ddsHeader.dwHeight / denom) * ddsHeader.ddspf.dwRGBBitCount / 8;
				}

				denom *= 2;
			}

			m_MemBuf.Read(outBuffer.PrepareBuffer(dataSize), dataSize);

			isSuccess = true;
		}

		return isSuccess;
	}
}