#include "stdafx.h"
#include "LoaderDDS.h"

namespace NGMC
{
	LoaderDDS::LoaderDDS(MemoryBuffer& memoryBuffer)
	{
		m_Reader.Init(memoryBuffer);
	}

	LoaderDDS::LoaderDDS(const wchar_t* path)
	{
		m_Reader.Init(path);
	}

	LoaderDDS::LoaderDDS(File* p_File)
	{
		m_Reader.Init(*p_File);
	}

	LoaderDDS::~LoaderDDS()
	{
	}

	bool LoaderDDS::GetHeader(DDS_HEADER& outHeader)
	{
		bool isSuccess = false;

		if (m_Reader.GetFileSize() >= 4 + sizeof(DDS_HEADER))
		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(0, MemoryBuffer::beg);

			DWORD magic = 0;
			m_Reader.ReadValue(magic);

			if (magic == ddsMagic)
			{
				m_Reader.ReadValue(outHeader);

				isSuccess = true;
			}

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

	bool LoaderDDS::GetMipData(MemoryBuffer& outBuffer, unsigned int mipLevel)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeader(ddsHeader))
		{
			if (mipLevel < ddsHeader.dwMipMapCount)
			{
				m_Reader.Seek(4 + sizeof(DDS_HEADER), MemoryBuffer::beg);

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
						m_Reader.Seek(dataSize, MemoryBuffer::cur);
					}
					else
					{
						break;
					}
				}

				m_Reader.ReadArray(outBuffer.PrepareBuffer(dataSize), dataSize);

				isSuccess = true;
			}
		}

		return isSuccess;
	}

	bool LoaderDDS::GetImageData(MemoryBuffer& outBuffer)
	{
		bool isSuccess = false;

		DDS_HEADER ddsHeader;
		if (GetHeader(ddsHeader))
		{
			m_Reader.Seek(4 + sizeof(DDS_HEADER), MemoryBuffer::beg);

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

			m_Reader.ReadArray(outBuffer.PrepareBuffer(dataSize), dataSize);

			isSuccess = true;
		}

		return isSuccess;
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
}