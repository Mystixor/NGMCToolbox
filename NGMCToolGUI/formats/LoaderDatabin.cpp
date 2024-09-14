#include "stdafx.h"

#include "LoaderDatabin.h"

#include "zlib.h"

namespace NGMC
{
	LoaderDatabin::LoaderDatabin(GAME game)
		: m_Game(game), m_FilePath(L"")
	{
	}

	LoaderDatabin::LoaderDatabin(GAME game, std::wstring& filePath)
		: m_Game(game), m_FilePath(filePath)
	{
	}

	LoaderDatabin::~LoaderDatabin()
	{
	}

	unsigned int LoaderDatabin::GetFileCount() const
	{
		return m_FileHeaderOffsets.size();
	}

	uint32_t LoaderDatabin::GetFileHeaderOffset(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0U;
		}
		return m_FileHeaderOffsets[index];
	}

	FileType LoaderDatabin::GetFileType(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return FileType();
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return FileType((S1::FileTypeId)m_FileHeadersS1[index].type);
		case SIGMA_2:
			return FileType((S2::FileTypeId)m_FileHeadersS2[index].type);
		default:
			return FileType();
		}
	}

	std::uintmax_t LoaderDatabin::GetFileSize(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return m_FileHeadersS1[index].size;
		case SIGMA_2:
			return m_FileHeadersS2[index].size;
		default:
			return 0ULL;
		}
	}
	
	std::uintmax_t LoaderDatabin::GetFileSizeCompressed(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return m_FileHeadersS1[index].sizeCompressed;
		case SIGMA_2:
			return m_FileHeadersS2[index].sizeCompressed;
		default:
			return 0ULL;
		}
	}

	void LoaderDatabin::SetFilePath(const wchar_t* filePath)
	{
		m_FilePath = filePath;
	}

	bool LoaderDatabin::LoadHeader(DatabinHeader& outHeader)
	{
		std::ifstream stream(m_FilePath, std::ios::binary);
	
		if (stream)
		{
			stream.read((char*)&outHeader, sizeof(DatabinHeader));
			return true;
		}
		return false;
	}
	
	bool LoaderDatabin::LoadHeader(DatabinHeader& outHeader, std::ifstream& inStream)
	{
		if (inStream)
		{
			inStream.read((char*)&outHeader, sizeof(DatabinHeader));
			return true;
		}
		return false;
	}
	
	bool LoaderDatabin::LoadHeader(DatabinHeader& outHeader, MemoryBuffer& inBuffer)
	{
		if (inBuffer.GetSize() > 0)
		{
			inBuffer.Read((char*)&outHeader, sizeof(DatabinHeader));
			return true;
		}
		return false;
	}

	bool LoaderDatabin::LoadItemHeader(S1::DatabinItemHeader& outItemHeader, unsigned int index)
	{
		bool isSuccess = false;

		std::ifstream stream(m_FilePath, std::ios::binary);

		if (stream)
		{
			DatabinHeader* header = new DatabinHeader;
			stream.read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				stream.seekg(index * sizeof(uint32_t), std::ios_base::cur);

				uint32_t fileHeaderOffset = 0;
				stream.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				stream.seekg(header->headerSize + fileHeaderOffset);
				stream.read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}

	bool LoaderDatabin::LoadItemHeader(S1::DatabinItemHeader& outItemHeader, std::ifstream& inStream, unsigned int index)
	{
		bool isSuccess = false;

		if (inStream)
		{
			DatabinHeader* header = new DatabinHeader;
			inStream.seekg(0, std::ios_base::beg);
			inStream.read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				inStream.seekg(index * sizeof(uint32_t), std::ios_base::cur);

				uint32_t fileHeaderOffset = 0;
				inStream.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inStream.seekg(header->headerSize + fileHeaderOffset);
				inStream.read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}

	bool LoaderDatabin::LoadItemHeader(S1::DatabinItemHeader& outItemHeader, MemoryBuffer& inBuffer, unsigned int index)
	{
		bool isSuccess = false;

		if (inBuffer.GetSize() > 0)
		{
			DatabinHeader* header = new DatabinHeader;
			inBuffer.Seek(0, MemoryBuffer::beg);
			inBuffer.Read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				inBuffer.Seek(index * sizeof(uint32_t), MemoryBuffer::cur);

				uint32_t fileHeaderOffset = 0;
				inBuffer.Read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inBuffer.Seek(header->headerSize + fileHeaderOffset);
				inBuffer.Read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}
	
	bool LoaderDatabin::LoadItemHeader(S2::DatabinItemHeader& outItemHeader, unsigned int index)
	{
		bool isSuccess = false;

		std::ifstream stream(m_FilePath, std::ios::binary);

		if (stream)
		{
			DatabinHeader* header = new DatabinHeader;
			stream.read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				stream.seekg(index * sizeof(uint32_t), std::ios_base::cur);

				uint32_t fileHeaderOffset = 0;
				stream.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				stream.seekg(header->headerSize + fileHeaderOffset);
				stream.read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}

	bool LoaderDatabin::LoadItemHeader(S2::DatabinItemHeader& outItemHeader, std::ifstream& inStream, unsigned int index)
	{
		bool isSuccess = false;

		if (inStream)
		{
			DatabinHeader* header = new DatabinHeader;
			inStream.seekg(0, std::ios_base::beg);
			inStream.read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				inStream.seekg(index * sizeof(uint32_t), std::ios_base::cur);

				uint32_t fileHeaderOffset = 0;
				inStream.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inStream.seekg(header->headerSize + fileHeaderOffset);
				inStream.read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}

	bool LoaderDatabin::LoadItemHeader(S2::DatabinItemHeader& outItemHeader, MemoryBuffer& inBuffer, unsigned int index)
	{
		bool isSuccess = false;

		if (inBuffer.GetSize() > 0)
		{
			DatabinHeader* header = new DatabinHeader;
			inBuffer.Seek(0, MemoryBuffer::beg);
			inBuffer.Read((char*)header, sizeof(DatabinHeader));

			if (index < header->fileCount)
			{
				inBuffer.Seek(index * sizeof(uint32_t), MemoryBuffer::cur);

				uint32_t fileHeaderOffset = 0;
				inBuffer.Read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inBuffer.Seek(header->headerSize + fileHeaderOffset);
				inBuffer.Read((char*)&outItemHeader, sizeof(outItemHeader));

				isSuccess = true;
			}

			delete header;
		}
		return isSuccess;
	}

	bool LoaderDatabin::LoadItemHeaders(std::ifstream& inStream)
	{
		if (inStream)
		{
			DatabinHeader header = DatabinHeader();// = new DatabinHeader;
			inStream.seekg(0, std::ios_base::beg);
			inStream.read((char*)&header, sizeof(DatabinHeader));

			size_t pos = inStream.tellg();

			unsigned int extractCount = 0;
			for (unsigned int i = 0; i < header.fileCount; i++)
			{
				inStream.seekg(pos + i * sizeof(uint32_t), std::ios_base::beg);

				m_FileHeaderOffsets.push_back(0U);

				inStream.read((char*)&m_FileHeaderOffsets[i], sizeof(uint32_t));

				inStream.seekg(header.headerSize + m_FileHeaderOffsets[i]);

				switch (m_Game)
				{
				case SIGMA_1:
					m_FileHeadersS1.push_back(S1::DatabinItemHeader());
					inStream.read((char*)&m_FileHeadersS1[i], sizeof(S1::DatabinItemHeader));
					break;
				case SIGMA_2:
					m_FileHeadersS2.push_back(S2::DatabinItemHeader());
					inStream.read((char*)&m_FileHeadersS2[i], sizeof(S2::DatabinItemHeader));
					break;
				default:
					//	Unsupported game.
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool LoaderDatabin::LoadItemHeaders(MemoryBuffer& inBuffer)
	{
		if (inBuffer.GetSize() > 0)
		{
			DatabinHeader header = DatabinHeader();
			inBuffer.Seek(0, MemoryBuffer::beg);
			inBuffer.Read((char*)&header, sizeof(DatabinHeader));

			size_t pos = inBuffer.Tell();

			unsigned int extractCount = 0;
			for (unsigned int i = 0; i < header.fileCount; i++)
			{
				inBuffer.Seek(pos + i * sizeof(uint32_t), MemoryBuffer::beg);

				m_FileHeaderOffsets.push_back(0U);

				inBuffer.Read((char*)&m_FileHeaderOffsets[i], sizeof(uint32_t));

				inBuffer.Seek(header.headerSize + m_FileHeaderOffsets[i]);

				switch (m_Game)
				{
				case SIGMA_1:
					m_FileHeadersS1.push_back(S1::DatabinItemHeader());
					inBuffer.Read((char*)&m_FileHeadersS1[i], sizeof(S1::DatabinItemHeader));
					break;
				case SIGMA_2:
					m_FileHeadersS2.push_back(S2::DatabinItemHeader());
					inBuffer.Read((char*)&m_FileHeadersS2[i], sizeof(S2::DatabinItemHeader));
					break;
				default:
					//	Unsupported game.
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool LoaderDatabin::LoadItemHeaders()
	{
		std::ifstream stream(m_FilePath, std::ios::binary);

		return LoadItemHeaders(stream);
	}

	bool LoaderDatabin::DecompressItem(MemoryBuffer& outBuffer, std::ifstream& inStream, unsigned int index)
	{
		bool isSuccess = false;

		if (inStream)
		{
			DatabinHeader header;

			inStream.seekg(0, std::ios_base::beg);
			inStream.read((char*)&header, sizeof(DatabinHeader));

			if (index < header.fileCount)
			{
				inStream.seekg(index * sizeof(uint32_t), std::ios_base::cur);

				uint32_t fileHeaderOffset;
				inStream.read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inStream.seekg(header.headerSize + fileHeaderOffset);

				uint32_t offset = 0U;
				uint32_t size = 0U;
				uint32_t sizeCompressed = 0U;
				switch (m_Game)
				{
				case SIGMA_1:
				{
					S1::DatabinItemHeader fileHeader = S1::DatabinItemHeader();
					inStream.read((char*)&fileHeader, sizeof(S1::DatabinItemHeader));
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				case SIGMA_2:
				{
					S2::DatabinItemHeader fileHeader = S2::DatabinItemHeader();
					inStream.read((char*)&fileHeader, sizeof(S2::DatabinItemHeader));
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				default:
					//	Unsupported game.
					return false;
				}

				char* fileDataRaw = new char[sizeCompressed];
				inStream.seekg(header.headerSize + header.fileDataOffset + offset);
				inStream.read(fileDataRaw, sizeCompressed);


				int ret;
				//unsigned have;
				z_stream strm;

				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK)
					return ret;

				strm.avail_in = sizeCompressed;
				strm.next_in = (Bytef*)fileDataRaw;

				strm.avail_out = size;
				strm.next_out = (Bytef*)outBuffer.PrepareBuffer(size);

				ret = inflate(&strm, Z_NO_FLUSH);

				isSuccess = true;

				//assert(ret != Z_STREAM_ERROR);
				switch (ret)
				{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR; [[fallthrough]];
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					isSuccess = false;
					//return ret;
				}

				(void)inflateEnd(&strm);

				delete[] fileDataRaw;
			}
		}
		return isSuccess;
	}

	bool LoaderDatabin::DecompressItem(MemoryBuffer& outBuffer, MemoryBuffer& inBuffer, unsigned int index)
	{
		bool isSuccess = false;

		if (inBuffer.GetSize() > 0)
		{
			DatabinHeader header;

			inBuffer.Seek(0, MemoryBuffer::beg);
			inBuffer.Read((char*)&header, sizeof(DatabinHeader));

			if (index < header.fileCount)
			{
				inBuffer.Seek(index * sizeof(uint32_t), MemoryBuffer::cur);

				uint32_t fileHeaderOffset;
				inBuffer.Read((char*)&fileHeaderOffset, sizeof(uint32_t));

				inBuffer.Seek(header.headerSize + fileHeaderOffset);

				uint32_t offset = 0U;
				uint32_t size = 0U;
				uint32_t sizeCompressed = 0U;
				switch (m_Game)
				{
				case SIGMA_1:
				{
					S1::DatabinItemHeader fileHeader = S1::DatabinItemHeader();
					inBuffer.Read((char*)&fileHeader, sizeof(S1::DatabinItemHeader));
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				case SIGMA_2:
				{
					S2::DatabinItemHeader fileHeader = S2::DatabinItemHeader();
					inBuffer.Read((char*)&fileHeader, sizeof(S2::DatabinItemHeader));
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				default:
					//	Unsupported game.
					return false;
				}

				char* fileDataRaw = new char[sizeCompressed];
				inBuffer.Seek(header.headerSize + header.fileDataOffset + offset);
				inBuffer.Read(fileDataRaw, sizeCompressed);


				int ret;
				//unsigned have;
				z_stream strm;

				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK)
					return ret;

				strm.avail_in = sizeCompressed;
				strm.next_in = (Bytef*)fileDataRaw;

				strm.avail_out = size;
				strm.next_out = (Bytef*)outBuffer.PrepareBuffer(size);

				ret = inflate(&strm, Z_NO_FLUSH);

				isSuccess = true;

				//assert(ret != Z_STREAM_ERROR);
				switch (ret)
				{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR; [[fallthrough]];
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					isSuccess = false;
					//return ret;
				}

				(void)inflateEnd(&strm);

				delete[] fileDataRaw;
			}
		}
		return isSuccess;
	}

	bool LoaderDatabin::DecompressItem(MemoryBuffer& outBuffer, unsigned int index)
	{
		std::ifstream stream(m_FilePath, std::ios::binary);

		return DecompressItem(outBuffer, stream, index);
	}
}